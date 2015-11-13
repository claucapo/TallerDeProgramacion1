#ifndef ENTIDAD_H
#define ENTIDAD_H

#include "Posicion.h"
#include "Spritesheet.h"
#include "Enumerados.h"

class Jugador;
class Escenario;

/* La clase Entidad representa cualquier
elemento que pueda ubicarse sobre una
Posicion. El Estado asociado a la Entidad
depende de lo que "esté haciendo" ella.
NOTA: Esta clase es una base, y de ella
deberán heredar los "entes" verdaderos.*/
class Entidad {
protected:
	unsigned int id;
	Posicion* pos;
	Spritesheet* sprites;

	Estados_t state;

	// Interaccion
	Accion_t accion;
	
	costo_t costo;
	int trainRate;

	int tamX;
	int tamY;

	// Quizas agregar un struct con la informacion?;
	int rangoVision;

	Jugador* owner;

public:
	unsigned int targetID;
	string name;
	entity_type_t tipo;

	int vidaAct, vidaMax, ataque, defensa;
	int cooldownMax, cooldownAct;
	bool habilidades[CANT_ACCIONES];

	// Destructor por defecto
	~Entidad(void);

	// Constructor sobrecargado
	Entidad(unsigned int id, string name, tipoEntidad_t pType);

	// Devuelve la Posicion sobre la cual
	// está asociada la Entidad.
	// NOTA: Si la Entidad ocupa más de
	// una Posicion, devuelve la Posicion
	// en la que iría el pixel origen.
	Posicion* verPosicion(void) const {return this->pos;}

	string verNombre() {return this->name;}
	// Devuelve el estado
	Estados_t verEstado(void) {return this->state;}

	// OJO con esto!!
	void settearEstado(Estados_t state);

	// Devuelve los dos tamanios
	int verTamX() {return this->tamX;}
	unsigned int verID() {return this->id;}
	int verTamY() {return this->tamY;}
	int verRango() {return this->rangoVision;}
	entity_type_t verTipo() {return this->tipo;}

	Jugador* verJugador() {return this->owner;}

	// Avanza un frame modificando los valores de la entidad
	// Si devuelve true, significa que hay que remover a la entidad el mapa
	virtual af_result_t avanzarFrame(Escenario* scene);

	// Metodos para setear los sprites.
	void asignarSprite(Spritesheet* sp);

	void asignarJugador(Jugador* player);

	// NOTA: Sobrescribir los operadores
	// ==, < y >.
	// "Dos Entidad son iguales si ocupan
	// las mismas casillas y tienen los
	// mismos estados".
	
	// Asigna una Posicion de prepo
	void asignarPos(Posicion* pos);

	bool operator ==(const Entidad other);
	bool operator < (const Entidad& other) const;
	Spritesheet* verSpritesheet(void);
};

#endif ENTIDAD_H