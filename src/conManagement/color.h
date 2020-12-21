#ifndef DEF_CONSOLE_COLOR_H
#define DEF_CONSOLE_COLOR_H
#include <stdint.h>
//https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
#define CONSOLE_COLOR_BLACK 30
#define CONSOLE_COLOR_RED 31
#define CONSOLE_COLOR_GREEN 32
#define CONSOLE_COLOR_YELLOW 33
#define CONSOLE_COLOR_BLUE 34
#define CONSOLE_COLOR_MAGENTA 35
#define CONSOLE_COLOR_CYAN 36
#define CONSOLE_COLOR_GREY 90
#define CONSOLE_COLOR_GRAY CONSOLE_COLOR_GREY
#define CONSOLE_COLOR_BRIGHT_GREY 37
#define CONSOLE_COLOR_BRIGHT_GRAY CONSOLE_COLOR_BRIGHT_GREY
#define CONSOLE_COLOR_BRIGHT_RED 91
#define CONSOLE_COLOR_BRIGHT_GREEN 92
#define CONSOLE_COLOR_BRIGHT_YELLOW 93
#define CONSOLE_COLOR_BRIGHT_BLUE 94
#define CONSOLE_COLOR_BRIGHT_MAGENTA 95
#define CONSOLE_COLOR_BRIGHT_CYAN 96
#define CONSOLE_COLOR_WHITE 97

#define CONSOLE_FLAG_BOLD 0b1 << 0
#define CONSOLE_FLAG_UNDERLINE 0b1 << 1
#define CONSOLE_FLAG_BLINK 0b1 << 2
#define CONSOLE_FLAG_REVERSE_COLOR 0b1 << 3
/**
 * @brief  color structure
 */
typedef struct
{
    /**
     * @brief  red channel of the color
     */
    uint8_t r;
    /**
     * @brief  green channel of the color
     */
    uint8_t g;
    /**
     * @brief  blue channel of the color
     */
    uint8_t b;
} color;
/**
 * @brief  Creates a color from the given rgb channels
 * @param red red channel value
 * @param green green channel value
 * @param blue blue channel value
 * @return color structure 
 */
color console_color(uint8_t red, uint8_t green, uint8_t blue);

#endif