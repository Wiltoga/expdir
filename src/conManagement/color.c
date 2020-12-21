#include "color.h"

color console_color(uint8_t red, uint8_t green, uint8_t blue)
{
    color c;
    c.r = red;
    c.g = green;
    c.b = blue;
    return c;
}
