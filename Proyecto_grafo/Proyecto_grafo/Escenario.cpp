#include "Escenario.h"
#include "Matriz.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Unidad.h"
#include <list>
#define TAM_DEFAULT 50

// Constructor especificando tamanio
Escenario::Escenario(int casillas_x, int casillas_y) {
	mapa = new Matriz(casillas_x, casillas_y);
	this->entidades = list<Entidad*>();
	this->tamX = casillas_x;
	this->tamY = casillas_y;
	this->protagonista = nullptr;
}

// Constructor por defecto
Escenario::Escenario(void) {
	mapa = new Matriz(TAM_DEFAULT, TAM_DEFAULT);
	this->entidades = list<Entidad*>();
	this->tamX = TAM_DEFAULT;
	this->tamY = TAM_DEFAULT;
	this->protagonista = nullptr;
}

// Destructor
Escenario::~Escenario(void) {
	delete mapa;
	if (this->protagonista)
		delete protagonista;
	while (!this->entidades.empty()) {
		delete this->entidades.front();
		this->entidades.pop_front();
	}
}

void Escenario::moverProtagonista(void) {
	Unidad* unit = this->protagonista;
	Estados_t state = unit->verEstado();
	Spritesheet* sp = unit->verSpritesheet();

	// Si se esta moviendo
	if (state == EST_CAMINANDO) {
		Posicion* act = unit->verPosicion();
		Posicion* dest = unit->verDestino();

		// Distantcias
		float distX = dest->getX() - act->getX();
		float distY = dest->getY() - act->getY();
		float totalDist = sqrt((distX*distX) + (distY*distY));	

		unit->setDireccion(unit->calcularDirecion(distX, distY));
		float rapidez = unit->verVelocidad();
		sp->cambiarImagen(unit->name + "_move");
		sp->siguienteFrame();
		sp->cambiarSubImagen(sp->calcularOffsetX()/sp->subImagenWidth(), unit->verDireccion());

		if (totalDist > rapidez) {
			float nuevoX = act->getX() + (distX*rapidez)/totalDist;
			float nuevoY = act->getY() + (distY*rapidez)/totalDist;
			Posicion nuevaPos(nuevoX, nuevoY);
			unit->asignarPos(&nuevaPos);
		} else {
			unit->setEstado(EST_QUIETO);
			sp->cambiarImagen(unit->name);
		}
	}
	else
		sp->siguienteFrame();
}


template <typename T> bool compare(const T* const & a, const T* const &b) {
	return *a < *b;
};

void Escenario::avanzarFrame(void) {
	// Avanzo el frame en cada edificio (por ahora no hace nada)
	for(list<Entidad*>::const_iterator it = entidades.begin(); it != entidades.end(); ++it)
		(*it)->avanzarFrame();

	// Modifico la posición del protagonista
	if (this->protagonista) {
		moverProtagonista();
	}
}


bool Escenario::ubicarEntidad(Entidad* entidad, Posicion* pos) {
	if (mapa->ubicarEntidad(entidad, pos)) {
		entidades.push_back(entidad);
		entidad->asignarPos(pos);
		entidades.sort(compare<Entidad>);
		return true;
	}
	return false;
}

void Escenario::quitarEntidad(Entidad* entidad) {
	mapa->quitarEntidad(entidad);
	entidades.remove(entidad);
}


bool Escenario::asignarProtagonista(Unidad* unidad, Posicion* pos) {
	if (pos && mapa->posicionPertenece(pos)) {
		unidad->asignarPos(pos);
		unidad->setEstado(EST_QUIETO);
		this->protagonista = unidad;
		return true;
	}
	return false;
}

void Escenario::asignarDestinoProtagonista(Posicion* pos) {
	if (this->protagonista) {
		if (this->mapa->posicionPertenece(pos)) {
			this->protagonista->nuevoDestino(pos);
			this->protagonista->setEstado(EST_CAMINANDO);
		}
	}
}

Unidad* Escenario::verProtagonista(void) {
	return this->protagonista;
}


list<Entidad*> Escenario::verEntidades(void) {
	return this->entidades;
}
