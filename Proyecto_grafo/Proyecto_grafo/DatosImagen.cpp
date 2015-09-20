#include "DatosImagen.h"
#include <SDL.h>
#include <SDL_image.h>

DatosImagen::DatosImagen(void) {
	// Datos por default...
	imagen = nullptr;
	path = "default.png";
	anchoImagen = 310;
	altoImagen = 349;
	filas = 1;
	columnas = 1;
	origenX = 0;
	origenY = 0;
	casillasX = 1;
	casillasY = 1;
	fps = 1;
	delay = 1;
}

DatosImagen::~DatosImagen(void) {
}
