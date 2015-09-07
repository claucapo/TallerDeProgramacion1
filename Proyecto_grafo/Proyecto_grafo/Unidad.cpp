#include "Unidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "ConversorUnidades.h"
#include <cmath>
#include <iostream>

Unidad::Unidad() : Entidad() {
	this->rapidez = 0;
	this->direccion = DIR_DOWN;
	this->destino = new Posicion(this->pos->getX(), this->pos->getY());
}

Unidad::Unidad(Posicion* p) : Entidad() {
	this->pos = p;
	this->rapidez = 0;
	this->direccion = DIR_DOWN;
	this->destino = new Posicion(this->pos->getX(), this->pos->getY());
}

Unidad::~Unidad() {
	delete this->destino;
}

void Unidad::setVelocidad(float nuevaVelocidad) {
	this->rapidez = nuevaVelocidad;
}

void Unidad::asignarSprite(string name) {
	this->spriteBaseName = name;
	this->sprites = new Spritesheet(name + IMG_EXT, 8, 10, 0, this->direccion);
}


// Designa un nuevo destino, cambiando la direccion del movimiento
void Unidad::nuevoDestino(float newX, float newY){
	// Asigno el nuevo destino
	if(this->destino)
		delete this->destino;
	this->destino = new Posicion(newX, newY);
	
	// TODO: Calcular la dirección de movimiento
	this->direccion = calcularDirecion(this->destino->getX() - this->pos->getX(), this->destino->getY() - this->pos->getY());

	// Asigno la direccion al sprite
	if (this->state != EST_CAMINANDO) {
		this->state = EST_CAMINANDO;
		// delete this->sprites; Debería borrar esto?
		// TODO: determinar los numeros "8" y "10" con algun metodo o establecerlos como constante
		
		this->sprites = new Spritesheet(this->spriteBaseName + estados_extensiones[EST_CAMINANDO] + IMG_EXT, 8, 10, 0, this->direccion);
	}
	this->sprites->cambiarSubImagen(1, this->direccion);
}

#define TOLERANCIA 3
void Unidad::avanzarFrame() {
	// SI esta caminando
	if (this->state == EST_CAMINANDO) {
		// La unidad se traslada
		float dirX = this->destino->getX() - this->pos->getX();
		float dirY = this->destino->getY()- this->pos->getY() ;
		// Calculo la distancia
		float distancia = sqrt(dirX*dirX + dirY*dirY);
		distancia = (ConversorUnidades::obtenerInstancia())->convertULToPixels(distancia);
		cout << "Distancia al objetivo: " << distancia << endl;
		if (distancia < TOLERANCIA) {
			// Si es menor a la tolerancia, llegue al destino
			delete this->pos;
			this->pos = this->destino;
			this->state = EST_QUIETO;
			// TODO: Idem que en el metodo nuevoDestino
			this->sprites = new Spritesheet(this->spriteBaseName + IMG_EXT, 8, 10, 0, this->direccion);
		} else {
			// Normalizo el vector direccion y calculo el desplazamiento en cada eje
			float newX = this->pos->getX() + (dirX * this->rapidez)/distancia;
			float newY = this->pos->getY() + (dirY * this->rapidez)/distancia;

			delete this->pos;
			this->pos = new Posicion(newX, newY);
		}
	}
	float newX = ConversorUnidades::obtenerInstancia()->convertULToPixels(this->pos->getX());
	float newY= ConversorUnidades::obtenerInstancia()->convertULToPixels(this->pos->getY());
	
	this->sprites->cambirCoord(newX, newY);
	this->sprites->siguienteFrame();
}


Direcciones_t Unidad::calcularDirecion(float velocidadX, float velocidadY)
{

	if(velocidadX > 0) {
		if(velocidadY == 0) return DIR_DOWN_LEFT;
		if(velocidadX == velocidadY) return DIR_DOWN;
		}
	if(velocidadX < 0) {
		if(velocidadY == 0) return DIR_TOP_RIGHT;
		if(velocidadX == velocidadY) return DIR_TOP;
		if((velocidadY > 0) && ((velocidadY+velocidadX) == 0))return DIR_RIGHT;
	}
	if(velocidadY > 0)
		if(velocidadX == 0) return DIR_DOWN_RIGHT;
	if(velocidadY < 0)
		if(velocidadX == 0) return DIR_TOP_LEFT;
	
	
	return DIR_LEFT;
}
