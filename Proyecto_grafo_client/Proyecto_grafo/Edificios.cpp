#include "Edificios.h"

Casa::Casa() {
	this->tamX = 3;
	this->tamY = 3;
	this->name = "casa";
	this->state = EST_QUIETO;
}

CentroUrbano::CentroUrbano() {
	this->tamX = 10;
	this->tamY = 10;
	this->name = "centro_urbano";
	this->state = EST_QUIETO;
}

Castillo::Castillo() {
	this->tamX = 5;
	this->tamY = 5;
	this->name = "castillo";
	this->state = EST_QUIETO;
}

Molino::Molino() {
	this->tamX = 3;
	this->tamY = 3;
	this->name = "molino";
	this->state = EST_QUIETO;
}

Cuarteles::Cuarteles() {
	this->tamX = 4;
	this->tamY = 4;
	this->name = "cuarteles";
	this->state = EST_QUIETO;
}

Arqueria::Arqueria() {
	this->tamX = 4;
	this->tamY = 4;
	this->name = "arqueria";
	this->state = EST_QUIETO;
}

Establo::Establo() {
	this->tamX = 4;
	this->tamY = 4;
	this->name = "establo";
	this->state = EST_QUIETO;
}