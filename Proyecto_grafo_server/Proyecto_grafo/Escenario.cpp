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

list<msg_update*> Escenario::avanzarFrame(void) {
	list<msg_update*> updates = list<msg_update*>();
	// Avanzo el frame en cada edificio (por ahora no hace nada)
	list<Entidad*> toRmv = list<Entidad*>();
	for(list<Entidad*>::iterator it = entidades.begin(); it != entidades.end(); ++it) {
		af_result_t res = (*it)->avanzarFrame(this);
		msg_update* upd;
		switch (res) {
		case AF_MOVE:
			upd = new msg_update();
			upd->idEntidad = (*it)->verID();
			if(((Unidad*)(*it))->verEstado() == EST_QUIETO)
				upd->accion = MSJ_QUIETO;
			else
				upd->accion = MSJ_MOVER;
			upd->extra1 = (*it)->verPosicion()->getX();
			upd->extra2 = (*it)->verPosicion()->getY();
			updates.push_back(upd);
			break;
		case AF_KILL:
			toRmv.push_front(*it);
			upd = new msg_update();
			upd->idEntidad = (*it)->verID();
			upd->accion = MSJ_ELIMINAR;
			updates.push_back(upd);
			break;
		case AF_NONE:
		default:
			break;
		}
		(*it)->verJugador()->agregarPosiciones(this->verMapa()->posicionesVistas(*it));
	}
	while(!toRmv.empty()) {
		Entidad* ent = toRmv.front();
		toRmv.pop_front();
		this->quitarEntidad(ent);
		delete ent;
	}

	return updates;
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
				unit->marcarCamino(mapa->caminoMinimo(*unit->verPosicion(),*unit->verDestino()));
				printf("seleccionado nuevo destino\n");
			}
		}
	}
}

list<Entidad*> Escenario::verEntidades(void) {
	return this->entidades;
}
