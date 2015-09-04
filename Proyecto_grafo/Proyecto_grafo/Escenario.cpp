#include "Escenario.h"
#include "Grafo.h"
#include "Posicion.h"
#include "Entidad.h"
#include "EstadoEntidad.h"
#include "Unidad.h"
#include <list>

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

Escenario::Escenario(int casillas_x, int casillas_y)
{
	generarMapaVacio(casillas_x, casillas_y);
}

Escenario::Escenario(void)
{
}

Escenario::~Escenario(void)
{
}


void Escenario::avanzarFrame(void)
{
}


void Escenario::ubicarEntidad(Entidad entidad, Posicion pos)
{
}


void Escenario::quitarEntidad(Entidad entidad)
{
}


void Escenario::moverUnidad(Unidad unidad, Posicion destino)
{
}
