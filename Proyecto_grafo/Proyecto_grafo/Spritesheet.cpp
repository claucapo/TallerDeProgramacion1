#include "Spritesheet.h"
#include "BibliotecaDeImagenes.h"
#include <SDL.h>
#include <SDL_image.h>

Spritesheet::Spritesheet(void) {
	this->sprite = nullptr;
	this->filas = 1;
	this->columnas = 1;
	this->subX = 1;
	this->subY = 1;
	this->x_pant = 0;
	this->y_pant = 0;
}

//TODO: Chequear errores como subX < rows
Spritesheet::Spritesheet(string imagen, int fil, int cols, int subX, int subY) {
	this->sprite = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(imagen);
	this->filas = fil;
	this->columnas = cols;
	this->subX = subX;
	this->subY = subY;
	this->y_pant = 0;
	this->x_pant = 0;
}

Spritesheet::~Spritesheet(void) {
}

void Spritesheet::cambiarImagen(string nuevaImagen, int fil, int cols, int subX, int subY) {
	this->sprite = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(nuevaImagen);
	this->filas = fil;
	this->columnas = cols;
	this->subX = subX;
	this->subY = subY;
}

void Spritesheet::cambiarSubImagen(int subX, int subY) {
	this->subX = subX;
	this->subY = subY;
}

void Spritesheet::siguienteFrame() {
	this->subX++;
	if (this->subX >= this->columnas)
		this->subX = 0;
}

int Spritesheet::subImagenWidth() {
	return this->sprite->w / this->columnas;
}
int Spritesheet::subImagenHeight() {
	return this->sprite->h / this->filas;
}

int Spritesheet::calcularOffsetX(void) {
	return (this->subX)  * this->subImagenWidth();
}
int Spritesheet::calcularOffsetY(void) {
	return (this->subY) * this->subImagenHeight();
}