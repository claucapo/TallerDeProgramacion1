#pragma once

#include "Matriz.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Protocolo.h"
#include <list>

/* La clase Escenario representa la cuadricula
de tiles (el mapa) del juego junto con sus
conexiones entre sí. Además, es la encargada
de mantener a las distintas Entidades que
actùan dentro de ese mapa.*/
class Escenario {

private:
	//Atributos
	list<Entidad*> entidades;
	// El grafo siguiente es el mapa
	Matriz* mapa;
	int tamX;
	int tamY;

public:
	// Constructor y destructor por defecto
	Escenario(void);
	~Escenario(void);
	
	// Crea un Escenario asociando un mapa
	// Grafo de tamaño casillas_x x casillas_y.
	Escenario(int casillas_x, int casillas_y);

	// Devuelve la matriz que representa el
	// mapa.
	Matriz* verMapa(){return mapa;} ;

	// Metodos que permiten ver el tamanio
	int verTamX() {return this->tamX;}
	int verTamY() {return this->tamY;}

	// Avanza un frame para cada Entidad sobre
	// el mapa, ejecutando sus acciones.
	void avanzarFrame(void);

	bool casillaEstaVacia(Posicion* pos) {return this->mapa->posicionEstaVacia(pos);}

	// Posiciona una Entidad en el mapa en la
	// Posicion pos. Si la Posicion ya estaba
	// ocupada, no se la ubica. ¿Lanzar excepción?
	// Pre: pos es una Posicion del mapa vacìa.
	bool ubicarEntidad(Entidad* entidad, Posicion* pos);

	// Remueve del mapa la Entidad recibida,
	// quitando sus referencias a cualquier
	// Posicion.
	// Pre: La entidad estaba en el mapa.
	void quitarEntidad(Entidad* entidad);

	void quitarEntidad(unsigned int entID);

	void moverEntidad(unsigned int entID, Posicion* pos, bool seguirMoviendo);
	
	list<Entidad*> verEntidades(void);
	void asignarDestino(unsigned int entID, Posicion pos);

};