#include <SDL.h>
#include <SDL_image.h>
#include "BibliotecaDeImagenes.h"
#include "DatosImagen.h"
#include <map>
#include <string>
#include <stdio.h>


using namespace std;


bool BibliotecaDeImagenes::hay_instancia = false;

SDL_Surface* BibliotecaDeImagenes::pantalla = NULL;

BibliotecaDeImagenes* BibliotecaDeImagenes::singleton = NULL;

BibliotecaDeImagenes::BibliotecaDeImagenes(void) {
}


BibliotecaDeImagenes::~BibliotecaDeImagenes(void) {
	//
	map<string, DatosImagen*>::iterator it;
	for(it = imagenes.begin(); it != imagenes.end(); it++){
		SDL_FreeSurface(it->second->imagen);
		free(it->second);
		}
	imagenes.clear();
	delete singleton;
}


BibliotecaDeImagenes* BibliotecaDeImagenes::obtenerInstancia(void)
{
	if(!hay_instancia){
		singleton = new BibliotecaDeImagenes();
		hay_instancia = true;
		pantalla = NULL;
		}

	return singleton;
}

SDL_Surface* BibliotecaDeImagenes::loadSurface(std::string path)
{
	SDL_Surface* optimizedSurface = NULL;
	//Carga la imagen del path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if(!loadedSurface)
		printf("SDL_image Error: %s con imagen %s\n", IMG_GetError(), path.c_str());
	// Si hay una pantalla, optimizo el formato de la
	// imagen cargada al de la pantalla
	else if(pantalla){
		optimizedSurface = SDL_ConvertSurface(loadedSurface, pantalla->format, NULL);
		if(!optimizedSurface)
			printf("Error al optimizar %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		SDL_FreeSurface(loadedSurface);
		return optimizedSurface;
	}
	return loadedSurface;
}

bool BibliotecaDeImagenes::cargarImagen(string img_name) {
	SDL_Surface* imagenActual = loadSurface( img_name );
	if(!imagenActual) {
		printf( "\r\nError al cargar imagen. ¡Imagen inexistente!" );
		return false;
	}

	// Si la surface se cargó, la agrego al
	// mapa de imagenes
	DatosImagen* dataImg = new DatosImagen(imagenActual);
	imagenes[img_name] = dataImg;
	return true;
}



SDL_Surface* BibliotecaDeImagenes::devolverImagen(string img_name) {
	if(imagenes.count(img_name) > 0)
		return imagenes[img_name]->imagen;
	// Si la surface no estaba en el mapa
	// de imagenes, la agrego
	if(cargarImagen(img_name))
		return imagenes[img_name]->imagen;
	return NULL;
}


void BibliotecaDeImagenes::asignarPantalla(SDL_Surface* screen){
	if(!screen)
		return;
	this->pantalla = screen;
}


bool BibliotecaDeImagenes::devolverCargadosDatosImagen(string img_name, int* altoImagen, int* anchoImagen, int* cantFilas, int* cantCol, int* coordOrigX, int* coordOrigY, int* casillasX, int* casillasY)
{
	if(imagenes.count(img_name) > 0){
		*altoImagen = imagenes[img_name]->altoImagen;
		*anchoImagen = imagenes[img_name]->anchoImagen;
		*cantFilas = imagenes[img_name]->filas;
		*cantCol = imagenes[img_name]->columnas;
		*coordOrigX = imagenes[img_name]->origenX;
		*coordOrigY = imagenes[img_name]->origenY;
		*casillasX = imagenes[img_name]->casillasX;
		*casillasY = imagenes[img_name]->casillasY;
		return true;
		}
	return false;
}


bool BibliotecaDeImagenes::asignarDatosAImagen(string img_name, int altoImagen, int anchoImagen, int cantFilas, int cantColumnas, int origX, int origY, int casillasX, int casillasY)
{
	if(imagenes.count(img_name) > 0){
		imagenes[img_name]->altoImagen = altoImagen;
		imagenes[img_name]->anchoImagen = anchoImagen;
		imagenes[img_name]->filas = cantFilas;
		imagenes[img_name]->columnas = cantColumnas;
		imagenes[img_name]->origenX = origX;
		imagenes[img_name]->origenY = origY;
		imagenes[img_name]->casillasX = casillasX;
		imagenes[img_name]->casillasY = casillasY;
		return true;
		}
	return false;
}
