#pragma once
#include "Entidad.h"
#include "Unidad.h"
#include "Posicion.h"

/* La clase Jugador representa a cada uno
de los participantes del juego, agrupando
sus disintas Entidades. En un futuro,
agrupar�an tambi�n sus recursos y etc.
NOTA: Actualmente, como el juego s�lo
poseer� una �nica Entidad m�vil, */
class Jugador
{
public:
	// Constructor y destructor por defecto
	Jugador(void);
	~Jugador(void);

	// Devuelve true si la Entidad recibida
	// pertenece a este Jugador o false en
	// caso contrario.
	bool poseeEntidad(Entidad entidad);

	// Agrega una Entidad al Jugador. Si
	// dicha Entidad ya era del Jugador,
	// no hace nada. �Lanzar excepci�n?
	void agregarEntidad(Entidad entidad);

	// Remueve una Entidad del Jugador.
	// Pre: La Entidad pertenec�a al Jugador.
	// Post: Se quit� la Entidad de la
	// lista de Entidades del Jugador, o bien
	// no se hizo nada si la Entidad no era
	// originalmente del Jugador. �Lanzar excepci�n?
	void quitarEntidad(Entidad entidad);

	// Remueve la Entidad del Jugador que se
	// encuentra en la Posicion pos.
	// Pre: pos tiene una Entidad que pertenece 
	// al Jugador.
	// Post: Se quit� la Entidad de la
	// lista de Entidades del Jugador, o bien
	// no se hizo nada si la Entidad no era
	// originalmente del Jugador o si pos no
	// conten�a una Entidad. �Lanzar excepci�n?
	void quitarEntidad(Posicion pos);
};

