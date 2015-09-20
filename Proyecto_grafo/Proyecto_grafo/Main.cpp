#include "Escenario.h"
#include "Enumerados.h"
#include "Entidad.h"
#include "Unidad.h"
#include "ConversorUnidades.h"
#include "BibliotecaDeImagenes.h"
#include "Posicion.h"
#include "ErrorLog.h"
#include "Edificios.h"
#include "Protagonistas.h"
#include "GraficadorPantalla.h"
#include "ConfigParser.h"
#include "Factory.h"
#include "DatosImagen.h"
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


void cargarBibliotecaImagenes(std::list<entidadInfo_t*> eInfoL) {
	for (std::list<entidadInfo_t*>::const_iterator iter = eInfoL.begin(); iter != eInfoL.end(); ++iter) {
		entidadInfo_t act = *(*iter);
		DatosImagen* data = new DatosImagen();
		data->path = act.spritePath;
		data->casillasX = act.tamX;
		data->casillasY = act.tamY;
		data->origenX = act.pixel_align_X;
		data->origenY = act.pixel_align_Y;
		data->columnas = act.subX;
		data->filas = act.subY;
		data->fps = act.fps;
		data->delay = act.delay;
		BibliotecaDeImagenes::obtenerInstancia()->cargarDatosImagen(act.nombre, data);
	}
}

Escenario* cargarEscenario(escenarioInfo_t escenarioInfo){
	Escenario* scene = new Escenario(escenarioInfo.size_X,escenarioInfo.size_Y);

	for(list<instanciaInfo_t*>::const_iterator it = escenarioInfo.instancias.begin(); it != escenarioInfo.instancias.end(); ++it) {
		Entidad* entidad = Factory::obtenerEntidad((*it));
		Posicion posicion = Posicion((float)(*it)->x, (float)(*it)->y);
	    scene->ubicarEntidad(entidad, &posicion);
		Spritesheet* cas = new Spritesheet((*it)->tipo);
	    entidad->asignarSprite(cas);
	}

	Unidad* unit = Factory::obtenerUnidad(&escenarioInfo.protagonista);
	Spritesheet* sprite = new Spritesheet(escenarioInfo.protagonista.tipo);
	unit->asignarSprite(sprite);
	unit->setVelocidad(1);
	Posicion posP = Posicion(escenarioInfo.protagonista.x, escenarioInfo.protagonista.y);
	scene->asignarProtagonista(unit, &posP);
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

		// Parseo de YAML
	    ConfigParser parser = ConfigParser();
		parser.setPath("Default.yaml");
		parser.parsearTodo();
			
		// Inicializar pantalla y graficador
		GraficadorPantalla* gp = new GraficadorPantalla(parser.verInfoPantalla().screenW, parser.verInfoPantalla().screenH, parser.verInfoPantalla().fullscreen);

		SDL_Window* gameWindow = gp->getVentana();
		SDL_Surface* gameScreen = gp->getPantalla();
		BibliotecaDeImagenes::obtenerInstancia()->asignarPantalla(gameScreen);

		// Cargar imagenes del YAML
		cargarBibliotecaImagenes(parser.verInfoEntidades());

		// Crear y asignar escenario
		Escenario* scene = cargarEscenario(parser.verInfoEscenario());
		scene->verProtagonista()->setVelocidad(parser.verInfoGameplay().velocidad);
		gp->asignarEscenario(scene);
		
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
		BibliotecaDeImagenes::obtenerInstancia()->clear();

		SDL_DestroyWindow(gameWindow);
		gameWindow = NULL;
		IMG_Quit();
		SDL_Quit();
	}
	delete BibliotecaDeImagenes::obtenerInstancia();
	ErrorLog::getInstance()->cerrarLog();
	return 0;
}