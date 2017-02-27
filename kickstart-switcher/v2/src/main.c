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
 * A15,B3-4: Delay select (B3:A15->2sec, B3:B4->4sec, neither->3sec)
 * B5-7:     Volume select (B6:B5->quiet, B6:B7->loud, neither->medium)
 * B8:       Speaker output (active high)
 * B12-14:   ROM select
 * C13:      Indicator LED (active low)
 */

int EXC_reset(void) __attribute__((alias("main")));

static void canary_init(void)
{
    _irq_stackbottom[0] = _thread_stackbottom[0] = 0xdeadbeef;
}

static void canary_check(void)
{
    ASSERT(_irq_stackbottom[0] == 0xdeadbeef);
    ASSERT(_thread_stackbottom[0] == 0xdeadbeef);
}

#define TICK_PERIOD stk_ms(50)
static struct timer tick_timer;
static volatile unsigned int ticks, switch_ks;

const static unsigned int switch_secs[] = { 2, 3, 4 };
static unsigned int switch_delay = 1; /* medium delay */
#ifndef NR_KICK_IMAGES
#define NR_KICK_IMAGES 2
#endif

static void tick_fn(void *data)
{
    static unsigned int reset_ticks = 0;
    static int prev_reset = 0;
    int curr_reset = !gpio_read_pin(gpioa, 8);
    if (prev_reset == curr_reset) { /* debounce */
        if (curr_reset) {
            if (++reset_ticks == (switch_secs[switch_delay] * 20)) {
                switch_ks++;
                reset_ticks = 0;
            }
        } else {
            reset_ticks = 0;
        }
    }
    prev_reset = curr_reset;

    ticks++;
    timer_set(&tick_timer, stk_diff(tick_timer.deadline, TICK_PERIOD));
}

int main(void)
{
    unsigned int i, kickstart = 0;

    /* Relocate DATA. Initialise BSS. */
    if (_sdat != _ldat)
        memcpy(_sdat, _ldat, _edat-_sdat);
    memset(_sbss, 0, _ebss-_sbss);

    canary_init();

    stm32_init();
    timers_init();
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
    gpio_configure_pin(gpiob, 3, GPI_pull_up);
    gpio_configure_pin(gpioa, 15, GPO_pushpull(_2MHz, LOW));
    gpio_configure_pin(gpiob, 4, GPO_pushpull(_2MHz, HIGH));
    delay_ms(1);
    if (!gpio_read_pin(gpiob, 3)) {
        /* PB3 jumpered to PA15 -> shorter delay */
        switch_delay = 0;
    } else {
        gpio_configure_pin(gpiob, 4, GPO_pushpull(_2MHz, LOW));
        delay_ms(1);
        if (!gpio_read_pin(gpiob, 3)) {
            /* PB3 jumpered to PB4 -> longer delay */
            switch_delay = 2;
        }
    }

    timer_init(&tick_timer, tick_fn, NULL);
    timer_set(&tick_timer, stk_deadline(TICK_PERIOD));

    for (;;) {
        canary_check();
        if (!switch_ks)
            continue;
        kickstart += switch_ks;
        switch_ks = 0;
        kickstart %= NR_KICK_IMAGES;
        speaker_pulses(kickstart + 1);
        for (i = 0; i < 3; i++)
            gpio_write_pin(gpiob, 12+i, (kickstart>>i)&1);
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
