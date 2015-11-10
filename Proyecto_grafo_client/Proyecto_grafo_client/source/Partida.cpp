#include "Partida.h"
#include "Escenario.h"
#include "Jugador.h"
#include "Enumerados.h"
#include "ErrorLog.h"
#include "Recurso.h"

#include <sstream> // Para convertir int en string

Partida::Partida(void) {
	this->jugadores = list<Jugador*>();
	this->seleccionados = list<Posicion*>();
	this->escenario = nullptr;
	this->ent_seleccionadas = list<Entidad*>();
	Jugador* gaia = new Jugador("gaia", 0, "green");
	gaia->settearConexion(true);
	this->jugadores.push_front(gaia);
	sx = 0; sy = 0; sx2 = 0; sy2 = 0;
	algoSeleccionado = false;
	this->seleccionSecundaria = nullptr;
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

void Partida::avanzarFrame(void){
	for (list<Jugador*>::iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		Jugador* act = (*iter);
		act->reiniciarVision();
	}
	// Acá adentro se asignan las casillas vistas de cada jugador
	this->escenario->avanzarFrame();
}


void Partida::procesarUpdate(msg_update msj) {
	Escenario* scene = this->escenario;
	CodigoMensaje accion = msj.accion;
	// Según la acción que sea, hago lo que corresponda
	Posicion destino;
	Entidad* ent = nullptr;
	Estados_t nState;
	switch (accion) {
		case MSJ_ELIMINAR:
			this->escenario->quitarEntidad(msj.idEntidad); break;

		case MSJ_STATE_CHANGE:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			nState = ent->verEstado();
			switch ((int)msj.extra1) {
			case 0: nState = EST_QUIETO; break;
			case 1: nState = EST_CAMINANDO; break;
			case 2: nState = EST_ATACANDO; break;
			case 3: nState = EST_RECOLECTANDO; break;
			case 4: nState = EST_CONSTRUYENDO; break;
			}
			ent->settearEstado(nState);

			if(nState == EST_QUIETO){
				Spritesheet* spEnt = (ent)->verSpritesheet();
				string nombreEnt = (ent)->verNombre();
				if((ent)->verJugador()->verID() == 2)
					nombreEnt = nombreEnt + '2';
				if((ent)->verJugador()->verID() == 3)
					nombreEnt = nombreEnt + '3';
				spEnt->cambiarImagen(nombreEnt);
			}
		//	if(!this->ent_seleccionadas.empty())
		//		this->seleccionarEntidad(this->ent_seleccionadas.front(), false);
			break;

		case MSJ_MOVER:
			destino = Posicion(msj.extra1, msj.extra2);
			scene->moverEntidad(msj.idEntidad, &destino, true);
			if(this->ent_seleccionadas.size() == 1)
				this->seleccionarEntidad(this->ent_seleccionadas.front(), true);
			break;

		case MSJ_RES_CHANGE:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			if (ent && ent->tipo == ENT_T_RESOURCE) {
				Recurso* res = (Recurso*)ent;
				res->recursoAct -= msj.extra1;
			}
			break;

		case MSJ_VIDA_CHANGE:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			if (ent) {
				ent->vidaAct -= msj.extra1;
				if (ent->vidaAct <= 0)
					ent->vidaAct = 0;
			}
			break;
			
	}
}

void Partida::seleccionarEntidad(Entidad* ent, bool emptyFirst){
	if(emptyFirst)
		this->deseleccionarEntidades();
	int coordX = ent->verPosicion()->getRoundX();
	int coordY = ent->verPosicion()->getRoundY();
	for(int i = 0; i < ent->verTamX(); i++) {
		for(int j = 0; j < ent->verTamY(); j++){
			Posicion* pos= new Posicion(coordX + i, coordY + j);
			
			this->seleccionados.push_front(pos);
		}
	}
	// cout<< "seleccionado: " << ent->verNombre() << endl;
	this->ent_seleccionadas.push_back(ent);
}


void Partida::deseleccionarEntidades(void){
	while(!this->seleccionados.empty()){
		Posicion* pAct = this->seleccionados.front();
		this->seleccionados.pop_front();
		delete pAct;
		}
	while(!this->ent_seleccionadas.empty())
		this->ent_seleccionadas.pop_front();
}


list<Posicion*> Partida::verSeleccionados(void){
	return this->seleccionados;
}

Entidad* Partida::verEntidadSeleccionada(void) {
	return ent_seleccionadas.front();
}

list<Entidad*> Partida::verListaEntidadesSeleccionadas(void){
	return this->ent_seleccionadas;
}