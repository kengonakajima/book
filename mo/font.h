#ifndef _FONT_H_
#define _FONT_H_

#include "SDL.h"

class Font
{
 public:

    SDL_Surface *m_font;    // The SDL Surface for the font image
    int m_width;            // Width of the SDL Surface (same as the height)
    int m_charWidth;        // Width of one block character in the font (fontWidth/16)
    int *m_widths;          // Real widths of all the fonts
    unsigned char *m_data;  // The raw font data
  
    Font( const char *fontdir, float r, float g, float b, float a );
    ~Font();
    void drawString( SDL_Surface *scr, int x, int y, const char *string );
    int stringWidth( const char *string );
};

#endif

