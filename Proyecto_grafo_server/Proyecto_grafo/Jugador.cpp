#include "Jugador.h"
#include "Entidad.h"
#include "Unidad.h"

Jugador::Jugador(string name){
	this->nombre = name;
	this->vision = nullptr;
	this->recurso = 0;
	this->esta_conectado = false;
}

Jugador::~Jugador(void) {
	if (this->vision)
		delete this->vision;
}

void Jugador::asignarVision(int filas, int columnas) {
	if (this->vision)
		delete this->vision;
	this->vision = new Vision(filas, columnas);
}

bool Jugador::poseeEntidad(Entidad* entidad) {
	if (!entidad || !entidad->verJugador())
		return false;
	return (this->nombre == entidad->verJugador()->nombre);
}

void Jugador::reiniciarVision(void) {
	if (this->vision)
		this->vision->resetearVisibles();
}

void Jugador::agregarPosiciones(list<Posicion> posiciones) {
	if (!this->vision)
		return;
	list<Posicion>::const_iterator iter;
	for (iter = posiciones.begin(); iter != posiciones.end(); ++iter ) {
		this->vision->agregarPosicionObservada(*iter);
	}
}

estado_vision_t Jugador::visionCasilla(Posicion pos) {
	return this->vision->visibilidadPosicion(pos);
}

bool Jugador::estaConectado(void){
	return esta_conectado;
}


void Jugador::settearConexion(bool estadoConexion){
	this->esta_conectado = estadoConexion;
}
