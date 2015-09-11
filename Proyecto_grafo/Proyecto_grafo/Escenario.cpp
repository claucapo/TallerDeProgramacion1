#include "Escenario.h"
#include "Matriz.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Unidad.h"
#include <list>
#define TAM_DEFAULT 100

/*
void Escenario::generarMapaVacio(int casillas_x, int casillas_y){
	// Primero agrego las Posicion
	int i, j;
	for(i = 0; i < casillas_x; i++)
		for(j = 0; j < casillas_y; j++)
			mapa.crearVertice(Posicion(i, j));

	// Debo crear las aristas
	for(i = 0; i < casillas_x; i++)
		for(j = 0; j < casillas_y; j++){
			Posicion org(i, j);
			list<Posicion> adyacentes;
			adyacentes.push_front(Posicion(i-1, j));
			adyacentes.push_front(Posicion(i, j-1));
			adyacentes.push_front(Posicion(i-1, j-1));
			adyacentes.push_front(Posicion(i+1, j-1));
			// Itero para los adyacentes y creo arista
			for (list<Posicion>::iterator it = adyacentes.begin(); it != adyacentes.end(); ++it)
				if(mapa.elementoPertenece(*it))
					if(!mapa.crearCamino(org, *it, true, 0))
						return; // Lanzar excepcion?
		}
}
*/

Escenario::Escenario(int casillas_x, int casillas_y)
{
	mapa = new Matriz(casillas_x, casillas_y);
}

Escenario::Escenario(void)
{
	mapa = new Matriz(TAM_DEFAULT, TAM_DEFAULT);
}

Escenario::~Escenario(void)
{
	delete mapa;
}


void Escenario::avanzarFrame(void)
{
	for(list<Entidad>::iterator it = entidades.begin(); it != entidades.end(); it++)
		(*it).avanzarFrame();
}


void Escenario::ubicarEntidad(Entidad entidad, Posicion pos)
{

	// Asumo que la entidad fue creada recientemente
	// Mejor: Lanzar excepcion en lugar de preguntar
	if (mapa->ubicarEntidad(entidad, pos)) {
		entidades.push_back(entidad);
		entidad.asignarPos(&pos);
	}

}


void Escenario::quitarEntidad(Entidad entidad)
{
	// lo mismo que antes
	mapa->quitarEntidad(entidad);
	entidades.remove(entidad);
}


void Escenario::moverUnidad(Unidad unidad, Posicion destino)
{
	mapa->quitarEntidad(unidad);
	unidad.nuevoDestino(destino.getX() + 0.5, destino.getY() + 0.5);
	mapa->ubicarEntidad(unidad, *(unidad.verPosicion()));
}
