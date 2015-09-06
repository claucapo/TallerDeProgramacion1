#include "Spritesheet.h"
#include "BibliotecaDeImagenes.h"
#include <SDL.h>
#include <SDL_image.h>

Spritesheet::Spritesheet(void)
{
	this->rows = 1;
	columnas = 1;
	offsetX = 0;
	offsetY = 0;
	x_pant = 0;
	y_pant = 0;
}

Spritesheet::Spritesheet(string imagen, int rows, int cols, int x_offset, int y_offset)
{
	this->rows = rows;
	columnas = cols;
	offsetX = x_offset;
	offsetY = y_offset;
	x_pant = 0;
	y_pant = 0;
	sprite = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(imagen);
}


Spritesheet::~Spritesheet(void)
{
}


int Spritesheet::verCoordXPantalla(void)
{
	return x_pant;
}


int Spritesheet::verCoordYPantalla(void)
{
	return y_pant;
}


void Spritesheet::cambirCoordPantalla(int coordXPant, int coordYPant)
{
	x_pant = coordXPant;
	y_pant = coordYPant;
}


void Spritesheet::cambiarImagen(string nuevaImagen, int rows, int cols, int x_offset, int y_offset)
{
	this->rows = rows;
	columnas = cols;
	offsetX = x_offset;
	offsetY = y_offset;
	sprite = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(nuevaImagen);

}


int Spritesheet::verOffsetX(void)
{
	return offsetX;
}


int Spritesheet::verOffsetY(void)
{
	return offsetY;
}


SDL_Surface* Spritesheet::devolverImagenAsociada(void)
{
	return sprite;
}


int Spritesheet::getRows(void)
{
	return rows;
}


int Spritesheet::getCols(void)
{
	return columnas;
}
