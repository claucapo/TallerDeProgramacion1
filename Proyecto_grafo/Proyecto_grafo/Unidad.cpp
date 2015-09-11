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
	int x = (int) (ConversorUnidades::obtenerInstancia())->obtenerCoordPantallaX(this->pos->getX(), this->pos->getY()); 
	int y = (int) (ConversorUnidades::obtenerInstancia())->obtenerCoordPantallaY(this->pos->getX(), this->pos->getY());
	this->sprites->cambirCoord(x, y);
}


// Designa un nuevo destino, cambiando la direccion del movimiento
void Unidad::nuevoDestino(float newX, float newY){
	// Asigno el nuevo destino
	if(this->destino)
		delete this->destino;
	this->destino = new Posicion(newX, newY);
	
	// Calcular la dirección de movimiento
	float deltaX = newX - this->verPosicion()->getX();
	float deltaY = newY - this->verPosicion()->getY();
	
	this->direccion = calcularDirecion(deltaX, deltaY);
	// Asigno la direccion al sprite
	if (this->state != EST_CAMINANDO) {
		this->state = EST_CAMINANDO;
		// TODO: determinar los numeros "8" y "10" con algun metodo o establecerlos como constante
		this->sprites->cambiarImagen(this->spriteBaseName + estados_extensiones[EST_CAMINANDO] + IMG_EXT, 8, 10, 0, this->direccion);
		//this->sprites = new Spritesheet(this->spriteBaseName + estados_extensiones[EST_CAMINANDO] + IMG_EXT, 8, 10, 0, this->direccion);
	}
	//this->sprites->cambiarSubImagen(1, this->direccion);
}

#define TOLERANCIA 3
void Unidad::avanzarFrame() {
	/*
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
			this->sprites->cambiarImagen(this->spriteBaseName + IMG_EXT, 8, 10, 0, this->direccion);
			this->pos = new Posicion(this->destino->getX(),this->destino->getY() );
			this->state = EST_QUIETO;
			// TODO: Idem que en el metodo nuevoDestino
		} else {
			// Normalizo el vector direccion y calculo el desplazamiento en cada eje
			float newX = this->pos->getX() + (dirX * this->rapidez)/distancia;
			float newY = this->pos->getY() + (dirY * this->rapidez)/distancia;

			delete this->pos;
			this->pos = new Posicion(newX, newY);
		}
	}
	*/
	int x = (int) (ConversorUnidades::obtenerInstancia())->obtenerCoordPantallaX(this->pos->getX(), this->pos->getY()); 
	int y = (int) (ConversorUnidades::obtenerInstancia())->obtenerCoordPantallaY(this->pos->getX(), this->pos->getY());
	
	this->sprites->cambirCoord(x, y);
	this->sprites->siguienteFrame();
}


Direcciones_t Unidad::calcularDirecion(float velocidadX, float velocidadY)
{
	
	// Transformo a cartesianas comunes
	float vX = velocidadY * 0.866 - velocidadX * 0.866;
	float vY = velocidadY * (-0.5) - velocidadX * 0.5;
	
	cout << "Velocidad: " << vX << " " << vY << endl;

	if(vX > 0){
		float tang = vY/vX;
		if(tang > 2.414)
			return DIR_TOP;
		else if(tang > 0.414)
			return DIR_TOP_RIGHT;
		else if(tang < -2.414)
			return DIR_DOWN;
		else if(tang < -0.414)
			return DIR_DOWN_RIGHT;
		else
			return DIR_RIGHT;
		}
	if(vX < 0){
		float tang = - vY/vX;
		if(tang > 2.414)
			return DIR_TOP;
		else if(tang > 0.414)
			return DIR_TOP_LEFT;
		else if(tang < -2.414)
			return DIR_DOWN;
		else if(tang < -0.414)
			return DIR_DOWN_LEFT;
		else
			return DIR_LEFT;
		}
	if(vY > 0)
		return DIR_TOP;
	return DIR_DOWN;
	}
