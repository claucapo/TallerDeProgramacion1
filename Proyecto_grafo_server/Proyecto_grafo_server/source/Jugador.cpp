#include "Jugador.h"
#include "Entidad.h"
#include "ErrorLog.h"
#include "Unidad.h"

Jugador::Jugador(string name, int id, string color){
	this->nombre = name;
	this->id = 1;
	if (id < 0)
		ErrorLog::getInstance()->escribirLog("Error al crear jugador [" + this->nombre + "]. ID invalida", LOG_ERROR);
	else
		this->id = id;
	
	this->color = color;
	this->vision = nullptr;
	this->recursos = recursos_jugador_t();
	this->esta_conectado = false;
	this->resources_dirty = false;
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

void Jugador::modificarRecurso(resource_type_t tipoR, int cant) {
	this->resources_dirty = true;
	switch (tipoR) {
	case RES_T_GOLD:
		this->recursos.oro += cant; break;
	case RES_T_WOOD:
		this->recursos.madera += cant; break;
	case RES_T_FOOD:
		this->recursos.comida += cant; break;
	case RES_T_STONE:
		this->recursos.piedra += cant; break;
	default:
		this->resources_dirty = false;
	}
}