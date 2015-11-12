#include "Partida.h"
#include "Entidad.h"
#include "Unidad.h"
#include "Cliente.h"
#include "GraficadorPantalla.h"
#include "Escenario.h"
#include "Posicion.h"
#pragma once

using namespace std;

class ProcesadorEventos
{
public:
	Partida* game;
	GraficadorPantalla* gp;
	ProcesadorEventos(void);
	ProcesadorEventos(Partida* partida, GraficadorPantalla* grafP);
	~ProcesadorEventos(void);

	void procesarBoton(Jugador* player);
	void procesarSeleccionMultiple(Jugador* player);
	void procesarMover(Cliente* client);
	void procesarSeleccionSecundaria(Cliente* client, Jugador* player);
	void procesarRecolectar(Cliente* client);
	void procesarAtacar(Cliente* client);
	void procesarSeleccion(Jugador* player);

	Posicion adyacenteSiguiente(Posicion pos, int i, Escenario* scene);

};

struct lex_compare {
    bool operator() (const Posicion& p1, const Posicion& p2) {
	if ( ((Posicion)(p1)).getX() == ((Posicion)(p2)).getX() )
		return ((Posicion)(p1)).getY() < ((Posicion)(p2)).getY();
    return ((Posicion)(p1)).getX() < ((Posicion)(p2)).getX();

		return true;
	
	int dist1 = p1.getRoundX() + p1.getRoundY();
	int dist2 = p2.getRoundX() + p2.getRoundY();
	if (dist1 < dist2)
		return true;
	else
		return false; }
};

