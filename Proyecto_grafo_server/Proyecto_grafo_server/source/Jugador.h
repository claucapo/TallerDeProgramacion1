#ifndef JUGADOR_H
#define JUGADOR_H

#include "Entidad.h"
#include "Posicion.h"
#include "Enumerados.h"
#include "Vision.h"
#include <list>


struct recursos_jugador_t {
	unsigned int oro;
	unsigned int madera;
	unsigned int comida;
	unsigned int piedra;

	recursos_jugador_t() {
		oro = 0;
		madera = 0;
		comida = 0;
		piedra = 0;
	}

	recursos_jugador_t(costo_t costo) {
		oro = costo.oro;
		madera = costo.madera;
		comida = costo.comida;
		piedra = costo.piedra;
	}
};



/* La clase Jugador representa a cada uno
de los participantes del juego, agrupando
sus disintas Entidades. En un futuro,
agrupar�an tambi�n sus recursos y etc.
NOTA: Actualmente, como el juego s�lo
poseer� una �nica Entidad m�vil, */
class Jugador {
private:
	std::string nombre;
	std::string nombre_def;
	int id;
	std::string color;	// Proximamente cambiar por un enum
					// Con un int inclusive ya basta

	Vision* vision;
	bool esta_conectado;
	
	recursos_jugador_t recursos;
	
	Jugador(void) {};

public:
	bool resources_dirty;

	// Constructor y destructor por defecto
	Jugador(string name, int id, string color);
	~Jugador(void);

	// Getters
	string verNombre(void) {return this->nombre;}
	string verNombreDef(void) {return this->nombre_def;}
	int verID(void) {return this->id;}
	string verColor(void) {return this->color;}
	Vision* verVision(void) {return this->vision;}

	void asingarNombre(string name);
	void resetearNombre(void);

	// Asigna al jugador una vision de tama�o fxc
	void asignarVision(int filas, int columnas);

	void reiniciarVision(void);
	void agregarPosiciones(list<Posicion> posiciones);

	estado_vision_t visionCasilla(Posicion pos);

	// Recursos basicos en un mismo struct (hacer p�blico y eliminar m�todos (?))
	void modificarRecurso(resource_type_t tipoR, int cant);
	recursos_jugador_t verRecurso(void) {return this->recursos;}
	bool puedePagar(recursos_jugador_t costo);
	void gastarRecursos(recursos_jugador_t costo);


	// Devuelve true si la Entidad recibida
	// pertenece a este Jugador o false en
	// caso contrario.
	bool poseeEntidad(Entidad* entidad);
	bool estaConectado(void);
	void settearConexion(bool estadoConexion);

};


#endif // JUGADOR_H