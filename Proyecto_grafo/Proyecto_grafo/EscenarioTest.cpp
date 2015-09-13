#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#include "Edificios.h"
#include "Protagonistas.h"
#include <iostream>

using namespace std;

void imprimirEscenario(Escenario* scene) {
	for(int i = 0; i < scene->verTamX(); i++) {
		for(int j = 0; j < scene->verTamY(); j++) {
			Posicion pos = Posicion(i, j);
			bool vacia = scene->casillaEstaVacia(&pos);
			if (vacia)
				cout << "0";
			else
				cout << "1";
		} 
		cout << endl;
	}
	cout << endl;
}


// Testea la creación de un escenario de 50x50
// Posiciona dos edificios
// Coloca un aldeano y lo hace caminar hasta una posición
// y en medio del camino le asigna otro nuevo destino

// Imprime el mapa y como cambia la posición del aldeano a medida que avanzan los frames
void testEscenarioBasico() {
	Escenario* scene = new Escenario();

	Entidad* ent1 = new CentroUrbano();
	Entidad* ent2 = new Casa();
	Unidad* unit = new Aldeano();
	cout << "Se crea un mapa de 50x50" << endl;
	cout << "Se coloca una casa (2x2) en la posicion 0,15" << endl;
	cout << "Se coloca un centro urbano (4x4) en la posicion 15,0" << endl;
	cout << "Se imprime el mapa (0=posicion vacia, 1=posicion ocupada)" << endl;
	Posicion pos1 = Posicion(0, 15);
	Posicion pos2 = Posicion(15, 0);

	scene->asignarProtagonista(unit, &pos2);
	scene->asignarDestinoProtagonista(&pos1);
	scene->ubicarEntidad(ent1, &pos1);
	scene->ubicarEntidad(ent2, &pos2);

	imprimirEscenario(scene);

	cout << endl << "Se posiciona un aldeano en 15,0; y se le designa como destino la posicion 0,15" << endl;
	cout << "Cada linea representa la posicion del aldeano en cada frame que pasa" << endl;
	cout << "Se simulan solo 50 frames" << endl << endl;
	for (int i = 0; i < 50; i++) {
		scene->avanzarFrame();
		if (i == 20) {
			cout << "El se le impone una nueva posicion de desitno, 40,40" << endl;
			Posicion pos3 = Posicion(40, 40);
			scene->asignarDestinoProtagonista(&pos3);
		}
		cout << unit->verPosicion()->getX() << "-" << unit->verPosicion()->getY() << endl;
	}

	delete scene;
}