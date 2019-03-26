/*
 * main.c
 * 
 * Bootstrap the STM32F103C8T6 and get things moving.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

/*
 * PIN ASSIGNMENTS:
 * 
 * A8:       Reset line
 * A9-10:    Serial Tx,Rx
 * A15,B3-4: Delay select (B3:A15->short, B3:B4->long, neither->medium)
 * B5-7:     Volume select (B6:B5->quiet, B6:B7->loud, neither->medium)
 * B8:       Speaker output (active high)
 * B12-14:   ROM select
 * C13:      Indicator LED (active low)
 */

int EXC_reset(void) __attribute__((alias("main")));

/* Guard the stacks with known values. */
static void canary_init(void)
{
    _irq_stackbottom[0] = _thread_stackbottom[0] = 0xdeadbeef;
}

/* Has either stack been clobbered? */
static void canary_check(void)
{
    ASSERT(_irq_stackbottom[0] == 0xdeadbeef);
    ASSERT(_thread_stackbottom[0] == 0xdeadbeef);
}

/* Sample the reset line this frequently. */
#define TICK_MS 50
static struct timer tick_timer;

/* Number of image switches that main thread has pending to apply. */
static volatile unsigned int nr_switches;

/* Switch delay time: 0 = small, 1 = medium (default), 2 = long. */
static unsigned int switch_delay = 1; /* medium delay */

static void tick_fn(void *data)
{
    /* For how many milliseconds have consecutive samples been in reset? */
    static unsigned int reset_ms = 0;

    /* Are we in reset now (ie. PA8 low)? */
    if (!gpio_read_pin(gpioa, 8)) {
        /* If so, update our sample count and check if we're at threshold. */
        reset_ms += TICK_MS;
        if (reset_ms >= (ksw_config.reset_delays[switch_delay])) {
            /* If we're at threshold inform main thread and restart count. */
            nr_switches++;
            reset_ms = 0;
        }
    } else {
        /* We're not in reset. Clear the count. */
        reset_ms = 0;
    }

    timer_set(&tick_timer, tick_timer.deadline + time_ms(TICK_MS));
}

/* Address the specified ROM bank on high-order EPROM address pins. */
void rom_bank_set(unsigned int bank)
{
    unsigned int i;
    for (i = 0; i < 3; i++)
        gpio_write_pin(gpiob, 12+i, (bank>>i)&1);
}

/* Update the currently-selected image number (1-8). */
static void image_set(unsigned int image)
{
    unsigned int bank = ksw_config.image_map[image-1];
    rom_bank_set(bank);
    printk("Image %u/%u -> ROM %u\n", image, ksw_config.nr_images, bank);
}

int main(void)
{
    unsigned int i, kickstart;

    /* Relocate DATA. Initialise BSS. */
    if (_sdat != _ldat)
        memcpy(_sdat, _ldat, _edat-_sdat);
    memset(_sbss, 0, _ebss-_sbss);

    canary_init();

    stm32_init();
    time_init();
    recall_init();
    console_init();
    speaker_init();

    /* PA8 = reset input */
    gpio_configure_pin(gpioa, 8, GPI_floating);

    /* PB[14:12] = select output */
    for (i = 0; i < 3; i++)
        gpio_configure_pin(gpiob, 12+i, GPO_pushpull(_2MHz, LOW));

    /* PC13 = LED */
    gpio_configure_pin(gpioc, 13, GPO_pushpull(_2MHz, HIGH)); /* PC13 LED */

    /* Probe delay setting. */
    if (gpio_pins_connected(gpiob, 3, gpioa, 15)) {
        /* PB3:PA15 -> shorter delay */
        switch_delay = 0;
    } else if (gpio_pins_connected(gpiob, 3, gpiob, 4)) {
        /* PB3:PB4 -> longer delay */
        switch_delay = 2;
    }

    config_init();

    timer_init(&tick_timer, tick_fn, NULL);
    timer_set(&tick_timer, time_now() + time_ms(TICK_MS));

    kickstart = recall_get();
    image_set(kickstart);

    for (;;) {
        canary_check();
        if (!nr_switches)
            continue;
        kickstart = ((kickstart + nr_switches - 1) % ksw_config.nr_images) + 1;
        nr_switches = 0;
        speaker_pulses(kickstart);
        image_set(kickstart);
        recall_set(kickstart);
    }

    return 0;
}

/*
 * Local variables:
 * mode: C
 * c-file-style: "Linux"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
