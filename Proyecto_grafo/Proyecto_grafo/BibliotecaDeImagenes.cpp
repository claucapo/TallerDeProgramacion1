#include <SDL.h>
#include <SDL_image.h>
#include "BibliotecaDeImagenes.h"
#include <map>
#include <string>
#include <stdio.h>


using namespace std;


bool BibliotecaDeImagenes::hay_instancia = false;

BibliotecaDeImagenes* BibliotecaDeImagenes::singleton = NULL;

BibliotecaDeImagenes::BibliotecaDeImagenes(void) {
}


BibliotecaDeImagenes::~BibliotecaDeImagenes(void) {
	//
	map<string, SDL_Surface*>::iterator it;
	for(it = imagenes.begin(); it != imagenes.end(); it++)
		SDL_FreeSurface(it->second);
	imagenes.clear();
	delete singleton;
}


BibliotecaDeImagenes* BibliotecaDeImagenes::obtenerInstancia(void)
{
	if(!hay_instancia){
		singleton = new BibliotecaDeImagenes();
		hay_instancia = true;
		}

	return singleton;
}


using namespace std;



SDL_Surface* loadSurface(std::string path)
{
	SDL_Surface* optimizedSurface = NULL;
	//Carga la imagen del path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if(!loadedSurface)
		printf( "SDL_image Error: %s con imagen %s\n", IMG_GetError(), path.c_str() );
	else
/*	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, NULL );
		if( optimizedSurface == NULL )
			printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		SDL_FreeSurface( loadedSurface );
	}
	return optimizedSurface;*/
	return loadedSurface;
}

bool BibliotecaDeImagenes::cargarImagen(string img_name)
{
	SDL_Surface* imagenActual = loadSurface( img_name );
	if(!imagenActual) {
		printf( "\r\nError al cargar imagen. ¡Imagen inexistente!" );
		return false;
	}

	// Si la surface se cargó, la agrego al
	// mapa de imagenes
	imagenes[img_name] = imagenActual;

	return true;
}



SDL_Surface* BibliotecaDeImagenes::devolverImagen(string img_name)
{
	if(imagenes.count(img_name) > 0)
		return imagenes[img_name];
	// Si la surface no estaba en el mapa
	// de imagenes, la agrego
	if(cargarImagen(img_name))
		return imagenes[img_name];
	return NULL;
}
