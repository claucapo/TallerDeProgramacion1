#include <list>
#include "Matriz.h"
#include "Spritesheet.h"
using namespace std;
#define TAM_DEFAULT 100

Matriz::Matriz(void) {

	this->filas = TAM_DEFAULT;
	this->columnas = TAM_DEFAULT;
	this->cantidad_entidades = 0;

	casillas = new Entidad*[TAM_DEFAULT];
	for(int i = 0; i < TAM_DEFAULT; i++)
		casillas[i] = new Entidad[TAM_DEFAULT];

	generarMatrizVacia();
}

Matriz::Matriz(int casillas_x, int casillas_y) {

	this->filas = casillas_x;
	this->columnas = casillas_y;
	this->cantidad_entidades = 0;

	casillas = new Entidad*[casillas_x];
	for(int i = 0; i < casillas_x; i++)
		casillas[i] = new Entidad[casillas_y];

	generarMatrizVacia();
}

Matriz::~Matriz(void) {

	for(int i = 0; i < this->filas; i++)
		delete[] casillas[i];
	delete[] casillas;
} 

void Matriz::generarMatrizVacia(){

	for(int i = 0; i < this->filas; i++){
		for(int j = 0; j < this->columnas; j++)
		 casillas[i][j] = nullptr;
	}
	
} 

bool Matriz::posicionPertenece(Posicion pos){

	int pos_x = (int)(pos.getX() + 0.5);
	int pos_y = (int)(pos.getY() + 0.5);

	if ((this->filas < pos_x) || (this->columnas < pos_y))
		return false;
	return true;
}


bool Matriz::quitarEntidad(Entidad elemento){

	Posicion pos = *(elemento.verPosicion());
	if (! posicionPertenece(pos))
		return false; //lanzar excepcion?
	// Obtengo la base del sprite asociado a la entidad
	Spritesheet* sprite = elemento.verSprites();
	int altura_x = (*sprite).subImagenHeight();
	int ancho_y = (*sprite).subImagenWidth();

	for(int i = 0; i < altura_x; i++){
		for(int j = 0; j < ancho_y; j++)
			casillas[(int)(pos.getX() + 0.5) + i][(int)(pos.getY() + 0.5) + j] = nullptr;
	}
	this->cantidad_entidades--;
	return true;
}

bool Matriz::ubicarEntidad(Entidad elemento, Posicion pos){

	// Despues armar metodo privado para no repetir codigo
	if (! posicionPertenece(pos))
		return false;
	// Obtengo la base del sprite asociado a la entidad
	Spritesheet* sprite = elemento.verSprites();
	int altura_x = (*sprite).subImagenHeight();
	int ancho_y = (*sprite).subImagenWidth();

	for(int i = 0; i < altura_x; i++){
		for(int j = 0; j < ancho_y; j++)
			casillas[(int)(pos.getX() + 0.5) + i][(int)(pos.getY() + 0.5) + j] = elemento;
	}
	this->cantidad_entidades++;
	return true;
}