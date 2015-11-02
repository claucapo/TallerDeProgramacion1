#ifndef VISION_H
#define VISION_H

#include "Posicion.h"

// Los tres estados distintos de una determinada casilla:
// VIS_NO_EXPLORADA ---> La casilla nunca ha sido visitada por el jugador. No se mostrará en el mapa.
// VIS_VISITADA ---> El jugador ha pasado alguna vez por la casilla, pero no tiene unidades ni edificios
//					cerca de ella en el momento. Se muestra pero con niebla de guerra (ligeramente más oscura y sin animaciones ni unidades).
// VIS_OBSREVADA ---> La casilla actualmente se encuentra en el rango de visión de al menos una entidad del jugador
//						por lo que se muestra normalmente.

enum estado_vision_t {VIS_NO_EXPLORADA, VIS_VISITADA, VIS_OBSERVADA};

class Vision {
private:
	estado_vision_t** mapa_de_vision;
	int filas, columnas;

	Vision(void);

	estado_vision_t** crearVisionVacia(int filas, int columnas);

public:
	// Crea un mapa de visión de filas x columnas
	Vision(int filas, int columnas);
	~Vision(void);

	// Resetea la visión de manera que el mapa no contenga ninguna casilla
	// observada.
	void resetearVisibles(void);

	// Agrega una casilla observada actualmente
	void agregarPosicionObservada(Posicion pos);

	// Devuelve el estado de vision de una determinada posicion
	estado_vision_t visibilidadPosicion(Posicion pos);

	estado_vision_t* visibilidadArray(void);
	void setFromArray(estado_vision_t*);

};



#endif // VISION_H