
#include "sprite.h"

Image::Image( const char *filename )
{
	SDL_Surface *temp;

	/* Load the sprite image */
	m_sfc = SDL_LoadBMP(filename);
        
	if ( m_sfc == NULL ) {
        std::cerr << "Couldn't load " << filename << SDL_GetError() << std::endl;
        throw;
	}

	/* Set transparent pixel as the pixel at (0,0) */
	if ( m_sfc->format->palette ) {
		SDL_SetColorKey( m_sfc, (SDL_SRCCOLORKEY|SDL_RLEACCEL), *(Uint8 *)m_sfc->pixels);
	}

	/* Convert sprite to video format */
	temp = SDL_DisplayFormat(m_sfc);
	SDL_FreeSurface(m_sfc);
	if ( temp == NULL ) {
        std::cerr << "Couldn't convert background: " << SDL_GetError() << std::endl;
        throw;
	}
	m_sfc = temp;

}
