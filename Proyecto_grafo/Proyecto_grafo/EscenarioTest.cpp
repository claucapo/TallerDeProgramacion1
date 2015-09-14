#include "Escenario.h"
#include "Entidad.h"
#include "Posicion.h"
#include "Edificios.h"
#include "Protagonistas.h"
#include <iostream>
#include "GraficadorPantalla.h"
#include <conio.h>

using namespace std;

void imprimirEscenario(GraficadorPantalla* gp) {
	/*
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
	*/


}


// Testea la creación de un escenario de 50x50
// Posiciona dos edificios
// Coloca un aldeano y lo hace caminar hasta una posición
// y en medio del camino le asigna otro nuevo destino

// Imprime el mapa y como cambia la posición del aldeano a medida que avanzan los frames
void testEscenarioBasico() {
	Escenario* scene = new Escenario();
	GraficadorPantalla gp(scene, 640, 480);


	Entidad* ent1 = new CentroUrbano();
	Entidad* ent2 = new Casa();
	Entidad* ent3 = new CentroUrbano();
	Unidad* unit = new Aldeano();
	
	Spritesheet* ald = new Spritesheet("champion.png", 8, 10, 0, 0);
	unit->asignarSprite(ald);
	ald->setAnimationDelay(50);
	Spritesheet* cas = new Spritesheet("house.png", 1, 1, 0, 0);
	ent2->asignarSprite(cas);
	Spritesheet* curb = new Spritesheet("urbanCenter.png", 1, 1, 0, 0);
	ent1->asignarSprite(curb);
	Spritesheet* swf = new Spritesheet("siegeWeaponFactory.png", 1, 1, 0, 0);
	ent3->asignarSprite(swf);

	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();

	Posicion pos1 = Posicion(24, 17);
	Posicion pos2 = Posicion(20, 24);
	Posicion posW = Posicion(26, 25);
	Posicion posP = Posicion(26, 25);
	Posicion posD = Posicion(22, 25);

	scene->asignarProtagonista(unit, &posP);
	scene->asignarDestinoProtagonista(&posD);
	scene->ubicarEntidad(ent1, &pos1);
	scene->ubicarEntidad(ent2, &pos2);
	scene->ubicarEntidad(ent3, &posW);
	gp.dibujarPantalla();

	for (int i = 0; i < 100; i++) {
		scene->avanzarFrame();
		if (i == 20) {
			cout << "El se le impone una nueva posicion de desitno, 40,40" << endl;
			Posicion pos3 = Posicion(21, 4);
			scene->asignarDestinoProtagonista(&pos3);
		}
		cout << unit->verPosicion()->getX() << "-" << unit->verPosicion()->getY() << endl;
		//
		gp.dibujarPantalla();
		SDL_Delay(50);
	}

	delete scene;
}