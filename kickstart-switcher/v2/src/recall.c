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

#define FLASH_PAGE_SIZE 1024

static uint16_t * const recall_map = (uint16_t *)0x08008000;
const unsigned int recall_nr_words = 32768/2;

static unsigned int recall_off;
static bool_t recall_enable_invert;

static bool_t recall_enabled(void)
{
    return ksw_config.image_recall ^ recall_enable_invert;
}

static unsigned int find_first_clear(uint16_t *map, unsigned int nr_words)
{
    uint16_t *l = map;
    uint16_t *h = map + nr_words - 1;
    uint16_t *m;

    do {
        m = l + (h-l)/2;
        if (*m == 0) l = m = m+1; else h = m-1;
    } while (l <= h);

    return m - map;
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
    recall_off = 0;
}

static bool_t recall_increment(void)
{
    uint16_t x = 0;
    if (recall_off >= recall_nr_words)
        return FALSE; /* map full */
    fpec_write(&x, 2, (uint32_t)&recall_map[recall_off]);
    recall_off++;
    return TRUE;
}

void recall_set(unsigned int image)
{
    unsigned int old = recall_off % ksw_config.nr_images;
    unsigned int new = image - 1;

    if (!recall_enabled())
        return;

    if (old > new)
        new += ksw_config.nr_images;

    while (old++ != new) {
        if (!recall_increment()) {
            /* map full: erase and re-start */
            recall_erase();
            old = 0;
            new = image - 1;
        }
    }
}

unsigned int recall_get(void)
{
    unsigned int image = 0;

    fpec_init();

    recall_off = find_first_clear(recall_map, recall_nr_words);

    if (recall_enabled())
        image = recall_off % ksw_config.nr_images;
    else if (recall_off != 0)
        recall_erase();

    return image + 1;
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

