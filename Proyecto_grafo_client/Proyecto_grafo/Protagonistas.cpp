#include "Protagonistas.h"

Aldeano::Aldeano() {
	this->name = "settler";
	this->state = EST_QUIETO;
	this->destino = nullptr;
	this->pos = nullptr;
	this->rapidez = 2;
}

Campeon::Campeon() {
	this->name = "champion";
	this->state = EST_QUIETO;
	this->destino = nullptr;
	this->pos = nullptr;
	this->rapidez = 2;
}

Jinete::Jinete() {
	this->name = "knight";
	this->state = EST_QUIETO;
	this->destino = nullptr;
	this->pos = nullptr;
	this->rapidez = 2;
}