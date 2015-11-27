#pragma once

#include "Matriz.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Enumerados.h"
#include "Protocolo.h"
#include <list>
#include <vector>

#define CANT_ENTIDADES_INI 100
#define RESIZE_AMOUNT 100

/* La clase Escenario representa la cuadricula
de tiles (el mapa) del juego junto con sus
conexiones entre sí. Además, es la encargada
de mantener a las distintas Entidades que
actùan dentro de ese mapa.*/
class Escenario {

private:
	//Atributos
	int cant_entidades;
	int max_entidades;
	vector<Entidad*> entidades;
	// El grafo siguiente es el mapa
	Matriz* mapa;
	int tamX;
	int tamY;
	bool jugadoresDerrotados[MAX_PLAYERS];

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
	int verCantEntidades() {return this->cant_entidades;}

	// Avanza un frame para cada Entidad sobre
	// el mapa, ejecutando sus acciones.
	bool avanzarFrame(unsigned int playerID);

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

	void derrotarJugador(unsigned int defeatedID, tipo_derrota_t accion, Jugador* newOwner);
	
	Entidad* obtenerEntidad(unsigned int entID);

	vector<Entidad*> verEntidades(void);
	void asignarDestino(unsigned int entID, Posicion pos);
	bool jugadorFueDerrotado(unsigned int playerID);
};