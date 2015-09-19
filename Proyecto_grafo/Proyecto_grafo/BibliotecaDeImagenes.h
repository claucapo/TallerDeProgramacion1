#include <SDL.h>
#include <SDL_image.h>
#include "DatosImagen.h"
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
Además, en esta clase éstán todos los datos asociados a
dicha imagen. A saber: su alto, su ancho, su cantidad de
filas, su cantidad de columnas, sus coordenadas de origen
y su cantidad de casillas en X y en Y.

NOTA IMPORTANTE: Esta clase requiere que SDL esté
inicializado correctamente para funcionar.*/

class BibliotecaDeImagenes
{
private:
	// Constructor privado que inicializa el map
	BibliotecaDeImagenes(void);

	// Método privado para cargar una SDL_Surface
	SDL_Surface* loadSurface(std::string path);

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
	map<string, DatosImagen*> imagenes;

	// Pantalla asociada a la BibliotecaDeImagenes sobre
	// la cual optimizar las imagenes.
	static SDL_Surface* pantalla;

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

	// Asigna la pantalla screen
	void asignarPantalla(SDL_Surface* screen);

	// Busca la imagen img_name y devuelve sus datos
	// asociados en los int* recibidos como parámetro.
	// Devuelve true si la imagen fue encontrada y
	// sus datos se devolvieron con éxito, o false en
	// caso contrario. En caso de false, las posiciones
	// apuntadas por los punteros recibidos mantienen su
	// valor original.
	bool devolverCargadosDatosImagen(string img_name, int* altoImagen, int* anchoImagen, int* cantFilas, int* cantCol, int* coordOrigX, int* coordOrigY, int* casillasX, int* casillasY);
	
	// Busca la imagen img_name y le asocia los datos
	// recibidos como parámetros. Devuelve true si la
	// carga fue exitosa o false en caso contrario.
	// NOTA: esta función no hace chequeos de ningún
	// tipo sobre los datos recibidos.
	bool asignarDatosAImagen(string img_name, int altoImagen, int anchoImagen, int cantFilas, int cantColumnas, int origX, int origY, int casillasX, int casillasY);
};
