#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#include "CondicionVictoria.h"
#include "Protocolo.h"
#include <list>
#pragma once

/* La clase Partida es la fachada principal
para acceder a toda la lógica (el modelo)
del juego. */
struct Partida {
	list<Jugador*> jugadores;
	Escenario* escenario;
	CondicionVictoria vCond;

	// Constructor y destructor por defecto
	Partida(void);
	~Partida(void);

	bool agregarJugador(Jugador*);
	void asignarEscenario(Escenario*);
	void inicializarCondicionVictoria(unsigned int tipoUnidad, tipo_derrota_t accion);

	Jugador* obtenerJugador(int id);
	
	void procesarEvento(msg_event ev, unsigned int source);

	// Función que avanza la lógica del modelo
	// en un frame. Básicamente, llama a la
	// misma función del Escenario.
	list<msg_update*> avanzarFrame(void);
};

