/*
 * config.c
 * 
 * Read/write/modify configuration parameters.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

const static struct ksw_config *flash_config = (struct ksw_config *)0x08008000;
const static struct ksw_config dfl_config = {
    .reset_delays = { 2*20, 3*20, 4*20 }, /* 2s, 3s, 4s */
    .volumes = { 10, 30, 80 },
    .nr_images = 2,
    .image_map = { 0, 1, 2, 3, 4, 5, 6, 7 },
    .menu_rom_bank = 0
};
struct ksw_config ksw_config;

static void config_printk(const struct ksw_config *conf)
{
    unsigned int i;
    printk("\nCurrent config:\n");
    printk(" Nr images: %u\n", conf->nr_images);
    printk(" Image map: [");
    for (i = 0; i < conf->nr_images-1; i++)
        printk("%u,", conf->image_map[i]);
    printk("%u]\n", conf->image_map[i]);
    printk(" Reset delays (/0.05s): [%u,%u,%u]\n",
           conf->reset_delays[0],
           conf->reset_delays[1],
           conf->reset_delays[2]);
    printk(" Speaker volumes: [%u,%u,%u]\n",
           conf->volumes[0],
           conf->volumes[1],
           conf->volumes[2]);
    printk(" Menu ROM bank: %u\n\n", conf->menu_rom_bank);
}

static void config_write_flash(struct ksw_config *conf)
{
    conf->crc16_ccitt = htobe16(
        crc16_ccitt(conf, sizeof(*conf)-2, 0xffff));
    fpec_init();
    fpec_page_erase((uint32_t)flash_config);
    fpec_write(conf, sizeof(*conf), (uint32_t)flash_config);
}

static unsigned int console_get_uint(void)
{
    uint8_t c, p[8];
    unsigned int i = 0, j, r;

    c = usart1->dr; /* clear UART_SR_RXNE */    
    for (;;) {
        while (!(usart1->sr & USART_SR_RXNE))
            continue;
        c = usart1->dr;
        if (c == '\r') /* CR */ {
            if (i == 0)
                continue;
            break;
        }
        if (((c == '\b') || (c == 0x7f)) && (i > 0)) { /* BS/DEL */
            printk("\b \b");
            i--;
        }
        if ((c >= '0') && (c <= '9') && (i < 10)) { /* NUM */
            printk("%c", c);
            p[i++] = c;
        }
    }

    printk("\n");

    r = 0;
    for (j = 0; j < i; j++) {
        r *= 10;
        r += p[j] - '0';
    }
    return r;
}

void config_init(void)
{
    uint16_t crc;
    unsigned int x;
    int i;

    printk("\n** Kickstart Switcher v2.2 **\n");
    printk("** Keir Fraser <keir.xen@gmail.com>\n");
    printk("** https://github.com/keirf/PCB-Projects\n");

    ksw_config = *flash_config;
    crc = crc16_ccitt(&ksw_config, sizeof(ksw_config), 0xffff);
    if (crc)
        ksw_config = dfl_config;

    config_printk(&ksw_config);

    if (crc) {
        printk("** No saved config");
    } else if (gpio_pins_connected(gpiob, 4, gpiob, 5)) {
        printk("** PB4-PB5 jumpered");
    } else {
        printk("** Jumper PB4-PB5 and reset for setup menu\n\n");
        return;
    }

    printk(" - entering setup menu...\n\n");

    rom_bank_set(ksw_config.menu_rom_bank);

    for (;;) {
        printk("Menu:\n");
        printk(" 1: Nr images\n");
        printk(" 2: Image map\n");
        printk(" 3: Reset delays\n");
        printk(" 4: Speaker volumes\n");
        printk(" 5: Menu ROM bank\n");
        printk(" --\n");
        printk(" 7: Load defaults\n");
        printk(" 8: Discard & reset\n");
        printk(" 9: Save & reset\n");
        printk(" --\n");
        printk(" 99: STM32 System Bootloader\n");

        printk("Select an option: ");
        switch (console_get_uint()) {
        case 1:
            do {
                printk("Nr images (2-8): ");
                x = console_get_uint();
            } while ((x < 2) || (x > 8));
            ksw_config.nr_images = x;
            for (i = 0; i < 8; i++)
                ksw_config.image_map[i] = i;
            break;
        case 2:
            for (i = 0; i < ksw_config.nr_images; i++) {
                do {
                    printk("Image %u -> ROM bank (0-7): ", i+1);
                    x = console_get_uint();
                } while (x > 7);
                ksw_config.image_map[i] = x;
            }
            break;
        case 3:
            for (i = 0; i < 3; i++) {
                do {
                    const static char *name[] = { "Short", "Default", "Long" };
                    printk("%s delay (0-255): ", name[i]);
                    x = console_get_uint();
                } while (x > 255);
                ksw_config.reset_delays[i] = x;
            }
            break;
        case 4:
            for (i = 0; i < 3; i++) {
                do {
                    const static char *name[] = { "Quiet", "Default", "Loud" };
                    printk("%s volume (0-255): ", name[i]);
                    x = console_get_uint();
                } while (x > 255);
                ksw_config.volumes[i] = x;
            }
            break;
        case 5:
            do {
                printk("Menu ROM bank (0-7): ");
                x = console_get_uint();
            } while (x > 7);
            ksw_config.menu_rom_bank = x;
            rom_bank_set(ksw_config.menu_rom_bank);
            break;
        case 7:
            ksw_config = dfl_config;
            break;
        case 8:
            system_reset();
            break;
        case 9:
            config_write_flash(&ksw_config);
            system_reset();
            break;
        case 99:
            stm32_bootloader_enter();
            break;
        }
        
        config_printk(&ksw_config);
    }    
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

