#include <SDL.h>
#include <SDL_image.h>

#pragma once

/* La clase DatosImagen asocia todos los
datos de una imagen con su correspondiente
SDL_Surface, dando acceso a todos estos datos.

La idea de distinguir esta clase es que cada
imagen .png tiene asociados distintos valores
completamente independientes de la forma de
representarla l�gicamente.*/
class DatosImagen
{
public:

	// DATOS ASOCIADOS A CADA IMAGEN

	// La imagen en s�
	SDL_Surface* imagen;

	// Las dimensiones en p�xeles
	int anchoImagen, altoImagen;

	// La cant. de filas y columnas
	// de la imagen (caso spritesheet)
	int filas, columnas;

	// El falso origen respecto de cada
	// subimagen, correspondiente al
	// v�rtice menor de la imagen.
	int origenX, origenY;

	// Cantidad de casillas l�gicas en 
	// X e Y que ocupa la imagen
	int casillasX, casillasY;

public:
	DatosImagen(void);
	~DatosImagen(void);

	// CONSTRUCTOR RECOMENDADO
	// Inicializa a la instancia con una
	// surface asociada y valores por
	// defecto.
	DatosImagen(SDL_Surface* img);
};

