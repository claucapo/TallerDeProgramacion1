#include "Spritesheet.h"
#include "BibliotecaDeImagenes.h"
#include "ConversorUnidades.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

Spritesheet::Spritesheet(void) {
	this->sprite = nullptr;
	this->filas = 1;
	this->columnas = 1;
	this->subX = 1;
	this->subY = 1;
	this->x_pant = 0;
	this->y_pant = 0;

	delayAnimacion = 1;
	contFrames = 0;
}

//TODO: Chequear errores como subX < rows
Spritesheet::Spritesheet(string name) {
	DatosImagen* data = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen(name);

	this->sprite = data->imagen;
	this->filas = data->filas;
	this->columnas = data->columnas;
	this->origenX = data->origenX;
	this->origenY = data->origenY;

	this->subX = 0;
	this->subY = 0;
	this->y_pant = 0;
	this->x_pant = 0;

	delayAnimacion = data->delay;
	contFrames = 0;
}

Spritesheet::~Spritesheet(void) {
}

void Spritesheet::cambiarImagen(string nuevaImagen) {
	DatosImagen* data = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen(nuevaImagen);

	this->sprite = data->imagen;
	this->filas = data->filas;
	this->columnas = data->columnas;
	this->origenX = data->origenX;
	this->origenY = data->origenY;

}

void Spritesheet::cambiarSubImagen(int subX, int subY) {
	this->subX = subX;
	this->subY = subY;
}

void Spritesheet::siguienteFrame() {
	contFrames++;
	if(contFrames >= delayAnimacion){
		contFrames = 0;
		this->subX++;
		if (this->subX >= this->columnas)
			this->subX = 0;
	}
	//cout << "Count: " << contFrames << endl;
	//cout << "SubX: " << subX << endl;
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

void Spritesheet::setAnimationDelay(float delay_ms) {
	delayAnimacion = (int) ConversorUnidades::obtenerInstancia()->convertMilisecondsToFrames(delay_ms);
	cout << "Delay: " << delayAnimacion << endl;
}
