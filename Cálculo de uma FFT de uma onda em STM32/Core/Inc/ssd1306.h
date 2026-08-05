#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_WriteString(uint8_t page, uint8_t col, const char *str);

#endif
