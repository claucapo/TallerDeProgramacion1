#include "Partida.h"
#include "Escenario.h"
#include "Jugador.h"
#include "ErrorLog.h"

Partida::Partida(void) {
	this->jugadores = list<Jugador*>();
	this->escenario = nullptr;
	Jugador* gaia = new Jugador("gaia");
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

void Partida::agregarJugador(Jugador* jugador) {
	for( list<Jugador*>::const_iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		if ((*iter)->verNombre() == jugador->verNombre()) {
			ErrorLog::getInstance()->escribirLog("El jugador [" + jugador->verNombre() + "] ya existe en la partida.", LOG_ERROR);
			return;
		}
	}
	this->jugadores.push_front(jugador);
}


void Partida::asignarEscenario(Escenario* escenario) {
	if (this->escenario)
		delete this->escenario;
	this->escenario = escenario;
}

void Partida::avanzarFrame(void){
	this->escenario->avanzarFrame();
}
