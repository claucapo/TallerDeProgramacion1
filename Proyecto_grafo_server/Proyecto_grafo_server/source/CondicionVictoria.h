#ifndef COND_VIC_H
#define COND_VIC_H

#include <vector>
#include "Enumerados.h"
#include "Jugador.h"
struct Partida;
class Escenario;
class Entidad;

class CondicionVictoria {
private:
	vector<unsigned int> cantUnidadesCriticas;
	vector<unsigned int> ultimoAtacante;
	vector<bool> sigueJugando;
	int tipoUnidadCritica;
	tipo_derrota_t accionAlPerder;


public:
	int cantJugadores;
	vector<Jugador*> jugadores;
	
	CondicionVictoria() {};
	CondicionVictoria(int cantJugadores, unsigned int tipoUnidadCritica, tipo_derrota_t accion);
	~CondicionVictoria() {};

	void inicializar(Partida* game);

	void spawnEntidad(Entidad* ent);
	
	// Devuelve true si el jugador dueño de la entidad borrada
	// ha perdido el juego
	bool deleteEntidad(Entidad* ent);

	int cantJugadoresActivos(void);
	unsigned int verUltimoJugador(void);

	void derrotarJugador(unsigned int ID) {this->sigueJugando[ID] = false;}
	
	tipo_derrota_t verTipoDerrota(void) {return this->accionAlPerder;}
	unsigned int verUltimoAtacante(unsigned int ID) {return this->ultimoAtacante[ID];}
	unsigned int verCantUnidadesCrit(unsigned int ID) {return this->cantUnidadesCriticas[ID];}
	bool verSigueJugando(unsigned int ID) {return this->sigueJugando[ID];}

};





#endif // COND_VIC_H