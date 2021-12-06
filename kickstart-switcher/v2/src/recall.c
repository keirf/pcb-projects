/*
 * recall.c
 * 
 * Last-image recall.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

/*
 * Time to write one word of Flash: ~55us
 * Time to erase one page of Flash: ~45ms
 */

static uint16_t * const recall_map = (uint16_t *)0x08002000;
const unsigned int recall_nr_words = (0x8007c00 - 0x8002000) / 2;

static unsigned int recall_off;
static bool_t recall_enable_invert;

static bool_t recall_enabled(void)
{
    return ksw_config.image_recall ^ recall_enable_invert;
}

void recall_erase(void)
{
    uint32_t p = (uint32_t)recall_map;
    int todo = recall_nr_words*2;
    while (todo > 0) {
        fpec_page_erase(p);
        p += FLASH_PAGE_SIZE;
        todo -= FLASH_PAGE_SIZE;
    }
}

void recall_set(unsigned int image)
{
    uint16_t x = image;

    if (!recall_enabled())
        return;

    if ((recall_off % (FLASH_PAGE_SIZE/2)) == 0)
        fpec_page_erase((uint32_t)&recall_map[recall_off]);

    recall_off = (recall_off == 0) ? recall_nr_words-1 : recall_off-1;
    fpec_write(&x, 2, (uint32_t)&recall_map[recall_off]);
}

unsigned int recall_get(void)
{
    fpec_init();

    if (recall_enabled()) {
        for (recall_off = 0; recall_off < recall_nr_words; recall_off++)
            if (recall_map[recall_off] != 0xffffu)
                return recall_map[recall_off];
    }

    recall_off = recall_nr_words - 1;
    return 1;
}

void recall_init(void)
{
    recall_enable_invert = gpio_pins_connected(gpioa, 9, gpioa, 10);
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

