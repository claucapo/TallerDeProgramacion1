#ifndef MATRIZ_H
#define MATRIZ_H

#include "Entidad.h"
#include "Posicion.h"
#include "CalculadorCaminoMinimo.h"
#include <list>
using namespace std;

/* La clase Matriz representa una cuadricula de posiciones
con x filas e y columnas, que contienen una entidad.*/
class Matriz {
	
private:
	// Atributos
	Entidad*** casillas;
	int cantidad_entidades;
	int columnas;
	int filas;

	int** mapDeOcupaciones;
	CalculadorCaminoMinimo* calculadorCamino;


	// Metodo que crea una matriz vacia.
	void generarMatrizVacia();
	
public:
	// Constructor y destructor por defecto
	Matriz(void);
	~Matriz(void);

	// Constructor sobrecargado.
	// Pre: las casillas deben ser mayores a cero.
	// Post: se crea la matriz con x filas e y columnas.
	Matriz(int casillas_x, int casillas_y);

	// Devuelve true si la posicion está en la matriz.
	// Pre: -
	// Post: se devolvio true o false segun corresponda.	
	bool posicionPertenece(Posicion* pos);

	// Devuelve true si el grafo está vacío o false
	// en caso contrario.
	bool estaVacio(){return (this->cantidad_entidades == 0);}

	bool posicionEstaVacia(Posicion* pos);

	// Devuelve la cantidad de filas de la matriz.
	int verFilas(){return this->filas;}
	// Devuelve la cantidad de columnas de la matriz.
	int verColumnas(){return this->columnas;}

	// Devuelve el contendio de la posición, si la posicion está ocupada
	Entidad* verContenido(Posicion* pos);

	// Elimina el elemento ubicado sobre la matriz.
	// Post: la posicion queda vacia. Devuelve true 
	// o false segun corresponda.
	bool quitarEntidad(Entidad* elemento);

	// Coloca la entidad sobre la posicion seleccionada.
	// incluyendo tambien las posiciones que ocupa.
	// Si la posicion esta ocupada, no ubica al elemento
	// y devuelve false.
	bool ubicarEntidad(Entidad* elemento, Posicion* pos);

	// Distancia entre una posición y una entidad
	int distanciaEntre(Posicion pos, Entidad* ent);

	// Devuelve la lista de posiciones que caen dentro del rango de vision de una entidad
	list<Posicion> posicionesVistas(Entidad* elemento);
	void actualizarMapDeOcupaciones();
	string Matriz::caminoMinimo(Posicion posAct, Posicion posDest);

};



#endif // MATRIZ_H