#include "GraficadorPantalla.h"
#include "Spritesheet.h"
#include "Escenario.h"
#include "Partida.h"
#include "ErrorLog.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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
	this->partida = nullptr;
	this->screen_height = pant_height;
	this->screen_width = pant_width;
	this->vel_scroll = VEL_ZERO_DEFAULT;
	this->margen_scroll = MARGEN_SCROLL_DEFAULT;
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	if(!cargarSDL(full_screen, title))
		delete this;
}

void GraficadorPantalla::asignarPartida(Partida* partida) {
	if (partida) {
		ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
		this->partida = partida;
		this->alto_borde = cu->convertULToPixels(partida->escenario->verTamX());
		this->ancho_borde = 1.732 * cu->convertULToPixels(partida->escenario->verTamY());

		this->view_x = VIEW_X_DEFAULT;
		this->view_y = VIEW_Y_DEFAULT;
	}
}


void GraficadorPantalla::dibujarPantalla(void) {
	// PASOS DEL DIBUJO DE LA PANTALLA
	if (!this->partida->escenario) {
		ErrorLog::getInstance()->escribirLog("FATAL: No hay Escenario que graficar!" , LOG_ERROR);
		return;
		}
	if (!this->partida->obtenerJugador(0)) {
		ErrorLog::getInstance()->escribirLog("FATAL: No hay Jugador al que graficar!" , LOG_ERROR);
		return;
		}
	// 0) Limpiar pantalla
	SDL_FillRect(pantalla, NULL, 0x000000);

	// 1) Scroll del mapa (cambiar view_x y view_y)
	reajustarCamara();

	// 2) Dibujar el Escenario
	renderizarTerreno();

	// 3) Dibujar los edificios
	renderizarEntidades();
	
	// 5) Dibujar el screen frame alrededor de la pantalla
	int minimapX, minimapY, minimapW, minimapH;
	dibujarMarcoPantalla(&minimapX, &minimapY, &minimapW, &minimapH);

	// 6) Dibujar el minimapa
	dibujarMinimapa(minimapX, minimapY, minimapW, minimapH);
				
	// 7) Window_Update
	SDL_UpdateWindowSurface(ventana);
}


// PASO 1: reajustar la cámara de acuerdo a la posición del mouse
#define K_SCREEN 0.2	// Constante de scroll
void GraficadorPantalla::reajustarCamara(void) {
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
	if(!partida->escenario->verMapa()->posicionPertenece(&pos_central))
		return;

	view_x = new_view_x;
	view_y = new_view_y;
}


// PASO 2: renderizar terreno
#define TRANSPARENCIA_COLOR_MOD 160 
void GraficadorPantalla::renderizarTerreno(void) {
	SDL_Surface* imgTile = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tile");
	SDL_SetColorKey(imgTile, true, SDL_MapRGB(imgTile->format, 255, 255, 255));
	SDL_Rect rectangulo;
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	Vision* vis = partida->obtenerJugador(1)->verVision();

	for(int i = 0; i < partida->escenario->verTamX(); i++) {
		for(int j = 0; j < partida->escenario->verTamY(); j++){
			if(vis->visibilidadPosicion(Posicion(i,j)) != VIS_NO_EXPLORADA){
				rectangulo.x = cu->obtenerCoordPantallaX(i, j, view_x, view_y, ancho_borde);
				// Esto chequea si la casilla cae dentro del la pantalla... si no resulta visible, no la grafica
				if ((rectangulo.x) < (this->screen_width) && (rectangulo.x + imgTile->w) > 0) {
					rectangulo.y = cu->obtenerCoordPantallaY(i, j, view_x, view_y, ancho_borde);
					if ((rectangulo.y) < (this->screen_height) && (rectangulo.y + imgTile->h) > 0) {
						if(vis->visibilidadPosicion(Posicion(i,j)) == VIS_VISITADA)
							SDL_SetSurfaceColorMod(imgTile, TRANSPARENCIA_COLOR_MOD , TRANSPARENCIA_COLOR_MOD , TRANSPARENCIA_COLOR_MOD );
						else
							SDL_SetSurfaceColorMod(imgTile, 255, 255, 255 );
						SDL_BlitSurface( imgTile, NULL, pantalla, &rectangulo );
					}
				}
			}
		}
	}
}


// PASO 4: renderizar entidades
// TODO: Optimizar para que no se grafiquen edificios que no aparezcan en pantalla (ver renderizarTerreno)

void GraficadorPantalla::renderizarEntidades(void) {
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	SDL_Rect rectangulo;
	list<Entidad*> lEnt = partida->escenario->verEntidades();
	for (list<Entidad*>::iterator it=lEnt.begin(); it != lEnt.end(); ++it){
		Posicion* posEntAct1 = (*it)->verPosicion();
		int tx = (*it)->verTamX(); int ty = (*it)->verTamY();
		Posicion posEntAct2 = Posicion(posEntAct1->getX() + tx - 1, posEntAct1->getY());
		Posicion posEntAct3 = Posicion(posEntAct1->getX(), posEntAct1->getY() + ty - 1);
		Posicion posEntAct4 = Posicion(posEntAct1->getX() + tx - 1, posEntAct1->getY() + ty - 1); 
		Vision* vis = partida->obtenerJugador(1)->verVision();
		// Si se ve alguna de las cuatro posiciones del borde, dibujo la entidad
		bool entEsVisible= false;
		entEsVisible |= (vis->visibilidadPosicion(*posEntAct1)!=VIS_NO_EXPLORADA);
		entEsVisible |= (vis->visibilidadPosicion(posEntAct2)!=VIS_NO_EXPLORADA);
		entEsVisible |= (vis->visibilidadPosicion(posEntAct3)!=VIS_NO_EXPLORADA);
		entEsVisible |= (vis->visibilidadPosicion(posEntAct4)!=VIS_NO_EXPLORADA);
		
		if(entEsVisible){
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

			Jugador* playerOwner = (*it)->verJugador();
			if(playerOwner != nullptr){
			if(!playerOwner->estaConectado())
				SDL_SetSurfaceColorMod(spEnt, TRANSPARENCIA_COLOR_MOD , TRANSPARENCIA_COLOR_MOD , TRANSPARENCIA_COLOR_MOD );
			else
				SDL_SetSurfaceColorMod(spEnt, 255, 255, 255 );
			}
			SDL_BlitSurface( spEnt, &recOr, pantalla, &rectangulo );	
		}		
	}
}


//	PASO 5: screen frame
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
	SDL_Surface* texto = this->renderText(player->verNombre());
	rectangulo.x = 210;
	rectangulo.y = 400;
	rectangulo.h = 18;
	rectangulo.w = player->verNombre().length()* 14;
//	SDL_BlitSurface( texto, NULL, pantalla, &rectangulo );
	SDL_BlitScaled( texto, NULL, pantalla, &rectangulo );
	SDL_FreeSurface(texto);
	
}

//	PASO 6: dibujar el minimapa
void GraficadorPantalla::dibujarMinimapa(int minimapX, int minimapY, int minimapW, int minimapH){
	SDL_Rect pixel;
	pixel.h = 2;
	pixel.w = 2;
	int i, j;
	
	for(i = 0; i < this->partida->escenario->verTamX(); i++)
		for(j = 0; j < this->partida->escenario->verTamX(); j++){
			Posicion pAct = Posicion(i, j);
			if(this->player->visionCasilla(pAct) != VIS_NO_EXPLORADA){
				pixel.x = minimapX - 4;
				pixel.y = minimapY;
				ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
				pixel.x += floor(cu->obtenerCoordPantallaX(i, j, 0, 0, minimapW)/LARGO_TILE);
				pixel.y += floor(cu->obtenerCoordPantallaY(i, j, 0, 0, minimapW)/LARGO_TILE);

				if(this->partida->escenario->verMapa()->posicionEstaVacia(&pAct))
					SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 51, 151, 37));
				else if(this->partida->escenario->verMapa()->verContenido(&pAct)->verTipo() == "resource")
					SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 250, 200, 16));
				else
					SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 10, 38, 163));
			}
		}
	// LA CAMARA	
	pixel.h = 1;
	pixel.w = 1;

	// Obtengo las coordenadas del minimapa de la view
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	float viewXlogico = cu->obtenerCoordLogicaX(0, 0, view_x, view_y, ancho_borde);
	float viewYlogico = cu->obtenerCoordLogicaY(0, 0, view_x, view_y, ancho_borde);
	
	float ultimoXlogico = cu->obtenerCoordLogicaX(this->screen_width, this->screen_height, view_x, view_y, ancho_borde);
	float ultimoYlogico = cu->obtenerCoordLogicaY(this->screen_width, this->screen_height, view_x, view_y, ancho_borde);
	
	float topeX = floor(cu->obtenerCoordPantallaX(ultimoXlogico, ultimoYlogico, 0, 0, minimapW)/LARGO_TILE);
	float topeY = floor(cu->obtenerCoordPantallaY(ultimoXlogico, ultimoYlogico, 0, 0, minimapW)/LARGO_TILE);
	
	pixel.x = minimapX - 4;
	pixel.y = minimapY;
	pixel.x += floor(cu->obtenerCoordPantallaX(viewXlogico, viewYlogico, 0, 0, minimapW)/LARGO_TILE);
	pixel.y += floor(cu->obtenerCoordPantallaY(viewXlogico, viewYlogico, 0, 0, minimapW)/LARGO_TILE);
	SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));

	while(pixel.x <= (topeX+minimapX - 4)){
		SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
		pixel.x += 1;
	}

	while(pixel.y <= (topeY+minimapY)){
		SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
		pixel.y += 1;
	}
		
	pixel.x = minimapX - 4;
	pixel.y = minimapY;
	pixel.x += floor(cu->obtenerCoordPantallaX(viewXlogico, viewYlogico, 0, 0, minimapW)/LARGO_TILE);
	pixel.y += floor(cu->obtenerCoordPantallaY(viewXlogico, viewYlogico, 0, 0, minimapW)/LARGO_TILE);

	while(pixel.y <= (topeY+minimapY)){
		SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
		pixel.y += 1;
	}

	while(pixel.x <= (topeX+minimapX - 4)){
		SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
		pixel.x += 1;
	}
	

SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
				
}

// METODOS DE INICIALIZACION Y GETTERS
bool GraficadorPantalla::cargarSDL(bool full_screen, string title) {
	Uint32 flags = SDL_WINDOW_SHOWN;
	if (full_screen) 
		flags = flags || SDL_WINDOW_FULLSCREEN;

	if(SDL_Init( SDL_INIT_EVERYTHING ) < 0)	{
		ErrorLog::getInstance()->escribirLog("CRÍTICO: SDL no pudo inicializarse." , LOG_ERROR);
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
				ErrorLog::getInstance()->escribirLog("CRÍTICO: SDL no pudo inicializarse." , LOG_ERROR);
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
		ErrorLog::getInstance()->escribirLog("Se intento asignar una velocidad de scroll inválida." , LOG_WARNING);

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


SDL_Surface* GraficadorPantalla::renderText(string msj){
	DatosImagen* fuenteData = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen("font");
	if(fuenteData->path == "default.png")
		return NULL;
	SDL_Surface* font = fuenteData->imagen;
	int largo = msj.length();
	SDL_Surface* elTexto = SDL_CreateRGBSurface(0, largo*16, 20,32,0,0,0,0); 
	if(elTexto == NULL)
		return NULL;
	// Relleno de blanco
	SDL_Rect aux;
	aux.x = 0;
	aux.y = 0;
	aux.h = 20;
	aux.w = largo*16;
	SDL_FillRect(elTexto, &aux, SDL_MapRGB(elTexto->format, 255, 255, 255));
	
	// Convierto caracter por caracter:
	int i;
	SDL_Rect rectOrig, rectDest;
	rectOrig.h = 20;
	rectOrig.w = 12;
	rectDest.h = 20;
	rectDest.w = 12;
	rectDest.y = 0;
	rectDest.x = 0;
	for(i = 0; i < largo; i++){
		bool printIt = false;
		if((msj[i] >= 'A') && (msj[i] <= 'Z')) {
			rectOrig.y = 0;
			rectOrig.x = (msj[i] - 'A') * 12 + 1;
			printIt = true;
			rectOrig.w = 12;
		}
		else if((msj[i] >= 'a') && (msj[i] <= 'z')) {
			rectOrig.y = 19;
			rectOrig.x = (msj[i] - 'a') * 12 + 2;
			rectOrig.w = 12;
			printIt = true;
		}
		if(printIt)
			SDL_BlitSurface(font, &rectOrig, elTexto, &rectDest);
		rectDest.x += rectOrig.w;
	}
	SDL_Surface* texto = SDL_ConvertSurface(elTexto, pantalla->format, NULL);
	SDL_FreeSurface(elTexto);
	SDL_SetColorKey(texto, true, SDL_MapRGB(elTexto->format, 255, 255, 255));
	return texto;
}