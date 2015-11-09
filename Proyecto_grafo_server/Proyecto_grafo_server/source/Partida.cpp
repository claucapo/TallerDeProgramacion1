#include "Partida.h"
#include "Escenario.h"
#include "Jugador.h"
#include "ErrorLog.h"
#include "Protocolo.h"
#include "Enumerados.h"


#include <sstream> // Para convertir int en string

Partida::Partida(void) {
	this->jugadores = list<Jugador*>();
	this->escenario = nullptr;
	Jugador* gaia = new Jugador("gaia", 0, "green");
	gaia->settearConexion(true);
	this->jugadores.push_front(gaia);
}

Partida::~Partida(void) {
	list<Jugador*>::const_iterator iter = this->jugadores.begin();
	while(iter != this->jugadores.end()) {
		delete (*iter);
		iter++;
	}
	
	if (this->escenario);
		delete this->escenario;
}

bool Partida::agregarJugador(Jugador* jugador) {
	for( list<Jugador*>::const_iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		if ((*iter)->verNombre() == jugador->verNombre()) {
			ErrorLog::getInstance()->escribirLog("El jugador [" + jugador->verNombre() + "] ya existe en la partida.", LOG_ERROR);
			return false;
		} else if ((*iter)->verID() == jugador->verID()) {
			std::stringstream s;
			s << jugador->verID();
			ErrorLog::getInstance()->escribirLog("El con ID [" + s.str() + "] ya existe en la partida.", LOG_ERROR);
			return false;
		}
	}
	this->jugadores.push_front(jugador);
	return true;
}


void Partida::asignarEscenario(Escenario* escenario) {
	if (this->escenario)
		delete this->escenario;
	this->escenario = escenario;
}

Jugador* Partida::obtenerJugador(int id) {
	for( list<Jugador*>::const_iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		if ((*iter)->verID() == id) {
			return *iter;
		}
	}
	return nullptr;
}

list<msg_update*> Partida::avanzarFrame(void){
	for (list<Jugador*>::iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		Jugador* act = (*iter);
		act->reiniciarVision();
	}

	// Algo explota ac�
/*	list<Entidad*>::const_iterator it;
	list<Entidad*> entidades = this->escenario->verEntidades();
	int i = 0;
	for ( it = entidades.begin(); it != entidades.end(); ++it) {
		Entidad* ent = (*it);
		list<Posicion> posVistas = this->escenario->verMapa()->posicionesVistas(ent);
		(ent)->verJugador()->agregarPosiciones(posVistas);
	}
	*/
	// Ac� adentro se asignan las casillas vistas de cada jugador

	list<msg_update*> updates = this->escenario->avanzarFrame();
	
	for (list<Jugador*>::iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		Jugador* act = (*iter);
		// Si modifiqu� los recursos, genero las updates
		// Hacer lo mismo para el resto de los recursos
		if (act->resources_dirty) {
			msg_update* upd = new msg_update();
			upd->accion = MSJ_RECURSO_JUGADOR;
			upd->idEntidad = act->verID();
			upd->extra1 = (float)act->verRecurso().oro;
			upd->extra2 = (float)RES_T_GOLD;
			updates.push_back(upd);

			upd = new msg_update();
			upd->accion = MSJ_RECURSO_JUGADOR;
			upd->idEntidad = act->verID();
			upd->extra1 = (float)act->verRecurso().madera;
			upd->extra2 = (float)RES_T_WOOD;
			updates.push_back(upd);

			upd = new msg_update();
			upd->accion = MSJ_RECURSO_JUGADOR;
			upd->idEntidad = act->verID();
			upd->extra1 = (float)act->verRecurso().piedra;
			upd->extra2 = (float)RES_T_STONE;
			updates.push_back(upd);

			upd = new msg_update();
			upd->accion = MSJ_RECURSO_JUGADOR;
			upd->idEntidad = act->verID();
			upd->extra1 = (float)act->verRecurso().comida;
			upd->extra2 = (float)RES_T_FOOD;
			updates.push_back(upd);

			act->resources_dirty = false;
		}
	}

	return updates;
}


void Partida::procesarEvento(msg_event msj) {
	Escenario* scene = this->escenario;
	CodigoMensaje accion = msj.accion;
	// Seg�n la acci�n que sea, hago lo que corresponda
	switch(accion){
	case MSJ_MOVER:
		Posicion destino = Posicion(msj.extra1, msj.extra2);
		scene->asignarDestino(msj.idEntidad, destino);
	}
}