#include <SDL.h>
#include <SDL_image.h>
#include <string>

#pragma once

/* La clase DatosImagen asocia todos los
datos de una imagen con su correspondiente
SDL_Surface, dando acceso a todos estos datos.

La idea de distinguir esta clase es que cada
imagen .png tiene asociados distintos valores
completamente independientes de la forma de
representarla lógicamente.*/
class DatosImagen {
public:	
	// La imagen en sí
	SDL_Surface* imagen;

	// DATOS ASOCIADOS A CADA IMAGEN
	std::string path;

	// Las dimensiones en píxeles
	int anchoImagen, altoImagen;

	// La cant. de filas y columnas
	// de la imagen (caso spritesheet)
	int filas, columnas;

	// El falso origen respecto de cada
	// subimagen, correspondiente al
	// vértice menor de la imagen.
	int origenX, origenY;

	// Cantidad de casillas lógicas en 
	// X e Y que ocupa la imagen
	int casillasX, casillasY;

	// FPS y delay (este último en ms)
	int fps, delay;

public:
	DatosImagen(void);
	~DatosImagen(void);
};

