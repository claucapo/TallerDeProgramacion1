#include "CalculadorCaminoMinimo.h"
#include "Nodo.h"
#include <iostream>
#include <iomanip>
#include <queue>
#include <string>
#include <math.h>
#include "Posicion.h"
#include <list>
#include <ctime>
using namespace std;

CalculadorCaminoMinimo::CalculadorCaminoMinimo(void){
}

CalculadorCaminoMinimo::CalculadorCaminoMinimo(int filas,int columnas)
{
	this-> filas = filas;
	this-> columnas = columnas;

	listaCerrada = new int*[filas];
	 for(int a = 0; a < filas; a++)
		listaCerrada[a] = new int[columnas];

	listaAbierta = new int*[filas];
	 for(int a = 0; a < filas; a++)
		listaAbierta[a] = new int[columnas];

	listaDirecciones = new int*[filas];
	 for(int a = 0; a < filas; a++)
		listaDirecciones[a] = new int[columnas];
}


CalculadorCaminoMinimo::~CalculadorCaminoMinimo(void)
{
	for(int i = 0; i < this->filas; i++){
		delete[] listaCerrada[i];
		delete[] listaAbierta[i];
		delete[] listaDirecciones[i];
	}
	delete[] listaCerrada;
	delete[] listaAbierta;
	delete[] listaDirecciones;
}


class mycomparison
{
  bool reverse;
public:
  mycomparison(const bool& revparam=false)
    {reverse=revparam;}
  bool operator() (const Nodo& lhs, const Nodo& rhs) const
  {
    if (reverse) return (lhs.obtenerPrioridad() < rhs.obtenerPrioridad());
    else return (lhs.obtenerPrioridad() > rhs.obtenerPrioridad());
  }
};

list<Posicion*> CalculadorCaminoMinimo::calcularCaminoMinimo( int xAct, int yAct, int xDest, int yDest, int** mapDeOcupaciones, terrain_type_t** mapDeTerreno, terrain_type_t validTerrain) {

	//diferenciales segun el movimiento
	int dx[8]={1, 1, 0, -1, -1, -1, 0, 1};
	int dy[8]={0, 1, 1, 1, 0, -1, -1, -1};
 
    static priority_queue<Nodo, vector<Nodo>, mycomparison> pN[2]; // Nodos aun no visitados
    static int pNi; // index del Nodo
    static Nodo* n0;
    static Nodo* m0;
    static int i, j, x, y, xdx, ydy;
    static char c;

    pNi = 0;

	//inicializar en cero la estructura
    for(int y = 0; y < this->columnas ; y++) {
        for(int x = 0; x < filas ; x++) {
            listaCerrada[x][y] = 0;
            listaAbierta[x][y] = 0;
        }
    }

	n0=new Nodo(xAct, yAct, 0, 0);
    n0->actualizarPrioridad(xDest, yDest);
    pN[pNi].push(*n0);
	if((x>0)&&(y>0))
		listaAbierta[x-1][y-1] = n0->obtenerPrioridad();

	// Mientras no haya nodos que probar
    while(!pN[pNi].empty()) {
		// Tomo el nodo que tiene mayor prioridad
        n0 = new Nodo( pN[pNi].top().obtenerPosX(), pN[pNi].top().obtenerPosY(),pN[pNi].top().obtenerNivel(), pN[pNi].top().obtenerPrioridad());

        x = n0->obtenerPosX(); 
		y = n0->obtenerPosY();

        pN[pNi].pop(); 
        listaAbierta[x][y] = 0;
        listaCerrada[x][y] = 1;

		// Si ese nodo es el mas cercano, llegue al destino... genero una lista de posiciones
        if( x == xDest && y == yDest) {
			list<Posicion*> camino = list<Posicion*>();
			Posicion* pNew = new Posicion(x, y);
			camino.push_front(pNew);

            while(!(x == xAct && y == yAct)){
                j = listaDirecciones[x][y];
                x += dx[j];
                y += dy[j];
				pNew = new Posicion(x, y); // Los waypoints del camino son desde los centros de las casillas
				camino.push_front(pNew);
            }

			// Limpio la lista de nodos que quedaron y devuelvo el camino
            delete n0;
            while(!pN[pNi].empty()) pN[pNi].pop(); 

			return camino;
        }

		// Si el nodo no es el destino, voy a iterar entre las ocho casillas adyacentes
        for(int i = 0; i < 8; i++) {
			// xdx e ydy son mi nuevo waypoint
            xdx = x + dx[i];
			ydy = y + dy[i];

			// Me fijo que no se a una casilla que he visitado, y que caiga dentro del mapa
            if( !(xdx < 0 || xdx > filas-1 || ydy < 0 || ydy > columnas-1 || listaCerrada[xdx][ydy] == 1) )
			// Chequeo que sea terreno válido y que no esté ocuado
			if(mapDeTerreno[xdx][ydy] == validTerrain && mapDeOcupaciones[xdx][ydy] == 0) {

				// Creo un nuevo nodo para la nueva posicion
                m0 = new Nodo( xdx, ydy, n0->obtenerNivel(), n0->obtenerPrioridad());
                m0->siguienteNivel(i);
                m0->actualizarPrioridad(xDest, yDest);

				// Si no tengo ningún camino registrado para esa casilla, lo anoto
				// y guardo a ese nodo para probarlo después
                if(listaAbierta[xdx][ydy] == 0 || (listaAbierta[xdx][ydy] > m0->obtenerPrioridad()) ) {

					if (i%2 == 0) {
						listaAbierta[xdx][ydy] = m0->obtenerPrioridad();
						pN[pNi].push(*m0);
						listaDirecciones[xdx][ydy] = (i+4)%8;
					} else {
						// Si es un movimiento en diagonal, voy a chequear si es válido
						if (mapDeTerreno[xdx - dx[i]][ydy] == validTerrain && mapDeOcupaciones[xdx - dx[i]][ydy] == 0) {
							if (mapDeTerreno[xdx][ydy - dy[i]] == validTerrain && mapDeOcupaciones[xdx][ydy - dy[i]] == 0) {
								listaAbierta[xdx][ydy] = m0->obtenerPrioridad();
								pN[pNi].push(*m0);
								listaDirecciones[xdx][ydy] = (i+4)%8;
							}
						}
					}



				// Si ya existía una alternativa para llegar a dicho nodo, pero la que tengo
				// tiene mejor prioridad, el camino actual es mejor al viejo
				/*
				} else if(listaAbierta[xdx][ydy] > m0->obtenerPrioridad()) {
                    listaAbierta[xdx][ydy] = m0->obtenerPrioridad();
                    listaDirecciones[xdx][ydy] = (i+4)%8;
					pN[pNi].push(*m0);


                    while(!(pN[pNi].top().obtenerPosX() == xdx && pN[pNi].top().obtenerPosY() == ydy)) {                
                        pN[1-pNi].push(pN[pNi].top());
                        pN[pNi].pop();
						if(pN[pNi].empty()){
							cout << "DESENCOLAR ALGO MAGICO by juan" << endl;
							break;
						}
                    }
					if (!pN[pNi].empty())
						pN[pNi].pop();
                    
                    if(pN[pNi].size() > pN[1-pNi].size())
						pNi = 1 - pNi;

                    while(!pN[pNi].empty()) {                
                        pN[1-pNi].push(pN[pNi].top());
                        pN[pNi].pop();       
                    }
                    pNi = 1-pNi;
                    pN[pNi].push(*m0);
					*/

				// Si ya existe un camino mejor a esta casilla, entonces el nodo no sirve
                } else {
					delete m0;
				}
			}
        }
        delete n0;
    }

    return list<Posicion*>();
}