#include "Protagonistas.h"

Aldeano::Aldeano() {
	this->name = "aldeano";
	this->state = EST_QUIETO;
	this->destino = nullptr;
	this->pos = nullptr;
	this->rapidez = 1;
}