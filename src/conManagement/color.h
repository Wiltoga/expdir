#ifndef DEF_CONSOLE_COLOR_H
#define DEF_CONSOLE_COLOR_H
#include <stdint.h>
#include <stdbool.h>

#define SYSTEM_COLOR_BLACK 30
#define SYSTEM_COLOR_RED 31
#define SYSTEM_COLOR_GREEN 32
#define SYSTEM_COLOR_YELLOW 33
#define SYSTEM_COLOR_BLUE 34
#define SYSTEM_COLOR_MAGENTA 35
#define SYSTEM_COLOR_CYAN 36
#define SYSTEM_COLOR_GREY 90
#define SYSTEM_COLOR_GRAY CONSOLE_COLOR_GREY
#define SYSTEM_COLOR_BRIGHT_GREY 37
#define SYSTEM_COLOR_BRIGHT_GRAY CONSOLE_COLOR_BRIGHT_GREY
#define SYSTEM_COLOR_BRIGHT_RED 91
#define SYSTEM_COLOR_BRIGHT_GREEN 92
#define SYSTEM_COLOR_BRIGHT_YELLOW 93
#define SYSTEM_COLOR_BRIGHT_BLUE 94
#define SYSTEM_COLOR_BRIGHT_MAGENTA 95
#define SYSTEM_COLOR_BRIGHT_CYAN 96
#define SYSTEM_COLOR_WHITE 97

#define CONSOLE_FLAG_BOLD 0b1 << 0
#define CONSOLE_FLAG_UNDERLINE 0b1 << 1
#define CONSOLE_FLAG_BLINK 0b1 << 2
#define CONSOLE_FLAG_REVERSE_COLOR 0b1 << 3

typedef uint32_t color_t;
/**
 * @brief  Creates a color from the given rgb channels
 * @param red red channel value
 * @param green green channel value
 * @param blue blue channel value
 * @return color int data 
 */
color_t color_create(uint8_t red, uint8_t green, uint8_t blue);
/**
 * @brief  Changes the red channel of the color. The color variable has to be a custom color.
 * @param  c color to change
 * @param  red new red value
 */
void color_setRed(color_t *c, uint8_t red);
/**
 * @brief  Changes the green channel of the color. The color variable has to be a custom color.
 * @param  c color to change
 * @param  green new green value
 */
void color_setGreen(color_t *c, uint8_t green);
/**
 * @brief  Changes the blue channel of the color. The color variable has to be a custom color.
 * @param  c color to change
 * @param  blue new blue value
 */
void color_setBlue(color_t *c, uint8_t blue);
/**
 * @brief  Returns the red channel of the color. The color varaible has to be a custom color.
 * @param  c color from which to extract the red channel
 * @return value of the red channel
 */
uint8_t color_getRed(color_t c);
/**
 * @brief  Returns the green channel of the color. The color varaible has to be a custom color.
 * @param  c color from which to extract the green channel
 * @return value of the green channel
 */
uint8_t color_getGreen(color_t c);
/**
 * @brief  Returns the blue channel of the color. The color varaible has to be a custom color.
 * @param  c color from which to extract the blue channel
 * @return value of the blue channel
 */
uint8_t color_getBlue(color_t c);
/**
 * @brief  Returns true if the color is a custom color, or false if it is a system color.
 * @param  c color to test
 */
bool isColorCustom(color_t c);
#endif