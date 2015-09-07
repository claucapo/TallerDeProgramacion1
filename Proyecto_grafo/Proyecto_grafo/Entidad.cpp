#include "Entidad.h"
#include "Posicion.h"

// El constructor sobrecargado inicializa la entidad en una posici�n determinada
// por la referencia del par�metro
Entidad::Entidad(Posicion* p) {
	if (!p) 
		this->pos = nullptr;
	 else 
		this->pos = p;
	this->sprites = nullptr;
	this->state = EST_QUIETO;
}

// El constructor por defecto inicializa la posici�n en nullptr
Entidad::Entidad(void) {
	this->pos = nullptr;
	this->sprites = nullptr;
	this->state = EST_QUIETO;
}

// Llamo al destructor de todos los miembros de la clase (en caso de que alguno
// necesitara librar memoria)
Entidad::~Entidad(void) {
	if(sprites)
		delete sprites;
}

void Entidad::asignarSprite(string name) {
	this->spriteBaseName = name;
	this->sprites = new Spritesheet(name + IMG_EXT, 1, 1, 0, 0);
}

// M�todo para avanzar un frame
void Entidad::avanzarFrame(void) {
	this->sprites->siguienteFrame();
}

void Entidad::asignarPos(Posicion* pos)
{
	if(pos)
		this->pos = pos;
}
