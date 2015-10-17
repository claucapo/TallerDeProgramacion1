#ifndef CLIENTE_H
#define CLIENTE_H

#include <WinSock2.h>
#include <SDL_thread.h>
#include <SDL_mutex.h>

#include "Protocolo.h"
#include <queue>


using namespace std;

class Cliente {
private:
	SDL_sem* updates_lock;
	queue<struct msg_update> updates;

	SDL_Thread* myReader;
	SDL_Thread* mySender;

	Cliente() {};

public:
	SOCKET clientSocket;

	SDL_sem* eventos_lock;
	queue<struct msg_event> eventos;


	Cliente(SOCKET sock);
	~Cliente();
	// Método inicial para enviar solicitud de login
	// Devuelve true si la solicitud es aceptada por el servidor
	bool login();

	// Metodo que inicializa los threads de lectura y escritura para que comiencen a leer
	void start();
	void shutdown();

	// Métodos que operan con las colas de updates y eventos
	void agregarEvento(struct msg_event);
	void agregarUpdate(struct msg_update);
	void procesarUpdates();

	struct msg_map getEscenario(void);
	// int enviarEventos(void);
};


#endif CLIENTE_H