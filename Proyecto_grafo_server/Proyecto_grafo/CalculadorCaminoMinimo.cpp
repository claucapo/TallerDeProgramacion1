#include "CalculadorCaminoMinimo.h"
#include "Nodo.h"
#include <iostream>
#include <iomanip>
#include <queue>
#include <string>
#include <math.h>
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

string CalculadorCaminoMinimo::calcularCaminoMinimo( int xAct, int yAct, int xDest, int yDest, int** mapDeOcupaciones)
{

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
    for(int y = 0; y < this->columnas ; y++)
    {
        for(int x = 0; x < filas ; x++)
        {
            listaCerrada[x][y] = 0;
            listaAbierta[x][y] = 0;
        }
    }

	n0=new Nodo(xAct, yAct, 0, 0);
    n0->actualizarPrioridad(xDest, yDest);
    pN[pNi].push(*n0);
	if((x>0)&&(y>0))
    listaAbierta[x-1][y-1] = n0->obtenerPrioridad();

    while(!pN[pNi].empty())
	{

        n0 = new Nodo( pN[pNi].top().obtenerPosX(), pN[pNi].top().obtenerPosY(),pN[pNi].top().obtenerNivel(), pN[pNi].top().obtenerPrioridad());

        x = n0->obtenerPosX(); 
		y = n0->obtenerPosY();

        pN[pNi].pop(); 
        listaAbierta[x][y] = 0;
        listaCerrada[x][y] = 1;

        if( x == xDest && y == yDest) 
        {
            string path="";

            while(!(x == xAct && y == yAct))
            {
                j = listaDirecciones[x][y];
                c = '0' + (j+8/2)%8;
				if((c == '1') || (c == '2') || (c == '0'))
					c += 5;
				else
					c -= 3;
                path += c;
				path += '-';
                x+= dx[j];
                y+= dy[j];
            }

            delete n0;
            while(!pN[pNi].empty()) pN[pNi].pop();           
            return path;
        }

        for(int i = 0; i < 8; i++)
        {
            xdx = x + dx[i];
			ydy = y + dy[i];

			// cout << "Pos" << xdx <<"," << ydy <<" ocupada?" << mapDeOcupaciones[xdx][ydy] << endl;
			if(mapDeOcupaciones[xdx][ydy] == 0)
            if(!( xdx < 0 || xdx > filas-1 || ydy < 0 || ydy > columnas-1 || listaCerrada[xdx][ydy] == 1))
            {
                m0 = new Nodo( xdx, ydy, n0->obtenerNivel(), n0->obtenerPrioridad());
                m0->siguienteNivel(i);
                m0->actualizarPrioridad(xDest, yDest);

                if(listaAbierta[xdx][ydy] == 0)
                {
                    listaAbierta[xdx][ydy] = m0->obtenerPrioridad();
                    pN[pNi].push(*m0);
                    listaDirecciones[xdx][ydy] = (i+8/2)%8;
                }
                else if(listaAbierta[xdx][ydy] > m0->obtenerPrioridad())
                {
                    listaAbierta[xdx][ydy] = m0->obtenerPrioridad();
                    listaDirecciones[xdx][ydy] = (i+8/2)%8;

                    while(!(pN[pNi].top().obtenerPosX() == xdx && pN[pNi].top().obtenerPosY() == ydy))
                    {                
                        pN[1-pNi].push(pN[pNi].top());
                        pN[pNi].pop();       
                    }
                    pN[pNi].pop();
                    
                    if(pN[pNi].size() > pN[1-pNi].size())
						pNi = 1 - pNi;

                    while(!pN[pNi].empty())
                    {                
                        pN[1-pNi].push(pN[pNi].top());
                        pN[pNi].pop();       
                    }
                    pNi = 1-pNi;
                    pN[pNi].push(*m0);
                }
                else delete m0;
            }
        }
        delete n0;
    }
    return "";
}
