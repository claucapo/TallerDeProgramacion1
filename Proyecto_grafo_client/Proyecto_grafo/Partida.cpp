#include "Partida.h"
#include "Escenario.h"
#include "Jugador.h"
#include "ErrorLog.h"

#include <sstream> // Para convertir int en string

Partida::Partida(void) {
	this->jugadores = list<Jugador*>();
	this->seleccionados = list<Posicion*>();
	this->escenario = nullptr;
	this->ent_seleccionada = nullptr;
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
	switch (accion) {
		case MSJ_ELIMINAR:
			this->escenario->quitarEntidad(msj.idEntidad); break;

		case MSJ_QUIETO:
			cout << "LLEGUE ACA" << endl;
			destino = Posicion(msj.extra1, msj.extra2);
			scene->moverEntidad(msj.idEntidad, &destino, false);
			if(this->ent_seleccionada != nullptr)
				this->seleccionarEntidad(this->ent_seleccionada);
			break;
		case MSJ_MOVER:
			cout << accion << endl;
			destino = Posicion(msj.extra1, msj.extra2);
		//	Entidad* entMovida = this->verEntidadSeleccionada();
			if(accion == MSJ_QUIETO){
				scene->moverEntidad(msj.idEntidad, &destino, false);
				cout << "ESTADO QUIETO" << endl;
			}
			else{
				scene->moverEntidad(msj.idEntidad, &destino, true);
				cout << "ESTADO MOVER" << endl;
			
			}
			if(this->ent_seleccionada != nullptr)
				this->seleccionarEntidad(this->ent_seleccionada);
			break;
	}
}

void Partida::seleccionarEntidad(Entidad* ent){
	this->deseleccionarEntidades();
	int coordX = ent->verPosicion()->getRoundX();
	int coordY = ent->verPosicion()->getRoundY();
	for(int i = 0; i < ent->verTamX(); i++) {
		for(int j = 0; j < ent->verTamY(); j++){
			Posicion* pos= new Posicion(coordX + i, coordY + j);
			this->seleccionados.push_front(pos);
		}
	}
	cout<< "seleccionado: " << ent->verNombre() << endl;
	this->ent_seleccionada = ent;
}


void Partida::deseleccionarEntidades(void){
	while(!this->seleccionados.empty()){
		Posicion* pAct = this->seleccionados.front();
		this->seleccionados.pop_front();
		delete pAct;
		}
	this->ent_seleccionada = nullptr;
}


list<Posicion*> Partida::verSeleccionados(void){
	return this->seleccionados;
}

Entidad* Partida::verEntidadSeleccionada(void) {
	return this->ent_seleccionada;
}
