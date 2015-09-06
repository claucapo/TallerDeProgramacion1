#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <string>
#include <stdio.h>

#pragma once

using namespace std;

/* La clase de BibliotecaDeImagenes es justamente
eso: la colección con las imágenes de cualquier cosa
que tengamos que mostrar. Para no cargar dichas imágenes
en memoria cada vez que tengamos que usarlas, esta clase
las carga una sola vez (la primera que se las necesita)
y luego permite acceder a esa misma imagen varias veces.

NOTA IMPORTANTE: Esta clase requiere que SDL esté
inicializado correctamente para funcionar.
NOTA IMPORTANTE 2: Aún falta liberar memoria al destruirse xd
*/

class BibliotecaDeImagenes
{
private:
	// Constructor privado que inicializa el map
	BibliotecaDeImagenes(void);

	// Carga una imagen en memoria y la guarda en
	// el mapa de imagenes.
	bool cargarImagen(string img_name);

	// Este atributo es la biblioteca singleton que
	// es devuelta cada vez que se llama al metodo
	// obtenerInstancia
	static BibliotecaDeImagenes* singleton;

	// Flag para saber si hay instancia o no
	static bool hay_instancia;

	// Este mapa relaciona los nombres de las imágenes
	// con sus determinados punteros a imagen según SDL
	map<string, SDL_Surface*> imagenes;



public:
	// Destructor por defecto
	~BibliotecaDeImagenes(void);

	// Devuelve una instancia singleton de la
	// BibliotecaDeImagenes, que o bien ya existía
	// o bien se crea si no existía ninguna.
	static BibliotecaDeImagenes* obtenerInstancia(void);

	// Devuelve el surface asociado a la imagen
	// con nombre img_name. Si la surface asociada
	// a dicha imagen no fue creada, la crea y la
	// agrega al mapa de imagenes.
	SDL_Surface* devolverImagen(string img_name);
};
