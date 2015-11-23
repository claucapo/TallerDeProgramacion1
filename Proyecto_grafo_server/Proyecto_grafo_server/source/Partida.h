#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#include "CondicionVictoria.h"
#include "Protocolo.h"
#include <list>
#pragma once

/* La clase Partida es la fachada principal
para acceder a toda la l�gica (el modelo)
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

	// Funci�n que avanza la l�gica del modelo
	// en un frame. B�sicamente, llama a la
	// misma funci�n del Escenario.
	list<msg_update*> avanzarFrame(void);
};

