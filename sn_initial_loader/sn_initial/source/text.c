#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctr/types.h>
#include "text.h"
#include "font_bin.h"

#define CHAR_SIZE_X (8)
#define CHAR_SIZE_Y (8)

int _strlen(char* str);

/** \brief Division result
 *  \sa uidiv
 */
typedef struct uidiv_result {
    u32 quo;  ///< Quotient
    u32 rem;  ///< Remainder
} uidiv_result_t;

/********************************************//**
 *  \brief Unsigned integer division
 *  
 *  ARM does not have native division support
 *  \returns Result of operation or zero if 
 *  dividing by zero.
 ***********************************************/
uidiv_result_t
uidiv (u32 num,   ///< Numerator
       u32 dem)   ///< Denominator
{
    u32 tmp = dem;
    uidiv_result_t ans = {0};
    
    if (dem == 0)
    {
        // TODO: Somehow make error
        return ans;
    }
    
    while (tmp <= num >> 1)
    {
        tmp <<= 1;
    }
    
    do
    {
        if (num >= tmp)
        {
            num -= tmp;
            ans.quo++;
        }
        ans.quo <<= 1;
        tmp >>= 1;
    } while (tmp >= dem);
    ans.quo >>= 1;
    ans.rem = num;
    
    return ans;
}

// thanks naehrwert for the tiny printf
static void _putn(char **p_str, u32 x, u32 base, char fill, int fcnt, int upper)
{
    char buf[65];
    char *digits;
    char *p;
    int c = fcnt;
    uidiv_result_t div_res;

    if (upper)
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    else
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    
    if(base > 36)
        return;

    p = buf + 64;
    *p = 0;
    do
    {
        c--;
        div_res = uidiv (x, base);
        *--p = digits[div_res.rem];
        x = div_res.quo;
    }while(x);
    
    if(fill != 0)
    {
        while(c > 0)
        {
            *--p = fill;
            c--;
        }
    }
    
    for(; *p != '\0'; *((*p_str)++) = *(p++));
}

/********************************************//**
 *  \brief Simple @c vsprintf
 *  
 *  Only supports %c, %s, %u, %x, %X with 
 *  optional zero padding.
 *  Always returns zero.
 ***********************************************/
int _vsprintf (char *str, const char *fmt, va_list ap)
{
    char *s;
    char c, fill;
    int fcnt;
    u32 n;
    
    while(*fmt)
    {
        if(*fmt == '%')
        {
            fmt++;
            fill = 0;
            fcnt = 0;
            if((*fmt >= '0' && *fmt <= '9') || *fmt == ' ')
                if(*(fmt+1) >= '0' && *(fmt+1) <= '9')
                {
                    fill = *fmt;
                    fcnt = *(fmt+1) - '0';
                    fmt++;
                    fmt++;
                }
            switch(*fmt)
            {
            case 'c':
                c = va_arg(ap, u32);
                *(str++) = c;
                break;
            case 's':
                s = va_arg(ap, char *);
                for(; *s != '\0'; *(str++) = *(s++));
                break;
            case 'u':
                n = va_arg(ap, u32);
                _putn(&str, n, 10, fill, fcnt, 0);
                break;
            case 'x':
                n = va_arg(ap, u32);
                _putn(&str, n, 16, fill, fcnt, 0);
                break;
            case 'X':
                n = va_arg(ap, u32);
                _putn(&str, n, 16, fill, fcnt, 1);
                break;
            case '%':
                *(str++) = '%';
                break;
            case '\0':
                goto out;
            default:
                *(str++) = '%';
                *(str++) = *fmt;
                break;
            }
        }
        else
            *(str++) = *fmt;
        fmt++;
    }

    out:
    *str = '\0';
    return 0;
}

void drawCharacter(u8* fb, char c, u16 x, u16 y)
{
	if(c<32)return;
	c-=32;
	u8* charData=(u8*)&font_bin[(CHAR_SIZE_X*CHAR_SIZE_Y*c)/8];
	fb+=(x*240+y)*3;
	int i, j;
	for(i=0;i<CHAR_SIZE_X;i++)
	{
		u8 v=*(charData++);
		for(j=0;j<CHAR_SIZE_Y;j++)
		{
			if(v&1)fb[0]=fb[1]=fb[2]=0x00;
			else fb[0]=fb[1]=fb[2]=0xFF;
			fb+=3;
			v>>=1;
		}
		fb+=(240-CHAR_SIZE_Y)*3;
	}
}

void drawString(u8* fb, char* str, u16 x, u16 y)
{
	if(!str)return;
	y=232-y;
	int k;
	int dx=0, dy=0;
	for(k=0;k<_strlen(str);k++)
	{
		if(str[k]>=32 && str[k]<128)drawCharacter(fb,str[k],x+dx,y+dy);
		dx+=8;
		if(str[k]=='\n'){dx=0;dy-=8;}
	}
}

/********************************************//**
 *  \brief Simple @c sprintf
 *  
 *  Only supports %c, %s, %u, %x, %X with 
 *  optional zero padding. 1024 max length.
 *  Always returns zero.
 ***********************************************/
void drawFormat(u8* fb, u16 x, u16 y, const char *format, ...)
{
    char buffer[1024];
    va_list arg;

    va_start (arg, format);
    _vsprintf (buffer, format, arg);
    va_end (arg);

    drawString (fb, buffer, x, y);
}
