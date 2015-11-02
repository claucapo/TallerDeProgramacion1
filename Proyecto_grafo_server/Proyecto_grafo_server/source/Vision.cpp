#include "Vision.h"
#include "ErrorLog.h"

#define VIS_TAM_DEFAULT 1

Vision::Vision(void) {
	this->filas = VIS_TAM_DEFAULT;
	this->columnas = VIS_TAM_DEFAULT;
	this->mapa_de_vision = crearVisionVacia(this->filas, this->columnas);
}

Vision::Vision(int filas, int columnas) {
	if (filas <= 0 || columnas <= 0) {
		ErrorLog::getInstance()->escribirLog("Error matriz visión con dimensiones inválidas.", LOG_ERROR);
		this->filas = VIS_TAM_DEFAULT;
		this->columnas = VIS_TAM_DEFAULT;
	} else {
		this->filas = filas;
		this->columnas = columnas;
	}
	this->mapa_de_vision = crearVisionVacia(this->filas, this->columnas);
}

// TODO: Chequear errores de memoria
estado_vision_t** Vision::crearVisionVacia(int filas, int columnas) {
	estado_vision_t** nVision = new estado_vision_t*[filas];
	for (int i = 0; i < filas; i++) {
		nVision[i] = new estado_vision_t[columnas];
		for (int j = 0; j < columnas; j++)
			nVision[i][j] = VIS_NO_EXPLORADA;
	}
	return nVision;
}

// Destructor, libera la memoria de la matriz
Vision::~Vision(void) {
	for (int i = 0; i < this->filas; i++)
		delete[] this->mapa_de_vision[i];
	delete[] this->mapa_de_vision;
}

// Se puede mejorar para que no tenga que revisar toda la matriz
// pero por ahora esto deberìa ser suficiente.
void Vision::resetearVisibles(void) {
	for (int i = 0; i < this->filas; i++)
		for (int j = 0; j < this->columnas; j++)
			if (this->mapa_de_vision[i][j] == VIS_OBSERVADA)
				this->mapa_de_vision[i][j] = VIS_VISITADA;
}


// Agrega una posicion a la vision de un jugador
void Vision::agregarPosicionObservada(Posicion pos) {
	int x = pos.getRoundX();
	int y = pos.getRoundY();
	if ( x >= this->filas || y >= this->columnas ) {
		// ErrorLog::getInstance()->escribirLog("Posicion " + pos.toStrRound() + " excede matriz de vision.", LOG_ERROR);
		return;
	}
	this->mapa_de_vision[x][y] = VIS_OBSERVADA;
}

estado_vision_t Vision::visibilidadPosicion(Posicion pos) {
	int x = pos.getRoundX();
	int y = pos.getRoundY();
	if ( x >= this->filas || y >= this->columnas ) {
		// ErrorLog::getInstance()->escribirLog("Posicion " + pos.toStrRound() + " excede matriz de vision.", LOG_ERROR);
		return VIS_NO_EXPLORADA;
	}
	return this->mapa_de_vision[x][y];
}

// Funciones utiles para enviar y recibir visibildiad

estado_vision_t* Vision::visibilidadArray(void) {
	int size = this->filas * this->columnas;
	estado_vision_t* varray = new estado_vision_t[size];
	for (int i = 0; i < this->filas; i++)
		for (int j = 0; j < this->columnas; j++)
			varray[i * this->columnas + j] = this->mapa_de_vision[i][j];

	return varray;
}

void Vision::setFromArray(estado_vision_t* varray) {
	if ( sizeof(*varray) == (this->filas * this->columnas * sizeof(estado_vision_t)) ) {
		for (int i = 0; i < this->filas; i++)
			for (int j = 0; j < this->columnas; j++)
				this->mapa_de_vision[i][j] = varray[i * this->columnas + j];
		delete[] varray;
	} else {
		ErrorLog::getInstance()->escribirLog("Asignación de visión por array invalida.", LOG_ERROR);
	}
}