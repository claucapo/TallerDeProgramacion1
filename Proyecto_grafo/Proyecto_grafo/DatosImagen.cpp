#include "DatosImagen.h"
#include <SDL.h>
#include <SDL_image.h>

DatosImagen::DatosImagen(void)
{
}

DatosImagen::DatosImagen(SDL_Surface* img){
	imagen = img;
	anchoImagen = 2000;
	altoImagen = 2000;
	filas = 1;
	columnas = 1;
	origenX = 0;
	origenY = 0;
	casillasX = 1;
	casillasY = 1;
}

DatosImagen::~DatosImagen(void)
{
}
