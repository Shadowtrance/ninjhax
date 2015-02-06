#ifndef TEXT_H
#define TEXT_H

void drawCharacter(u8* fb, char c, u16 x, u16 y);
void drawString(u8* fb, char* str, u16 x, u16 y);
void drawFormat(u8* fb, u16 x, u16 y, const char *format, ...);

#endif
