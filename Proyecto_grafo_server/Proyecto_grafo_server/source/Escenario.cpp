#include "Escenario.h"
#include "Matriz.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Unidad.h"
#include "Edificio.h"
#include "Jugador.h"
#include <iostream>
#include <list>
#define TAM_DEFAULT 50

using namespace std;
// Constructor especificando tamanio
Escenario::Escenario(int casillas_x, int casillas_y) {
	mapa = new Matriz(casillas_x, casillas_y);
	this->entidades = list<Entidad*>();
	this->tamX = casillas_x;
	this->tamY = casillas_y;
	
	this->updatesAux = list<msg_update*>();
}

// Constructor por defecto
Escenario::Escenario(void) {
	mapa = new Matriz(TAM_DEFAULT, TAM_DEFAULT);
	this->entidades = list<Entidad*>();
	this->tamX = TAM_DEFAULT;
	this->tamY = TAM_DEFAULT;

	this->updatesAux = list<msg_update*>();
}

// Destructor
Escenario::~Escenario(void) {
	delete mapa;
	while (!this->entidades.empty()) {
		delete this->entidades.front();
		this->entidades.pop_front();
	}
}


msg_update* Escenario::generarUpdate(CodigoMensaje accion, unsigned int id, float extra1, float extra2) {
	msg_update* upd = new msg_update();
	upd->accion = accion;
	upd->idEntidad = id;
	upd->extra1 = extra1;
	upd->extra2 = extra2;
	
	return upd;
}

template <typename T> bool compare(const T* const & a, const T* const &b) {
	return *a < *b;
};

list<msg_update*> Escenario::avanzarFrame(void) {

	// Lista de las entidades que murieron durante el frame
	list<Entidad*> toRmv = list<Entidad*>();

	// Lista de las entidades que deben spawnear en el frame
	list<Entidad*> toAdd = list<Entidad*>();

	list<msg_update*> updates = list<msg_update*>();

	for(list<Entidad*>::iterator it = entidades.begin(); it != entidades.end(); ++it) {
		af_result_t res = (*it)->avanzarFrame(this);
		msg_update* upd = nullptr;
		Posicion* posAux = (*it)->verPosicion();
		Entidad* entAux; Edificio* edAux;
		switch(res) {
		case AF_MOVE:
			upd = generarUpdate(MSJ_MOVER, (*it)->verID(), posAux->getX(), posAux->getY()); 
			break;
		
		case AF_STATE_CHANGE:
			upd = generarUpdate(MSJ_STATE_CHANGE, (*it)->verID(), (float)(*it)->verEstado(), (float)(*it)->targetID); 
			break;
		
		case AF_KILL:
			upd = generarUpdate(MSJ_ELIMINAR, (*it)->verID(), 0, 0);
			toRmv.push_back(*it);			
			break;

		case AF_SPAWN:
			edAux = (Edificio*)(*it);
			entAux = edAux->obtenerUnidadEntrenada();
			if (entAux)
				toAdd.push_back(entAux);
			break;
		}

		if (upd) {
			updates.push_back(upd);
		}

		// (*it)->verJugador()->agregarPosiciones(this->verMapa()->posicionesVistas(*it));
	}
	
	while (!this->updatesAux.empty()) {
		msg_update* upd = this->updatesAux.front();
		this->updatesAux.pop_front();
		updates.push_back(upd);
	}

	while(!toRmv.empty()) {
		Entidad* ent = toRmv.front();
		toRmv.pop_front();
		this->quitarEntidad(ent);
		delete ent;
	}

	while(!toAdd.empty()) {
		Entidad* ent = toAdd.front();
		toAdd.pop_front();

		// Acá va la lógica de spawn... hay que encontrar la posición vacía más cenrcana
		// a la que tiene cargada la entidad, y hacer que aparezca ahí.
		// Este método es muy ineficiente
		Posicion* pos = this->obtenerPosicionSpawn(ent->verPosicion());
		if (pos) {
			this->ubicarEntidad(ent, pos);
			msg_update* upd = generarUpdate(CodigoMensaje(MSJ_SPAWN + ent->typeID), ent->verID(), pos->getX(), pos->getY());
			updates.push_back(upd);

			upd = generarUpdate(MSJ_ASIGNAR_JUGADOR, ent->verID(), ent->verJugador()->verID(), 0);
			updates.push_back(upd);

			cout << "Debo spawnear un nuevo [" << ent->name << "] en " << pos->toStrRound() << endl;
		}

	}

	return updates;
}

Entidad* Escenario::obtenerEntidad(unsigned int entID) {
 	for(list<Entidad*>::iterator it = entidades.begin(); it != entidades.end(); ++it) {
 		if ( (*it)->verID() == entID ) {
			return (*it);
		}
	}
	return nullptr;
}

Entidad* Escenario::obtenerEntidad(Posicion pos) {
	Entidad* ent = this->mapa->verContenido(&pos);
	return ent;
}

bool Escenario::ubicarEntidad(Entidad* entidad, Posicion* pos) {
	if (mapa->ubicarEntidad(entidad, pos)) {
		entidades.push_back(entidad);
		entidad->asignarPos(pos);
		//entidades.sort(compare<Entidad>);
		return true;
	}
	return false;
}

void Escenario::quitarEntidad(Entidad* entidad) {
	mapa->quitarEntidad(entidad);
	entidades.remove(entidad);
}


void Escenario::moverEntidad(Entidad* entidad, Posicion* destino) {
	Posicion* aux = entidad->verPosicion();

	Entidad* ocupante = this->mapa->verContenido(destino);
	bool valida = ocupante == nullptr || ocupante == entidad;
	bool distinta = aux->getRoundX() != destino->getRoundX();
	distinta = distinta || ( aux ->getRoundY() != destino->getRoundY() );
	if (valida) {
		if (distinta) {
			this->mapa->quitarEntidad(entidad);
			this->mapa->ubicarEntidad(entidad, destino);
		}
		entidad->asignarPos(destino);
	}
}

 void Escenario::asignarDestino(unsigned int entID, Posicion pos) {
 	for(list<Entidad*>::iterator it = entidades.begin(); it != entidades.end(); ++it) {
 		if ( (*it)->verID() == entID ) {
 			if ( (*it)->tipo == ENT_T_UNIT ) {
 				Unidad* unit = (Unidad*)(*it);
 				unit->nuevoDestino(&pos);
				list<Posicion*> camino = mapa->caminoMinimo(*unit->verPosicion(),*unit->verDestino());
				if (!camino.empty()) {
					camino.pop_front();
					unit->marcarCamino(camino);
 					printf("seleccionado nuevo destino\n");
				} else {
					printf("no valid path\n");
				}
 			}
 		}
 	}
 }

 
Posicion* Escenario::obtenerPosicionSpawn(Posicion* origen) {
	if (this->casillaEstaVacia(origen)) {
		return origen;
	}
	Posicion* pos = nullptr;
	Entidad* ent = this->mapa->verContenido(origen);
	for (int i = 1; i <= ent->verRango(); i++) {
		list<Posicion> vistas = this->mapa->posicionesVistas(ent, i);
		while (!vistas.empty()) {
			Posicion act = vistas.front();
			vistas.pop_front();

			if (this->casillaEstaVacia(&act)) {
				return new Posicion(act.getX(), act.getY());
			}
		}
	}
	return pos;
}



list<Entidad*> Escenario::verEntidades(void) {
	return this->entidades;
}


