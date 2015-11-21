#ifndef CONEXION_CLIENTE_H
#define CONEXION_CLIENTE_H

#include <WinSock2.h>
#include "Protocolo.h"
#include <queue>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mutex.h>

using namespace std;

class Servidor;

class ConexionCliente {
private:
	unsigned int playerID;

	// Threads de lectura y escritura
	SDL_Thread* myReader;
	SDL_Thread* mySender;
	
	ConexionCliente() {};

public:
	SOCKET clientSocket;
	Servidor* server;
	bool must_close;
	bool must_send;

	//SDL_semaphore* updates_lock;
	SDL_mutex* updates_lock;
	queue<struct msg_update> updates;

	ConexionCliente(SOCKET cSocket, Servidor* server, unsigned int ID);
	~ConexionCliente();

	unsigned int getPlayerID() {return this->playerID;}

	// Lanzará los threads de lectura y escritura
	void start();
	void stop();

	void agregarUpdate(struct msg_update);

};


#endif // CONEXION_CLIENTE