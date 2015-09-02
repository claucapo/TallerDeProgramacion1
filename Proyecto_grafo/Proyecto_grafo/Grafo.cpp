#include <map>
#include "Grafo.h"
using namespace std;

Grafo::Grafo(void)
{
	cantidad = 0;
}

Grafo::~Grafo(void)
{
}

bool Grafo::crearVertice(Posicion elemento){
	// Si el vertice ya existía, no lo creo
	if(nodos.count(elemento)>0)
		return false;
	map<Posicion, int, classcomp> conexiones;
	nodos[elemento] = conexiones;
	cantidad++;
	return true;
}

bool Grafo::crearCamino(Posicion origen, Posicion destino, bool idaYvuelta, int peso){
	// Si origen o destino no estan incluidos, no creo camino
	if((!elementoPertenece(origen))||(!elementoPertenece(destino)))
		return false;
	nodos[origen][destino] = peso;
	// Hasta aca agregue el camino origen->destino. Me falta ver
	// si tengo que agregar el camino destino->origen
	if(idaYvuelta)
		return crearCamino(destino, origen, false, peso);
	return true;
}
	
bool Grafo::elementoPertenece(Posicion elemento){
	return (nodos.count(elemento) > 0);
}

bool Grafo::elementosEstanConectados(Posicion origen, Posicion destino){
	// Si origen o destino no estan incluidos, no creo camino
	if((!elementoPertenece(origen))||(!elementoPertenece(origen)))
		return false;
	map<Posicion, int, classcomp> conexiones = nodos[origen];
	return (conexiones.count(destino)>0);
}

bool Grafo::estaVacio(){
	return cantidad==0;
}
	
int Grafo::size(){
	return cantidad;
}

map<Posicion, int, classcomp> Grafo::devolverCaminos(Posicion elemento){
	return nodos[elemento];
}
	
void Grafo::borrarVertice(Posicion elemento){
	if(!elementoPertenece(elemento))
		return;
	nodos[elemento].clear();
	nodos.erase(elemento);
	cantidad--;
}

void Grafo::borrarCamino(Posicion origen, Posicion destino){
	if((!elementoPertenece(origen))||(!elementoPertenece(origen)))
		return;
	if(nodos[origen].count(destino) > 0)
		nodos[origen].erase(destino);
}
