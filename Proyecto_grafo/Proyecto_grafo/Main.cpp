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
Escenario* cargarEscenario(){
	Escenario* scene = new Escenario();
		
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

	Posicion pos1 = Posicion(24, 17);
	Posicion pos2 = Posicion(20, 24);
	Posicion posW = Posicion(26, 25);
	Posicion posP = Posicion(26, 25);

	scene->asignarProtagonista(unit, &posP);
	scene->ubicarEntidad(ent1, &pos1);
	scene->ubicarEntidad(ent2, &pos2);
	scene->ubicarEntidad(ent3, &posW);

	return scene;
}

int procesarEventos(Escenario* scene, GraficadorPantalla* gp){
	int retorno = CODE_CONTINUE;
	SDL_Event evento;
	while(SDL_PollEvent(&evento)){
		if((evento.type == SDL_QUIT))
			retorno = CODE_EXIT;
		else if(evento.type == SDL_KEYDOWN){
			if(evento.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				retorno = CODE_EXIT;
			else if(evento.key.keysym.scancode == SDL_SCANCODE_R)
				retorno = CODE_RESET;	
			else{
				}
			}
			else if(evento.type == SDL_MOUSEBUTTONDOWN ){
				int mx = 5 , my = 9;	
				if( evento.button.button == SDL_BUTTON_RIGHT){
					SDL_GetMouseState(&mx, &my);
					ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
					float pX = cu->obtenerCoordLogicaX(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
					float pY = cu->obtenerCoordLogicaY(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
					Posicion destino = Posicion(pX, pY);
					
					scene->asignarDestinoProtagonista(&destino);
				}
				else {
					}
				}
		else{
			
			}
		}
	
	return retorno;
}




/*******************************************

			PROGRAMA PRINCIPAL

********************************************/

//  Se puede definir como main... cambiando el subsistema del proyecto a Windows
int wmain(int argc, char** argv) {
	inicio:
	// CARGAR EL ESCENARIO ESTA RE HARDCODEADO AHORA
	// LA IDEA ES QUE USE LO DEL YAML
	Escenario* scene = cargarEscenario();
	GraficadorPantalla* gp = new GraficadorPantalla(scene, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_Window* gameWindow = gp->getVentana();
	SDL_Surface* gameScreen = gp->getPantalla();
	BibliotecaDeImagenes::obtenerInstancia()->asignarPantalla(gameScreen);
	int i = 0;
	// Main loop del juego
	int codigo_programa = CODE_CONTINUE;

//	gp->dibujarPantalla();
	while(codigo_programa > 0){
		float timeA = SDL_GetTicks();

		codigo_programa = procesarEventos(scene, gp);
	//	cout << "Prog code: " << codigo_programa << endl;
	//	cout << " i: " << i << endl;
		scene->avanzarFrame();
		gp->dibujarPantalla();

		float timeB = SDL_GetTicks();
	//	cout<< "D:" << FRAME_DURATION -timeB + timeA<< endl;
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

	if(codigo_programa == CODE_RESET)
		goto inicio;

	return 0;
}
