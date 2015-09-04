#include <map>
#include "Posicion.h"
#pragma once
using namespace std;

// Estructura de comparación auxiliar inventada que
// no sirve para nada... Igual no tocar
struct classcomp {
  bool operator() (const Posicion p1, const Posicion p2) const
  {return (((Posicion)p1).generarCodigo() < ((Posicion)p2).generarCodigo());}
};

/* La clase Grafo representa un grafo de Posicion
con aristas pesadas entre ellas.*/
class Grafo
{
	// Atributos del grafo
private:
	int cantidad;
	// Este map asocia el objeto Posicion con
	// un map de los objetos conectados a el.
	map<Posicion, map<Posicion, int, classcomp>, classcomp> nodos; 

public:
	//Constructor y destructor por defecto
	Grafo(void);
	~Grafo(void);

	// Crea un nodo en el grafo con la Posicion elemento
	// asociada. Si el vértice ya existía, no se crea.
	// Pre: -
	// Post: Se agregó el nodo elemento al grafo, se
	// aumentó el tamaño del grafo en 1 y se devolvió
	// true, o bien no se pudo y se devolvió false.
	bool crearVertice(Posicion elemento);

	// Crea un camino origen -> destino, con el peso
	// indicado. Si idaYvuelta es true, ademas crea 
	// el camino en el otro sentido (origen <- destino). 
	// Devuelve true o false segun la funcion haya 
	// funcionado o no.
	// Pre: -
	// Post: se creo el camino y se devolvio true o false.
	// NOTA: si ya existía un camino entre ambos nodos, 
	// se sobrescribe.
	bool crearCamino(Posicion origen, Posicion destino, bool idaYvuelta, int peso);


	// Devuelve true si elemento está en el grafo.
	// Pre: -
	// Post: se devolvio true o false segun corresponda.	
	bool elementoPertenece(Posicion elemento);

	// Devuelve true si origen-destino están conectados.
	// Pre: -
	// Post: se devolvio true o false segun corresponda.
	// NOTA: un nodo origen esta conectado a uno destino 
	// si existe un camino que sale de origen y va a 
	// destino. Si este camino no existe, la funcion 
	// devuelve false incluso aunque exista un camino de 
	// destino a origen.
	bool elementosEstanConectados(Posicion origen, Posicion destino);

	// Devuelve true si el grafo está vacío o false
	// en caso contrario.
	bool estaVacio();

	// Devuelve el tamaño (cant. de nodos) del grafo.
	int size();

	// Devuelve todos los caminos que tienen como 
	// origen a elemento.
	// Pre: -.
	// Post: se devolvio un map con todos sus caminos.
	map<Posicion, int, classcomp> devolverCaminos(Posicion elemento);

	// Borra un vertice del grafo con todos sus 
	// caminos asociados. Si el vértice no existe
	// no se hace nada.
	void borrarVertice(Posicion elemento);


	// Borra el camino que va de origen a destino.
	// NOTA: solo se borra el camino que va de 
	// origen a destino (si existe)y no el que va 
	// de destino a origen.
	void borrarCamino(Posicion origen, Posicion destino);
};
