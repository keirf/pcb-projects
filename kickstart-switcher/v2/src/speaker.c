/*
 * speaker.c
 *
 * PC speaker/buzzer control.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

/* Timer 2, channel 3, PB10 */
#define gpio_spk gpiob
#define PIN_SPK 10
#define tim tim2
#define PWM_CCR ccr3

/* Period = 500us -> Frequency = 2000Hz */
#define ARR 499

/* Three volume settings, default is medium. PB5 selects others. */
static unsigned int volume = 1;

static struct timer spk_timer;
static unsigned int nr_pulses;

static void spk_timer_fn(void *data)
{
    unsigned int ccr, next;

    if (nr_pulses & 1) {
        ccr = 0;
        next = time_ms(120);
    } else {
        ccr = ksw_config.volumes[volume];
        next = time_ms(30);
        if (ksw_config.long_beep && (nr_pulses >= 8)) {
            nr_pulses -= 6;
            next += time_ms(ksw_config.long_beep);
        }
    }
    gpio_write_pin(gpioc, 13, !ccr); /* PC13 LED */
    tim->PWM_CCR = ccr;
    if (--nr_pulses)
        timer_set(&spk_timer, spk_timer.deadline + next);
}

void speaker_init(void)
{
    /* Probe volume setting. */
    if (gpio_pins_connected(gpiob, 6, gpiob, 5)) {
        /* PB6 jumpered to PB5 -> low volume */
        volume = 0;
    } else if (gpio_pins_connected(gpiob, 6, gpiob, 7)) {
        /* PB6 jumpered to PB7 -> high volume */
        volume = 2;
    }
   
    /* Set up timer, switch speaker off. PWM output is active high. */
    rcc->apb1enr |= RCC_APB1ENR_TIM2EN;
    tim->arr = ARR;
    tim->psc = SYSCLK_MHZ - 1; /* tick at 1MHz */
    tim->ccmr2 = (TIM_CCMR2_CC3S(TIM_CCS_OUTPUT) |
                  TIM_CCMR2_OC3M(TIM_OCM_PWM1)); /* PWM1: high then low */
    tim->PWM_CCR = tim->cr2 = tim->dier = 0;
    tim->ccer = TIM_CCER_CC3E;
    tim->cr1 = TIM_CR1_CEN;

    /* Set up the output pin. */
    afio->mapr |= AFIO_MAPR_TIM2_REMAP_PARTIAL_2;
    gpio_configure_pin(gpio_spk, PIN_SPK, AFO_pushpull(_2MHz));

    timer_init(&spk_timer, spk_timer_fn, NULL);
}

/* Volume: 0 (silence) - 20 (loudest) */
void speaker_pulses(uint8_t nr)
{
    /* Quadratic scaling of pulse width seems to give linear-ish volume. */
    timer_cancel(&spk_timer);
    nr_pulses = nr*2;
    timer_set(&spk_timer, time_now());
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
