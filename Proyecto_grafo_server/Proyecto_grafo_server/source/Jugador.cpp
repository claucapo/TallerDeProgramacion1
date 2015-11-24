#include "Jugador.h"
#include "Entidad.h"
#include "ErrorLog.h"
#include "Unidad.h"

Jugador::Jugador(string name, int id, string color){
	this->nombre = name;
	this->nombre_def = name;
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


bool Jugador::puedePagar(recursos_jugador_t costo) {
	bool puedePagar = true;
	puedePagar &= (costo.comida <= recursos.comida);
	puedePagar &= (costo.madera <= recursos.madera);
	puedePagar &= (costo.oro <= recursos.oro);
	puedePagar &= (costo.piedra <= recursos.piedra);
	return puedePagar;
}

void Jugador::gastarRecursos(recursos_jugador_t costo) {
	recursos.comida -= costo.comida;
	recursos.oro -= costo.oro;
	recursos.madera -= costo.madera;
	recursos.piedra -= costo.piedra;
	this->resources_dirty = true;
}

void Jugador::asingarNombre(string name) {
	this->nombre = name;
}
	

void Jugador::resetearNombre(void) {
	this->nombre = this->nombre_def;
}