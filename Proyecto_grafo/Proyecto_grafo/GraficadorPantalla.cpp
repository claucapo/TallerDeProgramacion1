#include "GraficadorPantalla.h"
#include "Spritesheet.h"
#include "Escenario.h"
#include "ErrorLog.h"
#include <SDL.h>
#include <SDL_image.h>
#include "ConversorUnidades.h"
#include "BibliotecaDeImagenes.h"
#include <list>
#include <iostream>
#include <conio.h>

using namespace std;

GraficadorPantalla::GraficadorPantalla(void): pantalla(NULL) {}

GraficadorPantalla::~GraficadorPantalla(void){}

#define VIEW_X_DEFAULT (ancho_borde - screen_width)/2
#define VIEW_Y_DEFAULT 0
#define VEL_ZERO_DEFAULT 19
#define MARGEN_SCROLL_DEFAULT 66
GraficadorPantalla::GraficadorPantalla(int pant_width, int pant_height, bool full_screen, string title) {
	this->escenario = nullptr;
	this->screen_height = pant_height;
	this->screen_width = pant_width;
	this->vel_scroll = VEL_ZERO_DEFAULT;
	this->margen_scroll = MARGEN_SCROLL_DEFAULT;
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	if(!cargarSDL(full_screen, title))
		delete this;
	
}

void GraficadorPantalla::asignarEscenario(Escenario* scene) {
	if (scene) {
		ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
		this->escenario = scene;
		this->alto_borde = cu->convertULToPixels(escenario->verTamX());
		this->ancho_borde = 1.732 * cu->convertULToPixels(escenario->verTamY());

		this->view_x = VIEW_X_DEFAULT;
		this->view_y = VIEW_Y_DEFAULT;
	}
}

void GraficadorPantalla::dibujarPantalla(void) {
	// PASOS DEL DIBUJO DE LA PANTALLA
	if (!this->escenario) {
		ErrorLog::getInstance()->escribirLog("FATAL: No hay Escenario que graficar!" , LOG_ERROR);
		return;
		}
	// 0) Limpiar pantalla
	SDL_FillRect(pantalla, NULL, 0x000000);
//	SDL_PumpEvents(); // Actualiza los eventos de SDL

	// 1) Scroll del mapa (cambiar view_x y view_y)
	reajustarCamara();

	// 2) Dibujar el Escenario (HARDCODEADO DE MOMENTO)
	renderizarTerreno();

	
	// 3) Dibujar los edificios
	renderizarEntidades();

	// 4) Dibujar al protagonista (proximamente las unidades)
	renderizarProtagonista();

	// 5) Dibujar el screen frame alrededor de la pantalla
	dibujarMarcoPantalla(NULL, NULL, NULL, NULL);
				
	// 6) Window_Update
	SDL_UpdateWindowSurface(ventana);

}


// PASO 1: reajustar la c�mara de acuerdo a la posici�n del mouse

#define K_SCREEN 0.2	// Constante m�gica???? ------> Explicar... ya se... es la pendiente de la recta?;
void GraficadorPantalla::reajustarCamara(void) {

/*	int mx = 5 , my = 9;
	SDL_GetMouseState(&mx, &my);


	if(screen_width < ancho_borde){
	if(mx < MARGEN)
		view_x += -VEL_ZERO + mx * K_SCREEN;
	else if(mx > screen_width - MARGEN)
		view_x += VEL_ZERO*(1 - screen_width/MARGEN) + VEL_ZERO*mx/(MARGEN);

	if(view_x < -MARGEN)
		view_x = -MARGEN;
	else if((view_x + screen_width) > (ancho_borde + MARGEN))
		view_x = ancho_borde + MARGEN - screen_width;
	}

	if(screen_height < alto_borde){
	if(my < MARGEN)
		view_y += -VEL_ZERO + my * K_SCREEN;
	else if(my> screen_height - MARGEN)
		view_y += VEL_ZERO*(1 - screen_height/MARGEN) + VEL_ZERO*my/(MARGEN);

	
	if(view_y < -MARGEN)
		view_y = -MARGEN;
	else if((view_y + screen_height) > (alto_borde + MARGEN))
		view_y = alto_borde + MARGEN - screen_height;
	}
*/
	float new_view_x = view_x, new_view_y = view_y;

	int mx = 5 , my = 9;
	SDL_GetMouseState(&mx, &my);

	if(mx < margen_scroll)
		new_view_x += -vel_scroll + mx * K_SCREEN;
	else if(mx > screen_width - margen_scroll)
		new_view_x += vel_scroll*(1 - screen_width/margen_scroll) + vel_scroll*mx/(margen_scroll);


	if(my < margen_scroll)
		new_view_y += -vel_scroll + my * K_SCREEN;
	else if(my> screen_height - margen_scroll)
		new_view_y += vel_scroll*(1 - screen_height/margen_scroll) + vel_scroll*my/(margen_scroll);

	// Obtengo las coordenadas del punto central de la pantalla
	float x_centPant = screen_width / 2;
	float y_centPant = screen_height / 2;
	
	ConversorUnidades* cu =  ConversorUnidades::obtenerInstancia();
	float x_central = cu->obtenerCoordLogicaX(x_centPant, y_centPant, new_view_x, new_view_y, ancho_borde);
	float y_central = cu->obtenerCoordLogicaY(x_centPant, y_centPant, new_view_x, new_view_y, ancho_borde);
	
	// Si la posicion central no tiene una casilla en pantalla, return
	Posicion pos_central = Posicion(x_central, y_central);
	if(!escenario->verMapa()->posicionPertenece(&pos_central))
		return;

	view_x = new_view_x;
	view_y = new_view_y;
}


// PASO 2: renderizar terreno

// TODO: Optimizaciones
void GraficadorPantalla::renderizarTerreno(void) {
	SDL_Surface* imgTile = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tileHuge");
	SDL_SetColorKey(imgTile, true, SDL_MapRGB(imgTile->format, 255, 255, 255));
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	SDL_Rect rectangulo;
	int i = 0, j = 0;
	while((escenario->verTamY() - j) >= 10){
		i = 0;
		while((escenario->verTamX() - i) >= 10){
			rectangulo.x = cu->obtenerCoordPantallaX(i, j, view_x, view_y, ancho_borde) -260;
			rectangulo.y = cu->obtenerCoordPantallaY(i, j, view_x, view_y, ancho_borde);	
			SDL_BlitSurface( imgTile, NULL, pantalla, &rectangulo );
			i += 10;
			}
		j += 10;
		}

	imgTile = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tile");
	SDL_SetColorKey(imgTile, true, SDL_MapRGB(imgTile->format, 255, 255, 255));

	for(int k = i; k < escenario->verTamX(); k++) {
		for(int m = 0; m < escenario->verTamY(); m++){
			rectangulo.x = cu->obtenerCoordPantallaX(k, m, view_x, view_y, ancho_borde) -26;
			// Esto chequea si la casilla cae dentro del la pantalla... si no resulta visible, no la grafica
			if ((rectangulo.x) < (this->screen_width) && (rectangulo.x + imgTile->w) > 0) {
				rectangulo.y = cu->obtenerCoordPantallaY(k, m, view_x, view_y, ancho_borde);
				if ((rectangulo.y) < (this->screen_height) && (rectangulo.y + imgTile->h) > 0) 
					SDL_BlitSurface( imgTile, NULL, pantalla, &rectangulo );
				}
			}
		}

	for(int k = 0; k < i; k++) {
		for(int m = j; m < escenario->verTamY(); m++){
			rectangulo.x = cu->obtenerCoordPantallaX(k, m, view_x, view_y, ancho_borde) -26;
			// Esto chequea si la casilla cae dentro del la pantalla: si no resulta visible, no la grafica
			if ((rectangulo.x) < (this->screen_width) && (rectangulo.x + imgTile->w) > 0) {
				rectangulo.y = cu->obtenerCoordPantallaY(k, m, view_x, view_y, ancho_borde);
				if ((rectangulo.y) < (this->screen_height) && (rectangulo.y + imgTile->h) > 0) 
					SDL_BlitSurface( imgTile, NULL, pantalla, &rectangulo );
				}
			}
		}
	
	/*SDL_Surface* imgTile = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tile");
	SDL_SetColorKey(imgTile, true, SDL_MapRGB(imgTile->format, 255, 255, 255));
	SDL_Rect rectangulo;
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	


	for(int i = 0; i < escenario->verTamX(); i++) {
		for(int j = 0; j < escenario->verTamY(); j++){
			rectangulo.x = cu->obtenerCoordPantallaX(i, j, view_x, view_y, ancho_borde);
			// Esto chequea si la casilla cae dentro del la pantalla... si no resulta visible, no la grafica
			if ((rectangulo.x) < (this->screen_width) && (rectangulo.x + imgTile->w) > 0) {
				rectangulo.y = cu->obtenerCoordPantallaY(i, j, view_x, view_y, ancho_borde);
				if ((rectangulo.y) < (this->screen_height) && (rectangulo.y + imgTile->h) > 0) {
					SDL_BlitSurface( imgTile, NULL, pantalla, &rectangulo );
				}
			}
		}
	}*/
}


// PASO 3: renderizar protagonista

void GraficadorPantalla::renderizarProtagonista(void) {
	SDL_Rect recOr, rectangulo;
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();

	float oldX = escenario->verProtagonista()->verPosicion()->getX();
	float oldY = escenario->verProtagonista()->verPosicion()->getY();
	Spritesheet* unidad = escenario->verProtagonista()->verSpritesheet();

	int newX = (int) cu->obtenerCoordPantallaX(oldX, oldY, view_x, view_y, ancho_borde);
	int newY = (int) cu->obtenerCoordPantallaY(oldX, oldY, view_x, view_y, ancho_borde);
	unidad->cambirCoord(newX, newY);

	SDL_Surface* spUnidad = unidad->devolverImagenAsociada();
	SDL_SetColorKey( spUnidad, true, SDL_MapRGB(spUnidad->format, 255, 0, 255) );

	recOr.x = unidad->calcularOffsetX();
	recOr.y = unidad->calcularOffsetY();
	recOr.w = unidad->subImagenWidth();
	recOr.h = unidad->subImagenHeight();
	rectangulo.x = unidad->getCoordX();
	rectangulo.y = unidad->getCoordY();
	
	SDL_BlitSurface( spUnidad, &recOr, pantalla, &rectangulo );
}



// PASO 4: renderizar entidades

// TODO: Optimizar para que no se grafiquen edificios que no aparezcan en pantalla (ver renderizarTerreno)

void GraficadorPantalla::renderizarEntidades(void) {
/*	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	SDL_Rect rectangulo;
	list<Entidad*> lEnt = escenario->verEntidades();
	for (list<Entidad*>::iterator it=lEnt.begin(); it != lEnt.end(); ++it){
		Spritesheet* entAct = (*it)->verSpritesheet();
	
			// Esto chequea si la casilla cae dentro del la pantalla: si no resulta visible,no la grafica
		int newX = (int) cu->obtenerCoordPantallaX((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
		if ((newX-entAct->subImagenWidth()) < (this->screen_width) && (newX + entAct->subImagenWidth()) > 0) {
			int newY = (int) cu->obtenerCoordPantallaY((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
			if (newY < (this->screen_height) && (newY + entAct->subImagenHeight()) > 0){		
				entAct->cambirCoord(newX, newY);
				rectangulo.x = entAct->getCoordX();
				rectangulo.y = entAct->getCoordY();
	
				SDL_Rect recOr;
				recOr.x = entAct->calcularOffsetX();
				recOr.y = entAct->calcularOffsetY();
				recOr.w = entAct->subImagenWidth();
				recOr.h = entAct->subImagenHeight();

				
				SDL_Surface* spEnt = entAct->devolverImagenAsociada();
				SDL_SetColorKey( spEnt, true, SDL_MapRGB(spEnt->format, 255, 0, 255) );
				SDL_BlitSurface( spEnt, &recOr, pantalla, &rectangulo );
				}
			}
		}
		*/


	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	SDL_Rect rectangulo;
	list<Entidad*> lEnt = escenario->verEntidades();
	for (list<Entidad*>::iterator it=lEnt.begin(); it != lEnt.end(); ++it){
		Spritesheet* entAct = (*it)->verSpritesheet();
		SDL_Surface* spEnt = entAct->devolverImagenAsociada();
		SDL_SetColorKey( spEnt, true, SDL_MapRGB(spEnt->format, 255, 0, 255) );
	
		int newX = (int) cu->obtenerCoordPantallaX((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
		int newY = (int) cu->obtenerCoordPantallaY((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
		entAct->cambirCoord(newX, newY);
		rectangulo.x = entAct->getCoordX();
		rectangulo.y = entAct->getCoordY();

		SDL_Rect recOr;
		recOr.x = entAct->calcularOffsetX();
		recOr.y = entAct->calcularOffsetY();
		recOr.w = entAct->subImagenWidth();
		recOr.h = entAct->subImagenHeight();

		SDL_BlitSurface( spEnt, &recOr, pantalla, &rectangulo );
	}
}


//	PASO 5:

#define POS_REL_MINIMAP_X 0.805
#define POS_REL_MINIMAP_Y 0.775
#define	HEIGHT_REL_MINIMAP 0.225
#define	WIDTH_REL_MINIMAP 0.331
void GraficadorPantalla::dibujarMarcoPantalla(int* minimapX, int* minimapY, int* minimapW, int* minimapH){
	DatosImagen* marcoData = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen("screen_frame");
	if(marcoData->path == "default.png")
		return;
	SDL_Surface* borde = marcoData->imagen;
	SDL_SetColorKey(borde, true, SDL_MapRGB(borde->format, 255, 0, 255));
	SDL_Rect rectangulo;
	rectangulo.x = 0;
	rectangulo.y = 0;
	rectangulo.h = screen_height;
	rectangulo.w = screen_width;
	SDL_BlitScaled( borde, NULL, pantalla, &rectangulo );
	if(minimapX)
		*minimapX = rectangulo.w * POS_REL_MINIMAP_X;
	if(minimapY)
		*minimapY = rectangulo.h * POS_REL_MINIMAP_Y;
	if(minimapH)
		*minimapH = rectangulo.h * HEIGHT_REL_MINIMAP;
	if(minimapW)
		*minimapW = rectangulo.w * WIDTH_REL_MINIMAP;

}

// METODOS DE INICIALIZACION Y GETTERS

bool GraficadorPantalla::cargarSDL(bool full_screen, string title) {
	Uint32 flags = SDL_WINDOW_SHOWN;
	if (full_screen) 
		flags = flags || SDL_WINDOW_FULLSCREEN;

	if(SDL_Init( SDL_INIT_VIDEO ) < 0)	{
		ErrorLog::getInstance()->escribirLog("CR�TICO: SDL no pudo inicializarse." , LOG_ERROR);
		printf( "SDL Error: %s\n", SDL_GetError() );
		return false;
	} else {
		if (title.length() == 0) {
			title = "AGE OF TALLER DE PROGRAMACION I";
		}
		ventana = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, flags);
		if(!ventana) {
			ErrorLog::getInstance()->escribirLog("Ventana no pudo crearse" , LOG_ERROR);
			return false;
		} else {
			int flags = IMG_INIT_PNG;
			if(!(IMG_Init(flags) & flags)) {
				ErrorLog::getInstance()->escribirLog("CR�TICO: SDL no pudo inicializarse." , LOG_ERROR);
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				return false;
			} else {
				pantalla = SDL_GetWindowSurface(ventana);
			}
		}
	}
	return true;
}


void GraficadorPantalla::asignarParametrosScroll(int margen, int velocidad) {
	if (velocidad > 0)
		this->vel_scroll = velocidad;
	else
		ErrorLog::getInstance()->escribirLog("Se intento asignar una velocidad de scroll inv�lida." , LOG_WARNING);

	if (margen <= 0) {
		ErrorLog::getInstance()->escribirLog("Se intento asignar un margen de scroll nulo." , LOG_WARNING);	
	} else if(margen > (this->screen_height/3) || margen > (this->screen_width/3) ) {
		ErrorLog::getInstance()->escribirLog("Se intento asignar un margen de scroll demasiado grande." , LOG_WARNING);	
	} else {
		this->margen_scroll = margen;
	}
}

SDL_Window* GraficadorPantalla::getVentana(void) {
	return this->ventana;
}

SDL_Surface* GraficadorPantalla::getPantalla(void) {
	return pantalla;
}

float GraficadorPantalla::getViewX(void){
	return this->view_x;
}

float GraficadorPantalla::getViewY(void){
	return this->view_y;
}

float GraficadorPantalla::getAnchoBorde(void) {
	return this->ancho_borde;
}

