#include "Protagonistas.h"

Aldeano::Aldeano() {
	this->name = "champion";
	this->state = EST_QUIETO;
	this->destino = nullptr;
	this->pos = nullptr;
	this->rapidez = 1;
}