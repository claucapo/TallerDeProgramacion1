#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#pragma once

/* La clase Partida es la fachada principal
para acceder a toda la l�gica (el modelo)
del juego. */
class Partida
{
public:
	// Constructor y destructor por defecto
	Partida(void);
	~Partida(void);

	// Funci�n que avanza la l�gica del modelo
	// en un frame. B�sicamente, llama a la
	// misma funci�n del Escenario.
	void avanzarFrame(void);

	// Crea una Entidad en la Posicion pos y
	// la agrega en el mapa y al Jugador que
	// corresponda.
	// Pre: pos est� vac�a.
	// Post: se agreg� la Entidad en pos, o
	// bien no si esta estaba ocupada. �Lanzar
	// excepci�n?
	void crearEntidad(Entidad entidad, Posicion pos);

	// Mueve la Unidad recibida en direcci�n
	// a la Posicion de destino.
	// Pre: La Unidad estaba en el mapa,
	// destino es una Posicion del mapa.
	void moverUnidad(Unidad unidad, Posicion destino);
};

