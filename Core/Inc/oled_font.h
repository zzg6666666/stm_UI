#ifndef OLED_FONT_H
#define OLED_FONT_H

#include "stdint.h"




uint8_t get_char_vram_length(const char *string);
void set_oled_font_vram(const char *string, uint8_t *vram, uint8_t vram_length);
#endif