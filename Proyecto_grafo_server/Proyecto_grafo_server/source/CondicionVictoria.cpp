#include "CondicionVictoria.h"
#include "FactoryEntidades.h"
#include <vector>

#include "Escenario.h"
#include "Partida.h"
#include "Entidad.h"
#include "Jugador.h"

#include <iostream>

using namespace std;

CondicionVictoria::CondicionVictoria(int cantJugadores, unsigned int tipoUnidadCritica, tipo_derrota_t accion) {
	this->accionAlPerder = accion;

	string nombreUnidad = FactoryEntidades::obtenerInstancia()->obtenerName(tipoUnidadCritica);
	if (nombreUnidad == nombre_entidad_def)
		this->tipoUnidadCritica = 0;
	else
		this->tipoUnidadCritica = tipoUnidadCritica;

	this->cantUnidadesCriticas = vector<unsigned int>(cantJugadores);
	this->ultimoAtacante = vector<unsigned int>(cantJugadores);
	this->sigueJugando = vector<bool>(cantJugadores);
	this->cantJugadores = cantJugadores;
	this->jugadores = vector<Jugador*>(cantJugadores);
	for (int i = 0; i < cantJugadores; i++) {
		this->cantUnidadesCriticas[i] = 0;
		this->ultimoAtacante[i] = 0;
		this->sigueJugando[i] = true;
		this->jugadores[i] = nullptr;
	}
}


void CondicionVictoria::inicializar(Partida* game) {
	for (list<Jugador*>::const_iterator it = game->jugadores.begin(); it != game->jugadores.end(); it++) {
		this->jugadores[(*it)->verID()] = (*it);
	}

	Escenario* scene = game->escenario;
	list<Entidad*> entidades = scene->verEntidades();
	list<Entidad*>::const_iterator iter;

	for (iter = entidades.begin(); iter != entidades.end(); iter++) {
		if ((*iter)->typeID == this->tipoUnidadCritica || this->tipoUnidadCritica == 0)
			this->cantUnidadesCriticas[(*iter)->verJugador()->verID()]++;
	}
}



void CondicionVictoria::spawnEntidad(Entidad* ent) {
	if (ent->typeID == this->tipoUnidadCritica || this->tipoUnidadCritica == 0)
		this->cantUnidadesCriticas[ent->verJugador()->verID()]++;
}



bool CondicionVictoria::deleteEntidad(Entidad* ent) {
	if (ent->typeID == this->tipoUnidadCritica || this->tipoUnidadCritica == 0)
		this->cantUnidadesCriticas[ent->verJugador()->verID()]--;

	this->ultimoAtacante[ent->verJugador()->verID()] = (unsigned int) (ent->verEstado() - EST_MUERTO);

	if (this->cantUnidadesCriticas[ent->verJugador()->verID()] == 0) {
		cout << "Jugador " << ent->verJugador()->verID() << " ha perdido." << endl;
		return true;
	}
	return false;
}


int CondicionVictoria::cantJugadoresActivos(void) {
	int cant = 0;
	for (int i = 1; i < cantJugadores; i++) {
		if (this->sigueJugando[i])
			cant++;
	}
	return cant;
}

unsigned int CondicionVictoria::verUltimoJugador(void) {
	for (int i = 1; i < cantJugadores; i++) {
		if (this->sigueJugando[i])
			return i;
	}
	return 0;
}