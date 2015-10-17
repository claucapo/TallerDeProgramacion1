#ifndef SERVER_H
#define SERVER_H

#include <Winsock2.h>
#include "Protocolo.h"
#include <SDL_mutex.h>
#include <SDL_thread.h>

#include "ConfigParser.h"

#include <queue>
#include <list>

class ConexionCliente;

using namespace std;

class Servidor {
private:
	ConfigParser* parserInicial;
	SDL_sem* eventos_lock;
	queue<struct msg_event> eventos;

	SDL_sem* updates_lock;
	queue<struct msg_update> updates;

	SDL_sem* clientes_lock;
	list<ConexionCliente*> clientes;
	// Partida* partida
	
	Servidor() {};


public:
	int cantClientes;
	SOCKET listenSocket; // El socket que se quedará loopeado en accept


	// Constructor y destructor
	Servidor(SOCKET ls, ConfigParser* parser);
	~Servidor();

	// Inicializa el thread que quedará a la espera de nuevos clientes
	void start(void);

	// Lockea la cola de eventos y los procesa
	void procesarEventos(void);
	void enviarUpdates(void);

	// Permite a los clientes agregar un nuevo evento
	void agregarEvento(struct msg_event);

	// Valida una conexión entrante y la agrega a la lista de clientes
	void aceptarCliente(SOCKET cs);
	bool validarLogin(struct msg_login);

	// Devuelve un código de error <= 0 si no se pudo enviar la información en algún punto
	int enviarMapa(ConexionCliente* cliente);
	void Servidor::enviarKeepAlive(void);

	// Funciones para modificar la lista de clientes
	void agregarCliente(ConexionCliente* cliente);
	void removerCliente(ConexionCliente* cliente);
};


#endif // SERVER_H