#ifndef CLIENTE_H
#define CLIENTE_H

#include <WinSock2.h>
#include <SDL_thread.h>
#include <SDL_mutex.h>

#include "Protocolo.h"
#include "Partida.h"
#include "ConfigParser.h"
#include <queue>
#include <list>

enum estado_vision_t;

using namespace std;

// Definición del struct en el que el cliente recibirá el mapa
struct jugador_info {
	msg_jugador jInfo;
	estado_vision_t* varray;
};

struct mapa_inicial {
	struct msg_map mInfo;
	list<msg_tipo_entidad*> tipos;
	list<msg_instancia*> instancias;
	list<jugador_info*> jugadores;
};



class Cliente {
private:
	SDL_sem* updates_lock;
	queue<struct msg_update> updates;


	SDL_Thread* myReader;
	SDL_Thread* mySender;

	Cliente() {};

public:
	SOCKET clientSocket;
	unsigned int playerID;
	bool must_close;

	SDL_sem* eventos_lock;
	queue<struct msg_event> eventos;


	Cliente(SOCKET sock);
	~Cliente();
	// Método inicial para enviar solicitud de login
	// Devuelve true si la solicitud es aceptada por el servidor
	bool login(redInfo_t rInfo);
	bool sendReadySignal(bool ready);

	// Metodo que inicializa los threads de lectura y escritura para que comiencen a leer
	void start();
	void shutdown();

	// Métodos que operan con las colas de updates y eventos
	void agregarEvento(struct msg_event);
	void agregarUpdate(struct msg_update);
	void procesarUpdates(Partida* game, unsigned int actPlayer);
	void generarKeepAlive();
	struct mapa_inicial getEscenario(void);
	// int enviarEventos(void);
};


#endif CLIENTE_H