#ifndef NODO_H
#define NODO_H

#pragma once

#include <cmath>
#include <string>
using namespace std;

class Nodo {

	private:
		int xPos;
		int yPos;
		int prioridad;
		int nivel;

	public:
		Nodo(void);
		~Nodo(void);
        Nodo(int xp, int yp, int nivel, int prioridad);
    
        int obtenerPosX() const {return this->xPos;}
        int obtenerPosY() const {return this->yPos;}
        int obtenerPrioridad() const {return this->prioridad;}
		int obtenerNivel() const {return this->nivel;}

        void actualizarPrioridad(const int &xDest, const int &yDest);

        void siguienteNivel(const int &i);
        
        const int &estimar(const int &xDest, const int &yDest);
		//bool operator<(const Nodo& a, const Nodo& b){return a->obtenerPrioridad() > b->obtenerPrioridad();}
	/*	bool operator == (const Nodo & other);
		bool operator >(const Nodo & other);
		bool operator <(const Nodo & other);
		*/
	//	bool operator <(const Nodo & other) const{  return this->obtenerPrioridad() > other.obtenerPrioridad();}
};
#endif // NODO_H
