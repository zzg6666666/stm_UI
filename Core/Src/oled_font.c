#include "oled_font.h"
#include "string.h"
// #include "ascii_font.h"

extern uint8_t ascii_font[128][16];

// 获取字符的显存宽度
uint8_t get_char_vram_length(const char *string)
{
    uint8_t string_length = strlen(string);
    uint8_t char_vram_length = 0;

    for (uint8_t i = 0; i < string_length; i++)
    {

        // ascll 字符码小于128
        if (*string < 128)
        {
            char_vram_length += 1;
        }
        // 非ascll字符码大于128
        else
        {
            char_vram_length += 2;
            i++;
            string++;
        }
        string++;
    }
    return char_vram_length;
}

// 使用 宽 8 长 16
void set_oled_font_vram(const char *string, uint8_t *vram, uint8_t vram_length)
{

    uint16_t vram_index = 0;
    while (vram_length)
    {
        if (*string < 128)
        {
            for (uint8_t i = 0; i < 8; i++)
            {
                vram[vram_index + 2 * i] = ascii_font[(uint8_t)*string][i];
                vram[vram_index + 2 * i + 1] = ascii_font[(uint8_t)*string][i + 8];
            }
        }
        else
        {
            // 复制非ascll
            string++;
        }
        string++;
        vram_index += 16;
        vram_length--;
    }
}
