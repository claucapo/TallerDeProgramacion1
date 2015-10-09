#include "Jugador.h"
#include "Vision.h"
#include "Entidad.h"
#include "Matriz.h"
#include "Posicion.h"

#include <list>
#include <iostream>
#include <conio.h>

using namespace std;

#define TEST_SIZE 20


void imprimirVision(Vision* vis) {
	for (int i = 0; i < TEST_SIZE; i++) {
		for (int j = 0; j < TEST_SIZE; j++) {
			Posicion pos(i, j);
			estado_vision_t state = vis->visibilidadPosicion(pos);
			switch (state) {
			case VIS_NO_EXPLORADA:
				cout << "-"; break;
			case VIS_VISITADA:
				cout << "X"; break;
			case VIS_OBSERVADA:
				cout << "0"; break;
			}
		}
		cout << endl;
	}
}

void testVision() {
	Jugador p1 = Jugador("player1");
	p1.asignarVision(TEST_SIZE, TEST_SIZE);

	Vision* v1 = p1.verVision();
	list<Posicion> posiciones;

	for (int i = 0; i < 10; i++) {
		Posicion pos = Posicion(i, i);
		posiciones.push_front(pos);
	}
	p1.agregarPosiciones(posiciones);

	cout << "Jugador1" << endl;
	imprimirVision(v1);
	cout << endl << endl;

	p1.reiniciarVision();
	cout << "Jugador1" << endl;
	imprimirVision(v1);
	cout << endl << endl;

	Entidad* ent = new Entidad("champion", 1, 1, 2);
	Matriz mat = Matriz(TEST_SIZE, TEST_SIZE);
	Posicion* origen = new Posicion (15, 15);
	ent->asignarPos(origen);
	mat.ubicarEntidad(ent, origen);

	posiciones = mat.posicionesVistas(ent);
	p1.agregarPosiciones(posiciones);
	cout << "Jugador1" << endl;
	imprimirVision(v1);
	cout << endl << endl;

	p1.reiniciarVision();
	cout << "Jugador1" << endl;
	imprimirVision(v1);
	cout << endl << endl;
}