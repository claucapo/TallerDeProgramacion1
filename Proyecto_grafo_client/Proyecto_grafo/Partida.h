#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#include "Protocolo.h"
#include <list>
#pragma once

/* La clase Partida es la fachada principal
para acceder a toda la l�gica (el modelo)
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


	// Funci�n que avanza la l�gica del modelo
	// en un frame. B�sicamente, llama a la
	// misma funci�n del Escenario.
	void avanzarFrame(void);
	void seleccionarEntidad(Entidad* ent);
	void deseleccionarEntidades(void);
	list<Posicion*> verSeleccionados(void);
	Entidad* verEntidadSeleccionada(void);
};

