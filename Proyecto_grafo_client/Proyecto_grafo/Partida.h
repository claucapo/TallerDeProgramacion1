#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#include <list>
#pragma once

/* La clase Partida es la fachada principal
para acceder a toda la lógica (el modelo)
del juego. */
struct Partida {
	list<Jugador*> jugadores;
	Escenario* escenario;

	// Constructor y destructor por defecto
	Partida(void);
	~Partida(void);

	void agregarJugador(Jugador*);
	void asignarEscenario(Escenario*);

	// Función que avanza la lógica del modelo
	// en un frame. Básicamente, llama a la
	// misma función del Escenario.
	void avanzarFrame(void);
};

