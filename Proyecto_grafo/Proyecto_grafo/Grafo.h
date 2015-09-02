#include <map>
#include "Posicion.h"
#pragma once
using namespace std;

struct classcomp {
  bool operator() (const Posicion p1, const Posicion p2) const
 // {return (((Posicion)p1).devolverCoordX()) < ((Posicion)p2).devolverCoordX();}
  {return (((Posicion)p1).generarCodigo() < ((Posicion)p2).generarCodigo());}
};
class Grafo
{
	// Atributos del grafo
private:
	int cantidad;
	// Este map asocia el objeto Posicion con
	// un map de los objetos conectados a el.
	map<Posicion, map<Posicion, int, classcomp>, classcomp> nodos; 

public:
	Grafo(void);
	~Grafo(void);
	// Primitivas del grafo
	bool crearVertice(Posicion elemento);
	bool crearCamino(Posicion origen, Posicion destino, bool idaYvuelta, int peso);
	bool elementoPertenece(Posicion elemento);
	bool elementosEstanConectados(Posicion origen, Posicion destino);
	bool estaVacio();
	int size();
	map<Posicion, int, classcomp> devolverCaminos(Posicion elemento);
	void borrarVertice(Posicion elemento);
	void borrarCamino(Posicion origen, Posicion destino);
};
