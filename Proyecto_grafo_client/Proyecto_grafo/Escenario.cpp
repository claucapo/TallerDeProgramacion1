#include "Escenario.h"
#include "Matriz.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Unidad.h"
#include "Jugador.h"
#include <list>
#define TAM_DEFAULT 50

// Constructor especificando tamanio
Escenario::Escenario(int casillas_x, int casillas_y) {
	mapa = new Matriz(casillas_x, casillas_y);
	this->entidades = list<Entidad*>();
	this->tamX = casillas_x;
	this->tamY = casillas_y;
}

// Constructor por defecto
Escenario::Escenario(void) {
	mapa = new Matriz(TAM_DEFAULT, TAM_DEFAULT);
	this->entidades = list<Entidad*>();
	this->tamX = TAM_DEFAULT;
	this->tamY = TAM_DEFAULT;
}

// Destructor
Escenario::~Escenario(void) {
	delete mapa;
	while (!this->entidades.empty()) {
		delete this->entidades.front();
		this->entidades.pop_front();
	}
}

template <typename T> bool compare(const T* const & a, const T* const &b) {
	return *a < *b;
};

void Escenario::avanzarFrame(void) {
	// Avanzo el frame en cada edificio (por ahora no hace nada)
	list<Entidad*> toRmv = list<Entidad*>();
	for(list<Entidad*>::iterator it = entidades.begin(); it != entidades.end(); ++it) {
		/*if((*it)->verTipo() == ENT_T_UNIT){
			af_result_t res = ((Unidad*)(*it))->avanzarFrame(this);

		}
		else{*/
			af_result_t res = (*it)->avanzarFrame(this);
	//	}
		(*it)->verJugador()->agregarPosiciones(this->verMapa()->posicionesVistas(*it));
	}
	while(!toRmv.empty()) {
		Entidad* ent = toRmv.front();
		toRmv.pop_front();
		this->quitarEntidad(ent);
		delete ent;
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


void Escenario::asignarDestino(unsigned int entID, Posicion pos) {
	for(list<Entidad*>::iterator it = entidades.begin(); it != entidades.end(); ++it) {
		if ( (*it)->verID() == entID ) {
			if ( (*it)->tipo == ENT_T_UNIT ) {
				Unidad* unit = (Unidad*)(*it);
				unit->nuevoDestino(&pos);
			}
		}
	}
}

list<Entidad*> Escenario::verEntidades(void) {
	return this->entidades;
}

// TODO: Cambiar entidades de list<Entidad*> a un vector?
void Escenario::moverEntidad(unsigned int entID, Posicion* pos, bool seguirMoviendo) {
	
	for(list<Entidad*>::iterator it = entidades.begin(); it != entidades.end(); ++it) {
		if ( (*it)->verID() == entID ) {
			float xOld = (*it)->verPosicion()->getX();
			float yOld = (*it)->verPosicion()->getY();

			if(this->mapa->quitarEntidad(*it)){
				//this->mapa->ubicarEntidad(*it, pos);
				/*
				Entidad pepe = *(*it);
				Unidad* posta = (Unidad*) &pepe;
				posta->setDireccion(DIR_DOWN_LEFT);*/
				((Unidad*)(*it))->asignarPos(pos);
				this->mapa->ubicarEntidad(*it, pos);
			
			}
				Spritesheet* spEnt = (*it)->verSpritesheet();
				string nombreEnt = (*it)->verNombre();
				if(seguirMoviendo){
					(*it)->settearEstado(EST_CAMINANDO);
					nombreEnt += "_move";
					}
				else
					(*it)->settearEstado(EST_QUIETO);

				if((*it)->verJugador()->verID() == 2)
					nombreEnt = nombreEnt + '2';
				if((*it)->verJugador()->verID() == 3)
					nombreEnt = nombreEnt + '3';
				spEnt->cambiarImagen(nombreEnt);

		}
	}
	
	
}
