#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "BibliotecaDeImagenes.h"
#include "DatosImagen.h"
#include "ErrorLog.h"
#include "Enumerados.h"
#include <map>
#include <string>
#include <stdio.h>
using namespace std;

#define DEFAULT_IMAGE_LABEL "default.png"
#define DEFAULT_SND "def_snd"

bool BibliotecaDeImagenes::hay_instancia = false;
SDL_Surface* BibliotecaDeImagenes::pantalla = NULL;
BibliotecaDeImagenes* BibliotecaDeImagenes::singleton = NULL;

// El constructor siempre asigna una imagen por defecto al ser creado.
// Si se busca una imagen y no se encuentra, se redirigir� a este valor.
BibliotecaDeImagenes::BibliotecaDeImagenes(void) {
	DatosImagen* def = cargarImagenDefault();
	this->sonidos[DEFAULT_SND] = Mix_LoadWAV("recursos\\default.wav");
	this->imagenes[DEFAULT_IMAGE_LABEL] = def;
}

// Devuelve una imagen con sus datos por defecto.
// TODO: reemplazar el caso de error por una surface creada por software (un cuadrado amarillo, por ejemplo)
DatosImagen* BibliotecaDeImagenes::cargarImagenDefault() {
	DatosImagen* def = new DatosImagen();
	def->columnas = 1;
	def->filas = 1;
	def->fps = 1;
	def->delay = 0;
	def->path = "recursos\\default.png";
	if (!this->cargarImagen(def)) {
		ErrorLog::getInstance()->escribirLog("No se encuentra imagen por defecto!!", LOG_ERROR);
	} else {
		def->altoImagen = def->imagen->h;
		def->anchoImagen = def->imagen->w;
		def->origenX = def->anchoImagen/2;
		def->origenY = def->altoImagen/2;
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

	DatosImagen* def = cargarImagenDefault();
	imagenes[DEFAULT_IMAGE_LABEL] = def;

	map<string, Mix_Chunk*>::const_iterator it2 = sonidos.begin();
	while (it2 != sonidos.end()) {
		Mix_FreeChunk( it2->second ) ;
		it2++;
	}

}


BibliotecaDeImagenes::~BibliotecaDeImagenes(void) {
	map<string, DatosImagen*>::const_iterator it = imagenes.begin();
	while (it != imagenes.end()) {
		delete it->second;
		it++;
	}
	imagenes.clear();
	hay_instancia = false;
}

BibliotecaDeImagenes* BibliotecaDeImagenes::obtenerInstancia(void) {
	if(!hay_instancia){
		ErrorLog::getInstance()->escribirLog("Se inicializa la biblioteca de im�genes de la partida.", LOG_INFO);
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


// M�todo que carga una surface a partir de un path cualquiera.
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


// M�todo que carga una surface al paquete DatosImagen a partir de su path.
// Si no se encuentra la im�gen especificada, o no se puede cargar; loguea el evento y devuelve false.
bool BibliotecaDeImagenes::cargarImagen(DatosImagen* data) {
	SDL_Surface* imagenActual = loadSurface(data->path);
	if(!imagenActual) {
		ErrorLog::getInstance()->escribirLog("Error al cargar im�gen: " + data->path + ".�Imagen inexistente!", LOG_ERROR);
		data->imagen = nullptr;
		return false;
	}
	// Si la surface se carg�, la agrego al
	// mapa de imagenes
	data->imagen = imagenActual;
	return true;
}

void BibliotecaDeImagenes::cargarEfectoSonido(string name) {
	string p = "recursos\\snd\\" + name + ".wav";
	char* filName = &p[0u];
	Mix_Chunk *efectoSonido = Mix_LoadWAV(filName);
	if(efectoSonido == NULL){
		ErrorLog::getInstance()->escribirLog("Error al cargar sonido: " + name, LOG_ERROR);
		return;
	}

	this->sonidos[name] = efectoSonido;
	
}

Mix_Chunk* BibliotecaDeImagenes::devolverSonido(string snd_name) {
	if(sonidos.count(snd_name) > 0)
		return sonidos[snd_name];

	return sonidos[DEFAULT_SND];
}

// Este m�todo es el p�blico que se llamar� cada vez que se quiera obtener una instancia
// de la im�gen asociada a un nombre particular. Si la im�gen no se encuentra devuelve la
// im�gen por default.
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
	if (name == DEFAULT_IMAGE_LABEL) {
		delete data;
		ErrorLog::getInstance()->escribirLog("Se intent� sobreescribir la im�gen por defecto [" + nombre_entidad_def + "]. Operaci�n cancelada.", LOG_WARNING);
		return;
	}
	bool success = this->cargarImagen(data);
	if (success) {
		if (this->imagenes.count(name) >0) {
			ErrorLog::getInstance()->escribirLog("Se intent� cargar imagenes distintas para una misma entidad, se sobreescribir� el valor previo", LOG_INFO);
			delete this->imagenes[name];
		}
		this->imagenes[name] = data;
	} else {
		// Si no se puede cargar la im�gen, borro la informaci�n y no cargo nada. (Agregar una referencia al default??)
		delete data;
	}
}
