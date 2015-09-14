#include "GraficadorPantalla.h"
#include "Spritesheet.h"
#include "Escenario.h"
#include <SDL.h>
#include <SDL_image.h>
#include "ConversorUnidades.h"
#include "BibliotecaDeImagenes.h"
#include <list>
#include <iostream>
#include <conio.h>

using namespace std;

GraficadorPantalla::GraficadorPantalla(void)
	: pantalla(NULL)
{
}


GraficadorPantalla::~GraficadorPantalla(void)
{
}


#define VIEW_X_DEFAULT (ancho_borde-screen_width)/2
#define VIEW_Y_DEFAULT (alto_borde-screen_height)/2
GraficadorPantalla::GraficadorPantalla(Escenario* escenario, int pant_width, int pant_height)
{
	this->escenario = escenario;
	this->screen_height = pant_height;
	this->screen_width = pant_width;
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	this->alto_borde = cu->convertULToPixels(escenario->verTamX());
	this->ancho_borde = 1.732 * cu->convertULToPixels(escenario->verTamY());
	this->view_x = VIEW_X_DEFAULT;
	this->view_y = VIEW_Y_DEFAULT;
	if(!cargarSDL())
		delete this;
}


SDL_Surface* GraficadorPantalla::getPantalla(void)
{
	return pantalla;
}


void GraficadorPantalla::dibujarPantalla(void)
{
	// PASOS DEL DIBUJO DE LA PANTALLA
	// 1) Scroll del mapa (cambiar view_x y view_y)
	// 2) Dibujar el Escenario
	// 3) Dibujar las unidades
	// 4) Dibujar los edificios
	// 5) Window_Update
	SDL_FillRect(pantalla, NULL, 0x000000);
	SDL_PumpEvents(); // Actualiza los eventos de SDL

	SDL_Rect rectangulo;
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();

	// 1)
	reajustarCamara();

	// 2) ESTE PASO ESTA SEMI HARDCODEADO
	SDL_Surface* imgTile = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tileG.png");
	SDL_SetColorKey( imgTile, true, SDL_MapRGB(imgTile->format, 255, 255, 255) );
	
	int i=0, j=0;
	for(i = 0; i < escenario->verTamX(); i++)
		for(j = 0; j < escenario->verTamY(); j++){
			rectangulo.x = cu->obtenerCoordPantallaX(i, j, view_x, view_y, ancho_borde);
			rectangulo.y = cu->obtenerCoordPantallaY(i, j, view_x, view_y, ancho_borde);
			SDL_BlitSurface( imgTile, NULL, pantalla, &rectangulo );
		}

	// 3)
	SDL_Rect recOr;
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
		

	// 4)
	list<Entidad*> lEnt = escenario->verEntidades();
	for (list<Entidad*>::iterator it=lEnt.begin(); it != lEnt.end(); ++it){
		Spritesheet* entAct = (*it)->verSpritesheet();
		SDL_Surface* spEnt = entAct->devolverImagenAsociada();
		SDL_SetColorKey( spEnt, true, SDL_MapRGB(spEnt->format, 255, 0, 255) );
	
		newX = (int) cu->obtenerCoordPantallaX((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
		newY = (int) cu->obtenerCoordPantallaY((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
		entAct->cambirCoord(newX, newY);
		rectangulo.x = entAct->getCoordX();
		rectangulo.y = entAct->getCoordY();

		SDL_BlitSurface( spEnt, NULL, pantalla, &rectangulo );
		}

	// 5)
	SDL_UpdateWindowSurface(ventana);

}

#define MARGEN 66		// Distancia para scrollear
#define K_SCREEN 0.2	// Constante mágica
#define VEL_ZERO 19		// Velocidad de scroll en el borde de la pantalla		
void GraficadorPantalla::reajustarCamara(void)
{
	int mx = 5 , my = 9;
	SDL_GetMouseState(&mx, &my);
	if(mx < MARGEN)
		view_x += -VEL_ZERO + mx * K_SCREEN;
	else if(mx > screen_width - MARGEN)
		view_x += VEL_ZERO*(1 - screen_width/MARGEN) + VEL_ZERO*mx/(MARGEN);
	if(my < MARGEN)
		view_y += -VEL_ZERO + my * K_SCREEN;
	else if(my> screen_height - MARGEN)
		view_y += VEL_ZERO*(1 - screen_height/MARGEN) + VEL_ZERO*my/(MARGEN);
}


bool GraficadorPantalla::cargarSDL(void)
{

	if(SDL_Init( SDL_INIT_VIDEO ) < 0)	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;
	}
	else
	{
		ventana = SDL_CreateWindow( "AGE OF TALLER DE PROGRAMACION I", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN );
		if(!ventana)
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			return false;
		}
		else
		{
			int flags = IMG_INIT_PNG;
			if(!(IMG_Init(flags) & flags))
			{
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				return false;
			}
			else
				pantalla = SDL_GetWindowSurface(ventana);
		}
	}
	return true;
}
