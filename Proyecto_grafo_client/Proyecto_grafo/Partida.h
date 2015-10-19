#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#include "Protocolo.h"
#include <list>
#pragma once

/* La clase Partida es la fachada principal
para acceder a toda la lógica (el modelo)
del juego. */
struct Partida {
	list<Jugador*> jugadores;
	Escenario* escenario;
	list<Posicion*> seleccionados;
	Entidad* ent_seleccionada;

	// Constructor y destructor por defecto
	Partida(void);
	~Partida(void);

	bool agregarJugador(Jugador*);
	void asignarEscenario(Escenario*);

	Jugador* obtenerJugador(int id);
	
	void procesarUpdate(msg_update msj);


	// Función que avanza la lógica del modelo
	// en un frame. Básicamente, llama a la
	// misma función del Escenario.
	void avanzarFrame(void);
	void seleccionarEntidad(Entidad* ent);
	void deseleccionarEntidades(void);
	list<Posicion*> verSeleccionados(void);
	Entidad* verEntidadSeleccionada(void);
};

