#ifndef TEXT_H
#define TEXT_H

int _sprintf (char *str, const char *format, ...);
void drawCharacter(u8* fb, char c, u16 x, u16 y);
void drawString(u8* fb, char* str, u16 x, u16 y);

#endif
