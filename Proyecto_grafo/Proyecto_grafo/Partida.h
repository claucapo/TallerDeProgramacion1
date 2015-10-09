#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#include <list>
#pragma once

/* La clase Partida es la fachada principal
para acceder a toda la l�gica (el modelo)
del juego. */
struct Partida {
	list<Jugador*> jugadores;
	Escenario* escenario;

	// Constructor y destructor por defecto
	Partida(void);
	~Partida(void);

	void agregarJugador(Jugador*);
	void asignarEscenario(Escenario*);

	// Funci�n que avanza la l�gica del modelo
	// en un frame. B�sicamente, llama a la
	// misma funci�n del Escenario.
	void avanzarFrame(void);
};

