#ifndef JUGADOR_H
#define JUGADOR_H

#include "Entidad.h"
#include "Posicion.h"
#include "Vision.h"
#include <list>


/* La clase Jugador representa a cada uno
de los participantes del juego, agrupando
sus disintas Entidades. En un futuro,
agruparían también sus recursos y etc.
NOTA: Actualmente, como el juego sólo
poseerá una única Entidad móvil, */
class Jugador {
private:
	string nombre;
	Vision* vision;
	int recurso;
	bool esta_conectado;
	
	Jugador(void) {};

public:
	// Constructor y destructor por defecto
	Jugador(string name);
	~Jugador(void);

	// Getters
	string verNombre(void) {return this->nombre;}
	Vision* verVision(void) {return this->vision;}
	int verRecurso(void) {return this->recurso;}

	// Asigna al jugador una vision de tamaño fxc
	void asignarVision(int filas, int columnas);

	void reiniciarVision(void);
	void agregarPosiciones(list<Posicion> posiciones);

	estado_vision_t visionCasilla(Posicion pos);

	// Recurso básico para probar
	void modificarRecurso(int cant) {this->recurso += cant;}

	// Devuelve true si la Entidad recibida
	// pertenece a este Jugador o false en
	// caso contrario.
	bool poseeEntidad(Entidad* entidad);
	bool estaConectado(void);
	void settearConexion(bool estadoConexion);
};


#endif // JUGADOR_H