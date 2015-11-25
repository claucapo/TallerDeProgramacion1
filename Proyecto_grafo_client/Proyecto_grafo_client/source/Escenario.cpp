#include "Escenario.h"
#include "Matriz.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Unidad.h"
#include "Jugador.h"
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#define TAM_DEFAULT 50

// Constructor especificando tamanio
Escenario::Escenario(int casillas_x, int casillas_y) {
	mapa = new Matriz(casillas_x, casillas_y);
	this->entidades = vector<Entidad*>(CANT_ENTIDADES_INI);
	this->cant_entidades = 0;
	this->max_entidades = CANT_ENTIDADES_INI;
	this->tamX = casillas_x;
	this->tamY = casillas_y;
}

// Constructor por defecto
Escenario::Escenario(void) {
	mapa = new Matriz(TAM_DEFAULT, TAM_DEFAULT);
	this->entidades = vector<Entidad*>(CANT_ENTIDADES_INI);
	this->cant_entidades = 0;
	this->max_entidades = CANT_ENTIDADES_INI;
	this->tamX = TAM_DEFAULT;
	this->tamY = TAM_DEFAULT;
}

// Destructor
Escenario::~Escenario(void) {
	delete mapa;
	for (int i = 0; i < this->entidades.size(); i++) {
		delete this->entidades[i];
	}
	/*
	while (!this->entidades.empty()) {
		delete this->entidades.front();
		this->entidades.pop_front();
	}
	*/
}

template <typename T> bool compare(const T* const & a, const T* const &b) {
	return *a < *b;
};

bool Escenario::avanzarFrame(unsigned int actPlayer) {
	// Avanzo el frame en cada edificio (por ahora no hace nada)
	bool hayViolencia = false;
	for (int i = 0; i < this->cant_entidades; i++) {
		Entidad* act = this->entidades[i];

		act->avanzarFrame(this);

		if (act->verJugador()->verID() == actPlayer){
			if (act->verTipo() == ENT_T_UNIT)
				act->verJugador()->poblacionAct++;
			else if(act->verNombre() == "house")
				act->verJugador()->poblacionMax += 5;
			else if(act->verNombre() == "castle")
				act->verJugador()->poblacionMax += 20;
			else if(act->verNombre() == "town center")
				act->verJugador()->poblacionMax += 10;
			
			act->verJugador()->agregarPosiciones(this->verMapa()->posicionesVistas(act));
			if(act->verEstado() == EST_ATACANDO)
				hayViolencia = true;
		}
		if (act->verTipo() == ENT_T_UNIT){
			this->mapa->ocuparPosicionSinChequeo(act->verPosicion(), act);
		}
		
	}

	return hayViolencia;
}


bool Escenario::ubicarEntidad(Entidad* entidad, Posicion* pos) {
	if (mapa->ubicarEntidad(entidad, pos)) {
		if (cant_entidades > max_entidades - 5) {
			entidades.resize(max_entidades + RESIZE_AMOUNT);
			this->max_entidades += RESIZE_AMOUNT;
		}
		entidades[cant_entidades] = entidad;
		this->cant_entidades++;
		entidad->asignarPos(pos);
		sort(entidades.begin(), entidades.begin() + cant_entidades, compare<Entidad>);
		return true;
	}
	return false;
}

void Escenario::quitarEntidad(Entidad* entidad) {
	mapa->quitarEntidad(entidad);
	for (int i = 0; i < this->cant_entidades; i++) {
		if (entidades[i] == entidad) {
			entidades.erase(entidades.begin()+i);
			this->cant_entidades--;
		}
	}
}


void Escenario::asignarDestino(unsigned int entID, Posicion pos) {
	for (int i = 0; i < this->cant_entidades; i++) {
		if (entidades[i]->verID
			() == entID) {
			if ( entidades[i]->tipo == ENT_T_UNIT ) {
				Unidad* unit = (Unidad*)(entidades[i]);
				unit->nuevoDestino(&pos);
			}
		}
	}
}

vector<Entidad*> Escenario::verEntidades(void) {
	return this->entidades;
}

void Escenario::quitarEntidad(unsigned int entID) {
	for (int i = 0; i < this->cant_entidades; i++) {
		if (entidades[i]->verID() == entID) {
			mapa->quitarEntidad(entidades[i]);
			entidades.erase(entidades.begin()+i);
			this->cant_entidades--;
		}
	}
}


void Escenario::moverEntidad(unsigned int entID, Posicion* pos, bool seguirMoviendo) {

	Entidad* ent = NULL;
	for (int i = 0; i < this->cant_entidades; i++) {
		if (entidades[i]->verID() == entID) {
			ent = entidades[i];
		}
	}
	if (!ent) return;
				
	float xOld = ent->verPosicion()->getX();
	float yOld = ent->verPosicion()->getY();

	if(this->mapa->quitarEntidad(ent)){
		((Unidad*)(ent))->asignarPos(pos);
		this->mapa->ubicarEntidad(ent, pos);
		sort(entidades.begin(), entidades.begin() + cant_entidades, compare<Entidad>);
	}

	Spritesheet* spEnt = (ent)->verSpritesheet();
	string nombreEnt = (ent)->verNombre();
	if(seguirMoviendo){
		(ent)->settearEstado(EST_CAMINANDO);
		nombreEnt += "_move";
	}
	else{
		(ent)->settearEstado(EST_QUIETO);
	}

	if((ent)->verJugador()->verID() == 2)
		nombreEnt = nombreEnt + '2';

	if((ent)->verJugador()->verID() == 3)
		nombreEnt = nombreEnt + '3';
	spEnt->cambiarImagen(nombreEnt);
	
}


Entidad* Escenario::obtenerEntidad(unsigned int entID) {
 	for (int i = 0; i < this->cant_entidades; i++) {
		if (entidades[i]->verID() == entID) {
			return entidades[i];
		}
	}
	return nullptr;
}


void Escenario::derrotarJugador(unsigned int defeatedID, tipo_derrota_t accion, Jugador* newOwner) {
	list<Entidad*>::const_iterator iter, next;

	switch(accion) {
	case LOSE_ALL:
		for (int i = 0; i < this->cant_entidades;) {
			if (entidades[i]->verJugador()->verID() == defeatedID) {
				this->mapa->quitarEntidad(entidades[i]);
				this->entidades.erase(entidades.begin() + i);
				this->cant_entidades--;
			} else {
				i++;
			}
		}
		break;

	case LOSE_UNITS:
		for (int i = 0; i < this->cant_entidades;) {
			if (entidades[i]->verJugador()->verID() == defeatedID && entidades[i]->tipo == ENT_T_UNIT) {
				this->mapa->quitarEntidad(entidades[i]);
				this->entidades.erase(entidades.begin()+i);
				this->cant_entidades--;
			} else {
				i++;
			}
		}
		break;

	case TRANSFER_ALL:
		if (newOwner) {
			for (int i = 0; i < this->cant_entidades; i++) {
				if (entidades[i]->verJugador()->verID() == defeatedID) {
					entidades[i]->asignarJugador(newOwner);
				}			
			}
		}
		break;
	}
}