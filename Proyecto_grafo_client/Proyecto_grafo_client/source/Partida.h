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
	list<Entidad*> ent_seleccionadas;
	int sx, sy, sx2, sy2;
	bool algoSeleccionado;
	Posicion* seleccionSecundaria;

		string edificioAubicar;
		bool modoUbicarEdificio;
	// Constructor y destructor por defecto
	Partida(void);
	~Partida(void);

	bool agregarJugador(Jugador*);
	void asignarEscenario(Escenario*);

	bool edificioUbicablePuedeConstruirse(Posicion pos);

	Jugador* obtenerJugador(int id);
	
	void procesarUpdate(msg_update msj);


	// Función que avanza la lógica del modelo
	// en un frame. Básicamente, llama a la
	// misma función del Escenario.
	void avanzarFrame(unsigned int playerID);
	void seleccionarEntidad(Entidad* ent, bool emptyFirst);
	void deseleccionarEntidades(void);
	list<Posicion*> verSeleccionados(void);
	Entidad* verEntidadSeleccionada(void);
	list<Entidad*> verListaEntidadesSeleccionadas(void);
};

