#include "Escenario.h"
#include "Enumerados.h"
#include "Entidad.h"
#include "Unidad.h"
#include "ConversorUnidades.h"
#include "BibliotecaDeImagenes.h"
#include "Posicion.h"
#include "ErrorLog.h"
#include "GraficadorPantalla.h"
#include "ConfigParser.h"
#include "FactoryEntidades.h"
#include "DatosImagen.h"
#include "Jugador.h"
#include <iostream>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>

#include "Tests.h"

using namespace std;

#define TESTING_ENABLED false

#define ARCHIVO_YAML "test0.yaml"

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
		FactoryEntidades::obtenerInstancia()->agregarEntidad(act);
	}
}

Escenario* cargarEscenario(escenarioInfo_t escenarioInfo){
	Escenario* scene = new Escenario(escenarioInfo.size_X,escenarioInfo.size_Y);

	for(list<instanciaInfo_t*>::const_iterator it = escenarioInfo.instancias.begin(); it != escenarioInfo.instancias.end(); ++it) {
		Entidad* entidad = FactoryEntidades::obtenerInstancia()->obtenerEntidad((*it)->tipo);
		if (entidad) {
			Posicion posicion = Posicion((float)(*it)->x, (float)(*it)->y);
			if ( scene->ubicarEntidad(entidad, &posicion) ) {
				Spritesheet* cas = new Spritesheet(entidad->verTipo());
				entidad->asignarSprite(cas);
			} else {
				delete entidad;
			}
		}

		// Print de la lista de entidades ordenadas
		/*
		list<Entidad*> lEnt = scene->verEntidades();
		for(list<Entidad*>::iterator it = lEnt.begin(); it != lEnt.end(); ++it) {
			cout << (*it)->verPosicion()->getRoundX() << " - " << (*it)->verPosicion()->getRoundY() << " - " << (*it)->name << endl;
		}
		cout << endl;
		*/
	}

	Unidad* unit = FactoryEntidades::obtenerInstancia()->obtenerUnidad(escenarioInfo.protagonista.tipo);
	Posicion posP = Posicion(escenarioInfo.protagonista.x, escenarioInfo.protagonista.y);
	if (!scene->asignarProtagonista(unit, &posP)) {
		scene->asignarProtagonista(unit, &Posicion(0,0));
	}
	Spritesheet* sprite = new Spritesheet(unit->verTipo());
	unit->asignarSprite(sprite);
	unit->setVelocidad(1);
	return scene;
}



// Define distintas acciones para cada tecla
int procesarEventoKeyDown(Escenario* scene, GraficadorPantalla* gp, SDL_Event e) {
	switch (e.key.keysym.scancode) {
	case SDL_SCANCODE_ESCAPE:
		return CODE_EXIT;
	case SDL_SCANCODE_R:
		return CODE_RESET;
	//HARDCODEO
	case SDL_SCANCODE_S:
		return 117;
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

	// Testing por consola
	testVision();
	// --------------------

	int test_number = 0; // ONLY FOR TESTING
	int codigo_programa = CODE_CONTINUE;
	while (codigo_programa != CODE_EXIT) {

		ErrorLog::getInstance()->escribirLog("----INICIANDO----");

		codigo_programa = CODE_CONTINUE;
		
		// Parseo de YAML
	    ConfigParser parser = ConfigParser();
		parser.setPath(ARCHIVO_YAML);

		// TESTING MAP SEQUENCE
		if (TESTING_ENABLED) {
			stringstream ss;
			ss << "test" << test_number << ".yaml";
			parser.setPath(ss.str());
			test_number++;
		}

		parser.parsearTodo();
	
		// Inicializar pantalla y graficador
		pantallaInfo_t pInfo = parser.verInfoPantalla();
		GraficadorPantalla* gp = new GraficadorPantalla(pInfo.screenW, pInfo.screenH, pInfo.fullscreen, parser.verInfoEscenario().name);

		SDL_Window* gameWindow = gp->getVentana();
		SDL_Surface* gameScreen = gp->getPantalla();
		gp->asignarParametrosScroll(parser.verInfoGameplay().scroll_margen, parser.verInfoGameplay().scroll_vel);		
		BibliotecaDeImagenes::obtenerInstancia()->asignarPantalla(gameScreen);

		// Cargar imagenes del YAML
		cargarBibliotecaImagenes(parser.verInfoEntidades());

		// Crear y asignar escenario
		Escenario* scene = cargarEscenario(parser.verInfoEscenario());
		scene->verProtagonista()->setVelocidad(parser.verInfoGameplay().velocidad);
		gp->asignarEscenario(scene);
		
		// Por ahora le asigno un jugador sólo al protagonista
		// ya hago que sea gaia
		Jugador* player = new Jugador("gaia");
		scene->verProtagonista()->asignarJugador(player);
		gp->asignarJugador(player);

		player->asignarVision(scene->verTamX(), scene->verTamY());

			// Hardcodeo para agregar otro player!!!!
			Jugador* playerHard = new Jugador("hardcodeado");
			Posicion q = Posicion(1, 1);
			Unidad* uniHard = FactoryEntidades::obtenerInstancia()->obtenerUnidad("champion");
			uniHard->asignarJugador(playerHard);
			scene->ubicarEntidad(uniHard, &q);
			playerHard->asignarVision(scene->verTamX(), scene->verTamY());
			Spritesheet* sprHard = new Spritesheet(uniHard->verTipo());
			uniHard->asignarSprite(sprHard);
			uniHard->setVelocidad(2);
			uniHard->asignarPos(&q);
			int currentPlayer = 1;

		 int i = 1; float dTot = 0;
		// Main loop del juego
		while(codigo_programa > 0){
			float timeA = SDL_GetTicks();
			if(currentPlayer == 1){
				// Con esto actualizo la vision, ponele
				player->reiniciarVision();
				player->agregarPosiciones(scene->verMapa()->posicionesVistas(scene->verProtagonista()));
			}
			else{
				// Vision hardcodeada
				playerHard->reiniciarVision();
				playerHard->agregarPosiciones(scene->verMapa()->posicionesVistas(scene->verProtagonista()));
			}
			codigo_programa = procesarEventos(scene, gp);

				// Cambio de personajes hardcodeado
				if(codigo_programa == 117){
					/*Jugador* aux = playerHard;
					playerHard = player;
					player = aux;
					*/
					q = *uniHard->verPosicion();
					Unidad* uAux = scene->verProtagonista();
					
					scene->asignarProtagonista(uniHard, &q);
					uniHard = uAux;
					
					if(currentPlayer == 1){
						gp->asignarJugador(playerHard);
						currentPlayer = 2;
						}
					else {
						gp->asignarJugador(player);
						currentPlayer = 1;
						}
						
					codigo_programa = CODE_CONTINUE;
					}

			//	cout << "Prog code: " << codigo_programa << endl;
		//	cout << " i: " << i << endl;
			scene->avanzarFrame();
			gp->dibujarPantalla();

			float timeB = SDL_GetTicks();
			
			dTot += (timeB - timeA);
			// cout<< "Duracion Prom.:" << dTot/i << endl;
			cout << "Time:" << timeB-timeA << endl;
			if((FRAME_DURATION -timeB + timeA) > 0)
				SDL_Delay(FRAME_DURATION -timeB + timeA);

			 i++;
		}

		if (codigo_programa == CODE_RESET) {
			ErrorLog::getInstance()->escribirLog("----RESETEANDO----");
		}
		delete player;
		delete gp;
		delete scene;
		BibliotecaDeImagenes::obtenerInstancia()->clear();
		FactoryEntidades::obtenerInstancia()->limpar();

		SDL_DestroyWindow(gameWindow);
		gameWindow = NULL;
		IMG_Quit();
		SDL_Quit();
	}
	delete FactoryEntidades::obtenerInstancia();
	delete BibliotecaDeImagenes::obtenerInstancia();
	ErrorLog::getInstance()->cerrarLog();
	return 0;
}