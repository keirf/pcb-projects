/*
 * stm32f10x.c
 * 
 * Core and peripheral registers.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

void EXC_unexpected(void)
{
    struct exception_frame *frame =
        (struct exception_frame *)read_special(psp);
    uint8_t exc = (uint8_t)read_special(psr);
    printk("Unexpected %s #%u at PC=%08x\n",
           (exc < 16) ? "Exception" : "IRQ",
           (exc < 16) ? exc : exc - 16,
           frame->pc);
    system_reset();
}

static void bootloader_maybe_enter(void)
{
    static void (* const system_bootloader)(void) = (void *)0x1ffff001;
    bool_t bootloader_enter;

    /* Power up the backup-register interface and allow writes. */
    rcc->apb1enr |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
    pwr->cr |= PWR_CR_DBP;

    /* Has bootloader been requested via magic numbers in the backup regs? */
    bootloader_enter = ((bkp->dr1[0] == 0xdead) && (bkp->dr1[1] == 0xbeef));

    /* Clean up backup registers and peripheral clocks. */
    bkp->dr1[0] = bkp->dr1[1] = 0;
    rcc->apb1enr = 0;

    /* Jump to the bootloader if requested. */
    if (bootloader_enter)
        (*system_bootloader)();
}

static void exception_init(void)
{
    uint32_t sp;

    /* Switch to Process SP, and set up Main SP for IRQ/Exception context. */
    sp = read_special(msp);
    write_special(psp, sp);
    write_special(control, 2); /* CONTROL.SPSEL=1 */
    write_special(msp, _irq_stacktop);

    /* Initialise interrupts and exceptions. */
    scb->vtor = (uint32_t)(unsigned long)vector_table;
    scb->ccr |= SCB_CCR_STKALIGN | SCB_CCR_DIV_0_TRP | SCB_CCR_UNALIGN_TRP;
    scb->shcsr |= (SCB_SHCSR_USGFAULTENA |
                   SCB_SHCSR_BUSFAULTENA |
                   SCB_SHCSR_MEMFAULTENA);

    /* SVCall/PendSV exceptions have lowest priority. */
    scb->shpr2 = 0xff<<24;
    scb->shpr3 = 0xff<<16;
}

static void clock_init(void)
{
    /* Flash controller: reads require 2 wait states at 72MHz. */
    flash->acr = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY(2);

    /* Start up the external oscillator. */
    rcc->cr |= RCC_CR_HSEON;
    while (!(rcc->cr & RCC_CR_HSERDY))
        cpu_relax();

    /* PLLs, scalers, muxes. */
    rcc->cfgr = (RCC_CFGR_PLLMUL(9) |        /* PLL = 9*8MHz = 72MHz */
                 RCC_CFGR_PLLSRC_PREDIV1 |
                 RCC_CFGR_ADCPRE_DIV8 |
                 RCC_CFGR_PPRE1_DIV2);

    /* Enable and stabilise the PLL. */
    rcc->cr |= RCC_CR_PLLON;
    while (!(rcc->cr & RCC_CR_PLLRDY))
        cpu_relax();

    /* Switch to the externally-driven PLL for system clock. */
    rcc->cfgr |= RCC_CFGR_SW_PLL;
    while ((rcc->cfgr & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL)
        cpu_relax();

    /* Internal oscillator no longer needed. */
    rcc->cr &= ~RCC_CR_HSION;

    /* Enable SysTick counter at 72/8=9MHz. */
    stk->load = STK_MASK;
    stk->ctrl = STK_CTRL_ENABLE;
}

static void gpio_init(GPIO gpio)
{
    /* All pins are in weak Pull-Down mode. */
    gpio->crl = gpio->crh = 0x88888888u;
    gpio->odr = 0;
}

static void peripheral_init(void)
{
    /* Enable basic GPIO and AFIO clocks, and DMA. */
    rcc->apb1enr = 0;
    rcc->apb2enr = (RCC_APB2ENR_IOPAEN |
                    RCC_APB2ENR_IOPBEN |
                    RCC_APB2ENR_IOPCEN |
                    RCC_APB2ENR_AFIOEN);
    rcc->ahbenr = RCC_AHBENR_DMA1EN;

    /* Turn off serial-wire JTAG and reclaim the GPIOs. */
    afio->mapr = AFIO_MAPR_SWJ_CFG_DISABLED;

    /* All pins in a stable state. */
    gpio_init(gpioa);
    gpio_init(gpiob);
    gpio_init(gpioc);
}

void stm32_init(void)
{
    bootloader_maybe_enter();
    exception_init();
    clock_init();
    peripheral_init();
}

void stm32_bootloader_enter(void)
{
    /* Power up the backup-register interface and allow writes. */
    rcc->apb1enr |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
    pwr->cr |= PWR_CR_DBP;

    /* Indicate to early startup that we want system bootloader. */
    bkp->dr1[0] = 0xdead;
    bkp->dr1[1] = 0xbeef;

    /* Reset everything (except backup registers). */
    system_reset();
}

static void fpec_wait_and_clear(void)
{
    while (flash->sr & FLASH_SR_BSY)
        continue;
    flash->sr = FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR;
    flash->cr = 0;
}

void fpec_init(void)
{
    /* Erases and writes require the HSI oscillator. */
    rcc->cr |= RCC_CR_HSION;
    while (!(rcc->cr & RCC_CR_HSIRDY))
        cpu_relax();

    /* Unlock the FPEC. */
    if (flash->cr & FLASH_CR_LOCK) {
        flash->keyr = 0x45670123;
        flash->keyr = 0xcdef89ab;
    }

    fpec_wait_and_clear();
}

void fpec_page_erase(uint32_t flash_address)
{
    fpec_wait_and_clear();
    flash->cr |= FLASH_CR_PER;
    flash->ar = flash_address;
    flash->cr |= FLASH_CR_STRT;
    fpec_wait_and_clear();
}

void fpec_write(const void *data, unsigned int size, uint32_t flash_address)
{
    uint16_t *_f = (uint16_t *)flash_address;
    const uint16_t *_d = data;

    fpec_wait_and_clear();
    for (; size != 0; size -= 2) {
        flash->cr |= FLASH_CR_PG;
        *_f++ = *_d++; 
        fpec_wait_and_clear();
   }
}

void delay_ticks(unsigned int ticks)
{
    unsigned int diff, cur, prev = stk->val;

    for (;;) {
        cur = stk->val;
        diff = (prev - cur) & STK_MASK;
        if (ticks <= diff)
            break;
        ticks -= diff;
        prev = cur;
    }
}

void delay_ns(unsigned int ns)
{
    delay_ticks((ns * STK_MHZ) / 1000u);
}

void delay_us(unsigned int us)
{
    delay_ticks(us * STK_MHZ);
}

void delay_ms(unsigned int ms)
{
    delay_ticks(ms * 1000u * STK_MHZ);
}

void gpio_configure_pin(GPIO gpio, unsigned int pin, unsigned int mode)
{
    gpio_write_pin(gpio, pin, mode >> 4);
    mode &= 0xfu;
    if (pin >= 8) {
        pin -= 8;
        gpio->crh = (gpio->crh & ~(0xfu<<(pin<<2))) | (mode<<(pin<<2));
    } else {
        gpio->crl = (gpio->crl & ~(0xfu<<(pin<<2))) | (mode<<(pin<<2));
    }
}

bool_t gpio_pins_connected(GPIO gpio1, unsigned int pin1,
                           GPIO gpio2, unsigned int pin2)
{
    bool_t connected;
    /* Test if pin2 can pull pin1 high... */
    gpio_configure_pin(gpio1, pin1, GPI_pull_down);
    gpio_configure_pin(gpio2, pin2, GPO_pushpull(_2MHz, HIGH));
    delay_ms(1);
    /* ...if so, the pins are indeed connected. */
    connected = gpio_read_pin(gpio1, pin1);
    /* Return pins to their default configuration. */
    gpio_configure_pin(gpio2, pin2, GPI_pull_down);
    return connected;
}

void system_reset(void)
{
    console_sync();
    printk("Resetting...\n");
    /* Wait for serial console TX to idle. */
    while (!(usart1->sr & USART_SR_TXE) || !(usart1->sr & USART_SR_TC))
        cpu_relax();
    /* Request reset and loop waiting for it to happen. */
    scb->aircr = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
    for (;;) ;
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
