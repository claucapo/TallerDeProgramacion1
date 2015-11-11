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

class Servidor {
private:
	SDL_sem* eventos_lock;
	queue<struct msg_event> eventos;

	SDL_sem* updates_lock;
	queue<struct msg_update> updates;

	SDL_sem* clientes_lock;
	list<ConexionCliente*> clientes;
	
	SDL_sem* partida_lock;
	Partida* partida;
	
	Servidor() {};
	msg_instancia enviarEntidad(Entidad* ent);

public:
	int cantClientes;
	SOCKET listenSocket; // El socket que se quedar� loopeado en accept


	// Constructor y destructor
	Servidor(SOCKET ls, Partida* partida);
	~Servidor();

	// Inicializa el thread que quedar� a la espera de nuevos clientes
	void start(void);

	void avanzarFrame(void);

	// Lockea la cola de eventos y los procesa
	void procesarEventos(void);
	void enviarUpdates(void);

	// Permite a los clientes agregar un nuevo evento
	void agregarEvento(struct msg_event);
	void agregarUpdate(struct msg_update);

	// Valida una conexi�n entrante y la agrega a la lista de clientes
	void aceptarCliente(SOCKET cs);
	bool validarLogin(struct msg_login);
	void desconectarJugador(unsigned int playerID);

	// Devuelve un c�digo de error <= 0 si no se pudo enviar la informaci�n en alg�n punto
	int enviarMapa(ConexionCliente* cliente);
	// int enviarEntidad(ConexionCliente *cliente, Entidad* ent);
	void Servidor::enviarKeepAlive(void);

	// Funciones para modificar la lista de clientes
	void agregarCliente(ConexionCliente* cliente);
	void removerCliente(ConexionCliente* cliente);
};


#endif // SERVER_H