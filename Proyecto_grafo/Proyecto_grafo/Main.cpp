#include "Escenario.h"
#include "Enumerados.h"
#include "Entidad.h"
#include "Unidad.h"
#include "ConversorUnidades.h"
#include "BibliotecaDeImagenes.h"
#include "Posicion.h"
#include "Edificios.h"
#include "Protagonistas.h"
#include "GraficadorPantalla.h"
#include "ConfigParser.h"
#include "Factory.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define CODE_CONTINUE 1
#define CODE_RESET -2
#define CODE_EXIT -1

/*******************************************
		FUNCIONES DEL BUCLE DEL JUEGO
********************************************/

// Ejemplo hardcodeado!
Escenario* cargarEscenario(escenarioInfo_t escenarioInfo){
	Escenario* scene = new Escenario(escenarioInfo.size_X,escenarioInfo.size_Y);

	for(list<instanciaInfo_t*>::const_iterator it = escenarioInfo.instancias.begin(); it != escenarioInfo.instancias.end(); ++it)
	{
		Entidad* entidad = Factory::obtenerEntidad((*it));
		Posicion posicion = Posicion((float)(*it)->x, (float)(*it)->y);
	    scene->ubicarEntidad(entidad, &posicion);
		Spritesheet* cas = new Spritesheet("houseAOE.png", 1, 1, 0, 0);
	    entidad->asignarSprite(cas);
	}

	/*Entidad* ent1 = new CentroUrbano();
	Entidad* ent2 = new Casa();
	Entidad* ent3 = new CentroUrbano();*/
	Unidad* unit = new Aldeano();
	
	Spritesheet* ald = new Spritesheet("championAOE.png", 8, 10, 0, 0);
	unit->asignarSprite(ald);
	ald->setAnimationDelay(40);
	Posicion posP = Posicion(26, 25);
	scene->asignarProtagonista(unit, &posP);
	/*Spritesheet* cas = new Spritesheet("house.png", 1, 1, 0, 0);
	ent2->asignarSprite(cas);
	Spritesheet* curb = new Spritesheet("urbanCenter.png", 1, 1, 0, 0);
	ent1->asignarSprite(curb);
	Spritesheet* swf = new Spritesheet("siegeWeaponFactory.png", 1, 1, 0, 0);
	ent3->asignarSprite(swf)*/;

	/*Posicion pos1 = Posicion(24, 17);
	Posicion pos2 = Posicion(20, 24);
	Posicion posW = Posicion(26, 25);
	Posicion posP = Posicion(26, 25);
	scene->asignarProtagonista(unit, &posP);
	scene->ubicarEntidad(ent1, &pos1);
	scene->ubicarEntidad(ent2, &pos2);
	scene->ubicarEntidad(ent3, &posW);
*/
	return scene;
}

// Define distintas acciones para cada tecla
int procesarEventoKeyDown(Escenario* scene, GraficadorPantalla* gp, SDL_Event e) {
	switch (e.key.keysym.scancode) {
	case SDL_SCANCODE_ESCAPE:
		return CODE_EXIT;
	case SDL_SCANCODE_R:
		return CODE_RESET;
	}
}

// Define distintas acciones para los botones del mouse
int procesarEventoMouse(Escenario* scene, GraficadorPantalla* gp, SDL_Event e) {
	int mx, my;	
	if( e.button.button == SDL_BUTTON_RIGHT){
		SDL_GetMouseState(&mx, &my);

		ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
		float pX = cu->obtenerCoordLogicaX(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
		float pY = cu->obtenerCoordLogicaY(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
		Posicion destino = Posicion(pX, pY);
					
		scene->asignarDestinoProtagonista(&destino);
	}
	return CODE_CONTINUE;
}

int procesarEventos(Escenario* scene, GraficadorPantalla* gp){
	int retorno = CODE_CONTINUE;
	SDL_Event evento;
	while(SDL_PollEvent(&evento) != 0){
		switch (evento.type) {
		case SDL_QUIT:
			retorno = CODE_EXIT; break;

		case SDL_KEYDOWN:
			retorno = procesarEventoKeyDown(scene, gp, evento); break;

		case SDL_MOUSEBUTTONDOWN:
			retorno = procesarEventoMouse(scene, gp, evento); break;
		}
	}
	return retorno;
}




/*******************************************
			PROGRAMA PRINCIPAL
********************************************/

//  Se puede definir como main... cambiando el subsistema del proyecto a Windows
int wmain(int argc, char** argv) {
	int codigo_programa = CODE_CONTINUE;
	while (codigo_programa != CODE_EXIT) {
		codigo_programa = CODE_CONTINUE;
		// Primero se crea el parser
	     ConfigParser parser = ConfigParser();

		// [Opcional] Se le asigna una ruta distinta ("default.yaml" por defecto)
		parser.setPath("Default.yaml");

		// Se parsea el archivo
		parser.parsearTodo();

		Escenario* scene = cargarEscenario(parser.verInfoEscenario());
		GraficadorPantalla* gp = new GraficadorPantalla(scene, parser.verInfoPantalla().screenW, parser.verInfoPantalla().screenH);
		SDL_Window* gameWindow = gp->getVentana();
		SDL_Surface* gameScreen = gp->getPantalla();
		BibliotecaDeImagenes::obtenerInstancia()->asignarPantalla(gameScreen);
		 int i = 1; float dTot = 0;
		// Main loop del juego
		while(codigo_programa > 0){
			float timeA = SDL_GetTicks();

			codigo_programa = procesarEventos(scene, gp);
		//	cout << "Prog code: " << codigo_programa << endl;
		//	cout << " i: " << i << endl;
			scene->avanzarFrame();
			gp->dibujarPantalla();

			float timeB = SDL_GetTicks();
			
			dTot += (timeB - timeA);
			cout<< "Duracion Prom.:" << dTot/i << endl;
			
			if((FRAME_DURATION -timeB + timeA) > 0)
				SDL_Delay(FRAME_DURATION -timeB + timeA);

			 i++;
		}

		delete gp;
		delete scene;
	
		SDL_DestroyWindow(gameWindow);
		gameWindow = NULL;
		IMG_Quit();
		SDL_Quit();
	}

	return 0;
}