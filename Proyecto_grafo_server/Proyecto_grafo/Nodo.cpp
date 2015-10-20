#include "Nodo.h"
#include <math.h>
#include <sstream>


/* La clase Nodo representa cada uno de los elementos. 
Esta clase es una base abstracta de la cual derivaran las
unidades genuinas del juego.*/

Nodo::Nodo() {
		this-> xPos = 0;
		this-> yPos = 0;
		this-> prioridad = 0;
		this-> nivel = 0;
}

Nodo::~Nodo(void){
}

Nodo::Nodo(int xp, int yp, int nivel, int prioridad) {
		this-> xPos = xp;
		this-> yPos = yp;
		this-> prioridad = prioridad;
		this-> nivel = nivel;
}

 void Nodo::actualizarPrioridad(const int & xDest, const int & yDest){

        this->prioridad = this->nivel + this->estimar(xDest, yDest)*10;
}

void Nodo::siguienteNivel(const int &i){
             
		this->nivel+= (8 == 8 ? (i%2 == 0 ? 10 : 14):10);
}
        
const int &Nodo::estimar(const int &xDest, const int &yDest){
          static int xd, yd, d;
          xd=xDest-xPos;
          yd=yDest-yPos;        

           d=static_cast<int>(sqrt((double)xd*xd+yd*yd));

            return(d);
}
/*
bool Nodo::operator<(const Nodo& other) {  
	return this->obtenerPrioridad() > other.obtenerPrioridad();
}

bool Nodo::operator>(const Nodo& other){  
	return this->obtenerPrioridad() < other.obtenerPrioridad();
}

bool Nodo::operator==(const Nodo& other){  
	return this->obtenerPrioridad() == other.obtenerPrioridad();
}*/