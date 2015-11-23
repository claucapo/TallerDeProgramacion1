#include "Partida.h"
#include "Escenario.h"
#include "Jugador.h"
#include "ErrorLog.h"
#include "Protocolo.h"
#include "Enumerados.h"
#include "FactoryEntidades.h"
#include "Edificio.h"

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

void Partida::inicializarCondicionVictoria(unsigned int tipoUnidad, tipo_derrota_t accion) {
	this->vCond = CondicionVictoria(this->jugadores.size(), tipoUnidad, accion);
	this->vCond.inicializar(this);
}


list<msg_update*> Partida::avanzarFrame(void){
	for (list<Jugador*>::iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		Jugador* act = (*iter);
		act->reiniciarVision();
	}
		// Acá adentro se asignan las casillas vistas de cada jugador

	list<msg_update*> updates = this->escenario->avanzarFrame(&this->vCond);
	
	for (list<Jugador*>::iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		Jugador* act = (*iter);
		// Si modifiqué los recursos, genero las updates
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


void Partida::procesarEvento(msg_event msj, unsigned int source) {
	Escenario* scene = this->escenario;
	CodigoMensaje accion = msj.accion;
	Entidad* ent = nullptr;
	Entidad* aux = nullptr;
	Edificio* building = nullptr;
	Posicion destino;
	ent = scene->obtenerEntidad(msj.idEntidad);

	// Según la acción que sea, hago lo que corresponda
	switch(accion){
	case MSJ_MOVER:
		ent->asignarAccion(ACT_NONE, ent->verID());
		destino = Posicion(msj.extra1, msj.extra2);
		scene->asignarDestino(msj.idEntidad, destino);
		break;

	case MSJ_RECOLECTAR:
		cout << "Recibi un recolectar" << endl;
		if (ent)
			ent->asignarAccion(ACT_COLLECT, (unsigned int)msj.extra1);
		break;

	case MSJ_ATACAR:
		cout << "Recibi un recolectar" << endl;
		if (ent)
			ent->asignarAccion(ACT_ATACK, (unsigned int)msj.extra1);
		break;

	case MSJ_CONSTRUIR:
		cout << "Recibi un construir" << endl;
		destino = Posicion(msj.extra1, msj.extra2);
		aux = this->escenario->obtenerEntidad(destino);
		if (ent && aux)
			ent->asignarAccion(ACT_BUILD, aux->verID());
		break;

	case MSJ_PRODUCIR_UNIDAD:
		if (ent->tipo == ENT_T_BUILDING) {
			building = (Edificio*)ent;
			bool success = building->entrenarUnidad(msj.extra1);
			if (success) {
				msg_update* upd = this->escenario->generarUpdate(MSJ_PRODUCIR_UNIDAD, ent->verID(), msj.extra1, building->ticks_totales);
				this->escenario->updatesAux.push_back(upd);
			}
		}
		break;

	case MSJ_NUEVO_EDIFICIO:
		cout << "Recibi un construir edificio" << endl;

		tipoEntidad_t* pType = FactoryEntidades::obtenerInstancia()->obtenerPrototipo(msj.idEntidad);
		if (!this->obtenerJugador(source)->puedePagar(pType->costo)) {
			cout << "Can't afford [" << pType->typeID << "]" << endl;
		} else {
			this->obtenerJugador(source)->gastarRecursos(pType->costo);

			ent = FactoryEntidades::obtenerInstancia()->obtenerEntidad(msj.idEntidad);
			destino = Posicion(msj.extra1, msj.extra2);
			ent->asignarJugador(this->obtenerJugador(source));
			((Edificio*)ent)->setEnConstruccion(true);
			this->escenario->ubicarEntidad(ent, &destino);
		
			msg_update* upd = this->escenario->generarUpdate(CodigoMensaje(MSJ_SPAWN + ent->typeID), ent->verID(), destino.getX(), destino.getY());
			this->escenario->updatesAux.push_back(upd);

			upd = this->escenario->generarUpdate(MSJ_ASIGNAR_JUGADOR, ent->verID(), ent->verJugador()->verID(), 0);
			this->escenario->updatesAux.push_back(upd);
		}

		break;
	}
}