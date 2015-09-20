#include "Entidad.h"
#include "Posicion.h"

// El constructor sobrecargado inicializa la entidad en una posici�n determinada
// por la referencia del par�metro
Entidad::Entidad(Posicion* p) {
	if (!p) 
		this->pos = nullptr;
	 else 
		this->pos = new Posicion(*p);
	this->state = EST_QUIETO;
	this->tamX = 1;
	this->tamY = 1;
}

// El constructor por defecto inicializa la posici�n en nullptr
Entidad::Entidad(void) {
	this->pos = nullptr;
	this->state = EST_QUIETO;
	this->tamX = 1;
	this->tamY = 1;
}

// Llamo al destructor de todos los miembros de la clase (en caso de que alguno
// necesitara librar memoria)
Entidad::~Entidad(void) {
	if (this->pos)
		delete this->pos;
}

// M�todo para avanzar un frame
void Entidad::avanzarFrame(void) {
	this->sprites->siguienteFrame();
}

void Entidad::asignarPos(Posicion* pos) {
	if(pos) {
		delete this->pos;
		this->pos = new Posicion(*pos);		
	}
}

void Entidad::asignarSprite(Spritesheet* sp){
	this->sprites = sp;
}

bool Entidad::operator==(Entidad other){
	return ((this->pos == other.verPosicion()) && (this->state == other.verEstado()) );
}

Spritesheet* Entidad::verSpritesheet(void) {
	return sprites;
}
