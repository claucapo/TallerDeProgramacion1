#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#pragma once

/* La clase Partida es la fachada principal
para acceder a toda la lógica (el modelo)
del juego. */
class Partida
{
public:
	// Constructor y destructor por defecto
	Partida(void);
	~Partida(void);

	// Función que avanza la lógica del modelo
	// en un frame. Básicamente, llama a la
	// misma función del Escenario.
	void avanzarFrame(void);

	// Crea una Entidad en la Posicion pos y
	// la agrega en el mapa y al Jugador que
	// corresponda.
	// Pre: pos está vacía.
	// Post: se agregó la Entidad en pos, o
	// bien no si esta estaba ocupada. ¿Lanzar
	// excepción?
	void crearEntidad(Entidad entidad, Posicion pos);

	// Mueve la Unidad recibida en dirección
	// a la Posicion de destino.
	// Pre: La Unidad estaba en el mapa,
	// destino es una Posicion del mapa.
	void moverUnidad(Unidad unidad, Posicion destino);
};

