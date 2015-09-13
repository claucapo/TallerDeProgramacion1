#include "Edificios.h"

Casa::Casa() {
	this->tamX = 2;
	this->tamY = 2;
	this->name = "casa";
	this->state = EST_QUIETO;
}

CentroUrbano::CentroUrbano() {
	this->tamX = 4;
	this->tamY = 4;
	this->name = "centro_urbano";
	this->state = EST_QUIETO;
}