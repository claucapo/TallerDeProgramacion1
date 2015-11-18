#ifndef SERVER_H
#define SERVER_H

#include <Winsock2.h>
#include "Protocolo.h"
#include "Partida.h"
#include <SDL_mutex.h>
#include <SDL_thread.h>

#include <queue>
#include <list>

class ConexionCliente;

using namespace std;

struct msg_event_ext {
	unsigned int source;
	struct msg_event msg;
};


class Servidor {
private:
	SDL_sem* eventos_lock;
	queue<struct msg_event_ext> eventos;

	//SDL_sem* updates_lock;
	//queue<struct msg_update> updates;

	SDL_sem* clientes_lock;
	list<ConexionCliente*> clientes;
	
	SDL_sem* partida_lock;
	Partida* partida;
	
	Servidor() {};
	msg_instancia enviarEntidad(Entidad* ent);

public:
	int cantClientes;
	SOCKET listenSocket; // El socket que se quedará loopeado en accept


	// Constructor y destructor
	Servidor(SOCKET ls, Partida* partida);
	~Servidor();

	// Inicializa el thread que quedará a la espera de nuevos clientes
	void start(void);

	list<msg_update*> avanzarFrame(void);

	// Lockea la cola de eventos y los procesa
	void procesarEventos(void);
	void enviarUpdates(list<msg_update*> updates);

	// Permite a los clientes agregar un nuevo evento
	void agregarEvento(struct msg_event_ext);
	//void agregarUpdate(struct msg_update);

	// Valida una conexión entrante y la agrega a la lista de clientes
	void aceptarCliente(SOCKET cs);
	bool validarLogin(struct msg_login);
	void desconectarJugador(unsigned int playerID);

	// Devuelve un código de error <= 0 si no se pudo enviar la información en algún punto
	int enviarMapa(ConexionCliente* cliente);
	// int enviarEntidad(ConexionCliente *cliente, Entidad* ent);
	void Servidor::enviarKeepAlive(list<msg_update*> updates);

	// Funciones para modificar la lista de clientes
	void agregarCliente(ConexionCliente* cliente);
	void removerCliente(ConexionCliente* cliente);
};


#endif // SERVER_H