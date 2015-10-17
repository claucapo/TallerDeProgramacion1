#include "Cliente.h"

int sRead(SOCKET source, char* buffer, int length);

#include <queue>
#include <SDL.h>
#include <SDL_thread.h>

using namespace std;

Cliente::Cliente(SOCKET sock) {
	this->clientSocket = sock;

	this->eventos = queue<struct msg_event>();
	this->updates = queue<struct msg_update>();

	this->eventos_lock = SDL_CreateSemaphore(1);
	this->updates_lock = SDL_CreateSemaphore(1);

	this->myReader = NULL;
	this->mySender = NULL;
}

Cliente::~Cliente() {
	SDL_DestroySemaphore(this->eventos_lock);
}

//------------------------------------
//-----------THREADS!!!---------------
//------------------------------------

// El lector de updates
int clientReader( void* data ) {
	Cliente* cliente = (Cliente*)data;
	struct msg_update upd;
	char buffer[sizeof(struct msg_update)];
	int result = 1;
	do {
		result = sRead(cliente->clientSocket, buffer, sizeof(struct msg_update));
		if (result <= 0) {
			printf("Error de recepci�n. Terminando conexi�n");
			cliente->shutdown();
			printf("Se ciera thread de lectura\n");
			return result;
		} else {
			upd = *(struct msg_update*)buffer;
			cliente->agregarUpdate(upd);
		}
	} while (result > 0);
	return result;
}


// El despachador de eventos
int clientSender( void* data ) {
	printf("Se lanza thread envio\n");
	Cliente* cliente = (Cliente*)data;
	int result = 1;
	while (result > 0) {
		SDL_SemWait(cliente->eventos_lock);
		// Si hay un evento, lo envio
		if ( !cliente->eventos.empty() ) {
			struct msg_event ev = cliente->eventos.front();
			cliente->eventos.pop();

			result = send(cliente->clientSocket, (char*)&ev, sizeof(struct msg_event), 0);
			if (result <= 0) {
				printf("Error enviando evento. Terminando conexion\n");
				cliente->shutdown();
			} else {
				printf("Envie un %d\n", ev.idEntidad);
			}
		}
		SDL_SemPost(cliente->eventos_lock);
	}
	printf("Salgo del loop envio\n");
	return result;
}


// M�todo que dispara los threads
void Cliente::start() {
	this->myReader = SDL_CreateThread(clientReader, "A client reader", this);
	this->mySender = SDL_CreateThread(clientSender, "A client sender", this);
}


void Cliente::shutdown() {
	closesocket(this->clientSocket);

	SDL_DetachThread(this->myReader);
	SDL_DetachThread(this->mySender);

}


//------------------------------------
//-----------FIN THREADS--------------
//------------------------------------

// Env�a la solicitud de logueo (con la informaci�n del jugador con el que se est� 
// tratando de entrar a la partida), y espera una respuesta.
bool Cliente::login() {
	// 1) Enviar informaci�n de logueo
	struct msg_login solicitud;
	solicitud.playerCode = 1;
	int result = send(clientSocket, (char*)&solicitud, sizeof(solicitud), 0);
	if (result <= 0) {
		printf("Error enviando solicitud de login. Terminando conexi�n");
		closesocket(this->clientSocket);
		return false;
	}

	// 2) Recibir la respuesta
	char buffer[sizeof(struct msg_login_response)];
	result = sRead(this->clientSocket, buffer, sizeof(struct msg_login_response));
	if (result <= 0) {
		printf("Error recibiendo respuesta al login. Terminando conexi�n");
		closesocket(clientSocket);
		return false;
	}
	struct msg_login_response rsp = *(struct msg_login_response*)buffer;
	if(rsp.ok)
		printf( "Solicitud OK\n");
	return rsp.ok;
}



// Agrega un evento ya generado a la cola de eventos a enviar.
void Cliente::agregarEvento(struct msg_event ev) {
	SDL_SemWait(this->eventos_lock);
	// printf("Voy a mandar un %d\n", ev.something);
	this->eventos.push(ev);
	// printf("Se agrego un evento. Cantidad de eventos enlistados %d\n", this->eventos.size());
	SDL_SemPost(this->eventos_lock);
}


// Agrega un update a la cola para que luego sea procesada
void Cliente::agregarUpdate(struct msg_update upd) {
	SDL_SemWait(this->updates_lock);
	this->updates.push(upd);
	SDL_SemPost(this->updates_lock);
}



// Proces todos los updates que estan en el momento de la invoaci�n
// en la cola. La estructura se bloquea hasta que se acaben los updates.
void Cliente::procesarUpdates() {
	SDL_SemWait(this->updates_lock);
	// printf("Hay %d updates\n", this->updates.size());
	while( !this->updates.empty() ) {
		struct msg_update upd;
		upd = this->updates.front();
		this->updates.pop();

		printf("Recibi un %d\n", upd.idEntidad);
		// this->partida->decodificarUpdate(upd);
	}
	SDL_SemPost(this->updates_lock);
}


struct msg_map Cliente::getEscenario(void){
	int result; 
	char buffer[sizeof(struct msg_map)];
	result = sRead(this->clientSocket, buffer, sizeof(struct msg_map));
	if (result <= 0) {
		printf("Error recibiendo mapa. Terminando conexion\n");
		closesocket(this->clientSocket);
	}
	struct msg_map mapaRecv = *(struct msg_map*)buffer;

	return mapaRecv;
}

// NO USAR
/*
int Cliente::enviarEventos(void) {
	SDL_SemWait(this->eventos_lock);
	msg_event msjAct = this->eventos.front();
	SDL_SemPost(this->eventos_lock);
	return 0;
}*/
