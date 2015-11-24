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
	
	SDL_sem* clientes_lock;
	list<ConexionCliente*> clientes;
	
	SDL_sem* partida_lock;
	Partida* partida;
	
	Servidor() {};
	msg_instancia enviarEntidad(Entidad* ent);

	SDL_sem* lobby_lock;
	list<msg_lobby> lobby_upd;
	
	int cantClientes;
	int maxClientes;

public:
	SDL_cond* send_signal;
	SOCKET listenSocket; // El socket que se quedará loopeado en accept


	// Constructor y destructor
	Servidor(SOCKET ls, Partida* partida, unsigned int maxJugadores = 1);
	~Servidor();

	// Inicializa el thread que quedará a la espera de nuevos clientes
	void start(void);

	list<msg_update*> avanzarFrame(void);

	// Lockea la cola de eventos y los procesa
	void procesarEventos(void);
	void enviarUpdates(list<msg_update*> updates);

	// Permite a los clientes agregar un nuevo evento
	void agregarEvento(struct msg_event_ext);

	// Valida una conexión entrante y la agrega a la lista de clientes
	ConexionCliente* aceptarCliente(SOCKET cs);
	DisconectCause_t validarLogin(struct msg_login);
	void desconectarJugador(unsigned int playerID);

	// Devuelve un código de error <= 0 si no se pudo enviar la información en algún punto
	int enviarMapa(ConexionCliente* cliente);

	// Funciones para modificar la lista de clientes
	void agregarCliente(ConexionCliente* cliente);
	void removerCliente(ConexionCliente* cliente);
	bool debeAceptarClientes(void);

	// Lobby specific
	void agregarUpdateLobby(CodigoLobby code, unsigned int playerID, char aux[50]);
};


#endif // SERVER_H