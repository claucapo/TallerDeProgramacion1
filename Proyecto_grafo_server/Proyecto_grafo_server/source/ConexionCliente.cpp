#include "ConexionCliente.h"
#include "Servidor.h"
#include "ErrorLog.h"

#include <sstream>

#include <queue>
#include <time.h>


// Función de lectura auxiliar
int sRead(SOCKET source, char* buffer, int length);

ConexionCliente::ConexionCliente(SOCKET cSocket, Servidor* server, unsigned int id) {
	this->clientSocket = cSocket;
	
	this->playerID = id;

	this->updates = queue<struct msg_update>();
	this->updates_lock = SDL_CreateMutex();

	this->server = server;
	this->must_send = false;


	this->myReader = NULL;
	this->mySender = NULL;
}

ConexionCliente::~ConexionCliente() {
	closesocket(this->clientSocket);
	SDL_DestroyMutex(this->updates_lock);
}


//---------------------------------------
//---------THREAD DE LECTURA-------------
//---------------------------------------

int conexionReader( void* data ) {
	ConexionCliente* cliente = (ConexionCliente*)data;
	struct msg_event msg;
	char buffer[sizeof(struct msg_event)];
	int result = 1;
	do {
		result = sRead(cliente->clientSocket, buffer, sizeof(struct msg_event));
		if (result <= 0) {
			printf("Error de recepcion. Terminando conexion %d\n", WSAGetLastError());
			cliente->stop();
			printf("Exited Reading for cliente\n");
			return result;
		} else {
			msg = *(struct msg_event*)buffer;
			struct msg_event_ext msg_ext;
			msg_ext.msg = msg;
			msg_ext.source = cliente->getPlayerID();
			cliente->server->agregarEvento(msg_ext);
		}
	} while (result > 0 && !cliente->must_close);
	return result;
}


//------------------------------------------
//----------THREAD DE ESCRITURA-------------
//------------------------------------------

int conexionSender( void* data ) {
	ConexionCliente* cliente = (ConexionCliente*)data;
	int result = 1;
	while (result > 0 && !cliente->must_close) {

		SDL_LockMutex(cliente->updates_lock);
		if (cliente->updates.empty())
			SDL_CondWait(cliente->server->send_signal, cliente->updates_lock);

		while ( !cliente->updates.empty() ) {

			struct msg_update act = cliente->updates.front();
			cliente->updates.pop();

			result = send(cliente->clientSocket, (char*)(&act), sizeof(act), 0);
			if (result <= 0) {
				printf("Error enviando updates. Terminando conexion\n");
				cliente->stop();
				
				printf("Exited Sending for cliente\n");
				return result;
			}
		}
		
		SDL_UnlockMutex(cliente->updates_lock);
	}
	printf("Exited Sending for cliente\n");
	return result;
}

// Lanza los dos threads correspondientes a la
// lectura y la escritura
void ConexionCliente::start() {
	DWORD timeout =  3000;
	if (setsockopt(this->clientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout, sizeof(timeout)))
		printf("Error on setting timeout");

	if (setsockopt(this->clientSocket,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout, sizeof(timeout)))
		printf("Error on setting timeout");

	this->must_close = false;

	this->myReader = SDL_CreateThread(conexionReader, "A client reader", this);
	this->mySender = SDL_CreateThread(conexionSender, "A client sender", this);
}

// Desasocia los threads y libera el socket
void ConexionCliente::stop() {
	if (!this->must_close) {
		this->server->removerCliente(this);
		this->server->desconectarJugador(this->playerID);
		SDL_DetachThread(this->myReader);
		SDL_DetachThread(this->mySender);
	}
	this->must_close = true;
	
	// this->server->desconectarJugador(this->playerID);
			
	// Aca estaban los detach antes
}


// Agrega un update a la cola de updates por mandar
// Bloquea la cola para que nadie más acceda al mismo tiempo
void ConexionCliente::agregarUpdate(struct msg_update msg) {
	SDL_LockMutex(this->updates_lock);
	this->updates.push(msg);
	SDL_UnlockMutex(this->updates_lock);
}