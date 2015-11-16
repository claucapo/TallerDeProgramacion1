#pragma once

#include "Matriz.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Protocolo.h"
#include <list>

/* La clase Escenario representa la cuadricula
de tiles (el mapa) del juego junto con sus
conexiones entre s�. Adem�s, es la encargada
de mantener a las distintas Entidades que
act�an dentro de ese mapa.*/
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
	
	list<msg_update*> updatesAux;

	// Crea un Escenario asociando un mapa
	// Grafo de tama�o casillas_x x casillas_y.
	Escenario(int casillas_x, int casillas_y);

	// Devuelve la matriz que representa el
	// mapa.
	Matriz* verMapa(){return mapa;} ;

	// Metodos que permiten ver el tamanio
	int verTamX() {return this->tamX;}
	int verTamY() {return this->tamY;}

	// Avanza un frame para cada Entidad sobre
	// el mapa, ejecutando sus acciones.
	list<msg_update*> avanzarFrame(void);
	msg_update* generarUpdate(CodigoMensaje accion, unsigned int id, float extra1, float extra2);


	bool casillaEstaVacia(Posicion* pos) {return this->mapa->posicionEstaVacia(pos);}
	Entidad* obtenerEntidad(unsigned int entID);
	Entidad* obtenerEntidad(Posicion pos);

	// Posiciona una Entidad en el mapa en la
	// Posicion pos. Si la Posicion ya estaba
	// ocupada, no se la ubica. �Lanzar excepci�n?
	// Pre: pos es una Posicion del mapa vac�a.
	bool ubicarEntidad(Entidad* entidad, Posicion* pos);

	// Remueve del mapa la Entidad recibida,
	// quitando sus referencias a cualquier
	// Posicion.
	// Pre: La entidad estaba en el mapa.
	void quitarEntidad(Entidad* entidad);

	void moverEntidad(Entidad* entidad, Posicion* destino);

	// Busca y devuelve la primer casilla de las m�s cercanas a lo que haya
	// en origen, que se encuentre libre. Si el mapa est� lleno, devuelve null.
	Posicion* obtenerPosicionSpawn(Posicion* origen);

	list<Entidad*> verEntidades(void);
	void asignarDestino(unsigned int entID, Posicion pos);

};