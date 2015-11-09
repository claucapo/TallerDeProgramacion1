#include "Recurso.h"
#include "Escenario.h"
#include "Jugador.h"
#include <iostream>

using namespace std;

Recurso::Recurso(unsigned int id, string name, tipoEntidad_t pType) : Entidad(id, name, pType) {
	this->recursoMax = pType.recursoMax;
	this->recursoAct = pType.recursoMax;

	this->tipoR = pType.tipoR;
}

af_result_t Recurso::avanzarFrame(Escenario* scene) {
	if (this->state == EST_MUERTO || this->recursoAct == 0) {
		return AF_KILL;
	}	
	return AF_NONE;
}

		/*
		Entidad* ent = scene->verProtagonista();
		Jugador* player = ent->verJugador();
		if (player) {
			int distancia = scene->verMapa()->distanciaEntre(*ent->verPosicion(), this);
			if (distancia > 1)
				return false;

			player->modificarRecurso(this->amount);
			cout << player->verNombre() << " tiene " << player->verRecurso() << " recurso/s." << endl;
			return true;
		}		
		*/

		// Cambiar por un algoritmo de devolver adyacentes?
		/* for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				Posicion act = Posicion(this->pos->getRoundX() + i, this->pos->getRoundY() + j);
				Entidad* ent = scene->verMapa()->verContenido(&act);
				// Si la posici�n no es la del recurso original
				if ( ent && (i != 0 || j != 0) ) {
					Jugador* player = ent->verJugador();
					if ((player && player->verID() != 0) && (ent->tipo == ENT_T_UNIT)) {
						player->modificarRecurso(this->tipoR, this->recursoAct);
						// Print temporal de recursos!!
						cout << player->verNombre() << " tiene " << player->verRecurso().oro << " oro, ";
						cout << player->verRecurso().comida << " comida, ";
						cout << player->verRecurso().madera << " madera y ";
						cout << player->verRecurso().piedra << " piedra." << endl;
						return AF_KILL;
					}
				}
			}
		}
		*/