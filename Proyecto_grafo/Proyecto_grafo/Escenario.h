#include "Grafo.h"
#include "Posicion.h"
#include "Entidad.h"
#include "Unidad.h"
#pragma once

/* La clase Escenario representa la cuadricula
de tiles (el mapa) del juego junto con sus
conexiones entre sí. Además, es la encargada
de mantener a las distintas Entidades que
actùan dentro de ese mapa.*/
class Escenario
{
private:
	// El grafo siguiente es el mapa
	Grafo mapa;
	// Método interno que genera un mapa de
	// casillas_x x casillas_y vacío a partir
	// de un Grafo.
	void generarMapaVacio(int casillas_x, int casillas_y);
public:
	// Constructor y destructor por defecto
	Escenario(void);
	~Escenario(void);
	
	// Crea un Escenario asociando un mapa
	// Grafo de tamaño casillas_x x casillas_y.
	Escenario(int casillas_x, int casillas_y);

	// Devuelve el Grafo que representa el
	// mapa con sus conexiones.
	Grafo verMapa(){return mapa;} ;

	// Avanza un frame para cada Entidad sobre
	// el mapa, ejecutando sus acciones.
	void avanzarFrame(void);

	// Posiciona una Entidad en el mapa en la
	// Posicion pos. Si la Posicion ya estaba
	// ocupada, no se la ubica. ¿Lanzar excepción?
	// Pre: pos es una Posicion del mapa vacìa.
	void ubicarEntidad(Entidad entidad, Posicion pos);

	// Remueve del mapa la Entidad recibida,
	// quitando sus referencias a cualquier
	// Posicion.
	// Pre: La entidad estaba en el mapa.
	void quitarEntidad(Entidad entidad);

	// Mueve la Unidad recibida en dirección
	// a la Posicion de destino.
	// Pre: La Unidad estaba en el mapa,
	// destino es una Posicion del mapa.
	void moverUnidad(Unidad unidad, Posicion destino);
};

