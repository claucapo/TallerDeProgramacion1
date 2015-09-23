#include <SDL.h>
#include <SDL_image.h>
#include "BibliotecaDeImagenes.h"
#include "DatosImagen.h"
#include "ErrorLog.h"
#include "Enumerados.h"
#include <map>
#include <string>
#include <stdio.h>
using namespace std;

#define DEFAULT_IMAGE_LABEL "default.png"

bool BibliotecaDeImagenes::hay_instancia = false;
SDL_Surface* BibliotecaDeImagenes::pantalla = NULL;
BibliotecaDeImagenes* BibliotecaDeImagenes::singleton = NULL;

// El constructor siempre asigna una imagen por defecto al ser creado.
// Si se busca una imagen y no se encuentra, se redirigirá a este valor.
BibliotecaDeImagenes::BibliotecaDeImagenes(void) {
	DatosImagen* def = cargarImagenDefault();
	this->imagenes[DEFAULT_IMAGE_LABEL] = def;
}

// Devuelve una imagen con sus datos por defecto.
// TODO: reemplazar el caso de error por una surface creada por software (un cuadrado amarillo, por ejemplo)
DatosImagen* BibliotecaDeImagenes::cargarImagenDefault() {
	DatosImagen* def = new DatosImagen();
	def->columnas = 1;
	def->filas = 1;
	def->origenX = 0;
	def->origenY = 0;
	def->fps = 1;
	def->delay = 0;
	def->casillasX = 1;
	def->casillasY = 1;
	def->path = "default.png";
	if (!this->cargarImagen(def)) {
		ErrorLog::getInstance()->escribirLog("No se encuentra imagen por defecto!!", LOG_ERROR);
	} else {
		def->altoImagen = def->imagen->h;
		def->anchoImagen = def->imagen->w;
	}
	return def;
}


void BibliotecaDeImagenes::clear(void) {
	map<string, DatosImagen*>::const_iterator it = imagenes.begin();
	while (it != imagenes.end()) {
		delete it->second;
		it++;
	}
	imagenes.clear();
}


BibliotecaDeImagenes::~BibliotecaDeImagenes(void) {

}

BibliotecaDeImagenes* BibliotecaDeImagenes::obtenerInstancia(void) {
	if(!hay_instancia){
		ErrorLog::getInstance()->escribirLog("Se inicializa la biblioteca de imágenes de la partida.", LOG_INFO);
		singleton = new BibliotecaDeImagenes();
		hay_instancia = true;
		pantalla = NULL;
		}
	return singleton;
}

void BibliotecaDeImagenes::asignarPantalla(SDL_Surface* screen){
	if(!screen) {
		ErrorLog::getInstance()->escribirLog("FATAL: Pantalla invalida.", LOG_ERROR);
		return;
		}
	this->pantalla = screen;
}


// Método que carga una surface a partir de un path cualquiera.
// Si ocurre algun error, devuelve NULL.
SDL_Surface* BibliotecaDeImagenes::loadSurface(std::string path) {
	SDL_Surface* optimizedSurface = NULL;

	//Carga la imagen del path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if(!loadedSurface)
		ErrorLog::getInstance()->escribirLog("SDL_image Error:  con imagen" +  path, LOG_ERROR);

	// Si hay una pantalla, optimizo el formato de la
	// imagen cargada al de la pantalla
	else if(pantalla){
		optimizedSurface = SDL_ConvertSurface(loadedSurface, pantalla->format, NULL);
		if(!optimizedSurface)
			ErrorLog::getInstance()->escribirLog("Error al optimizar: " + path, LOG_ERROR);
		SDL_FreeSurface(loadedSurface);
		return optimizedSurface;
	}
	return loadedSurface;
}


// Método que carga una surface al paquete DatosImagen a partir de su path.
// Si no se encuentra la imágen especificada, o no se puede cargar; loguea el evento y devuelve false.
bool BibliotecaDeImagenes::cargarImagen(DatosImagen* data) {
	SDL_Surface* imagenActual = loadSurface(data->path);
	if(!imagenActual) {
		ErrorLog::getInstance()->escribirLog("Error al cargar imágen: " + data->path + ".¡Imagen inexistente!", LOG_ERROR);
		data->imagen = nullptr;
		return false;
	}
	// Si la surface se cargó, la agrego al
	// mapa de imagenes
	data->imagen = imagenActual;
	return true;
}


// Este método es el público que se llamará cada vez que se quiera obtener una instancia
// de la imágen asociada a un nombre particular. Si la imágen no se encuentra devuelve la
// imágen por default.
SDL_Surface* BibliotecaDeImagenes::devolverImagen(string img_name) {
	if(imagenes.count(img_name) > 0)
		return imagenes[img_name]->imagen;

	return imagenes[DEFAULT_IMAGE_LABEL]->imagen;
}

DatosImagen* BibliotecaDeImagenes::devolverDatosImagen(string img_name) {
	if(imagenes.count(img_name) > 0)
		 return imagenes[img_name];

	return imagenes[DEFAULT_IMAGE_LABEL];
}

void BibliotecaDeImagenes::cargarDatosImagen(string name, DatosImagen* data) {
	bool success = this->cargarImagen(data);
	if (success) {
		if (this->imagenes.count(name) >0) {
			ErrorLog::getInstance()->escribirLog("Se intentó cargar imagenes distintas para una misma entidad, se sobreescribirá el valor previo", LOG_INFO);
			delete this->imagenes[name];
		}
		this->imagenes[name] = data;
	} else {
		// Si no se puede cargar la imágen, borro la información y no cargo nada. (Agregar una referencia al default??)
		delete data;
	}
}
