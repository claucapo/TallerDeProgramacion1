#include "Servidor.h"
#include "ConexionCliente.h"
#include "Partida.h"
#include "Protocolo.h"
#include "Jugador.h"

#include <queue>
#include <list>
using namespace std;

// Defino la función de lectura personalizada
int sRead(SOCKET source, char* buffer, int length);

Servidor::Servidor(SOCKET ls, Partida* partida) {
	this->listenSocket = ls;
	this->partida = partida;

	this->eventos = queue<struct msg_event>();
	this->updates = queue<struct msg_update>();

	this->cantClientes = 0;
	this->clientes = list<ConexionCliente*>();
	
	// Creo los semáforos, que sólo permiten el acceso a las colas de a un thread por vez
	this->eventos_lock = SDL_CreateSemaphore(1);
	this->updates_lock = SDL_CreateSemaphore(1);
	this->clientes_lock = SDL_CreateSemaphore(1);
	this->partida_lock = SDL_CreateSemaphore(1);
}


// Destructor
Servidor::~Servidor(void) {
	SDL_DestroySemaphore(this->clientes_lock);
	SDL_DestroySemaphore(this->eventos_lock);
	SDL_DestroySemaphore(this->updates_lock);
	SDL_DestroySemaphore(this->partida_lock);

}


//----------------------------------------------
//---------Thread que acepta clientes-----------
//----------------------------------------------

// Función principal del thread
int conectorClientes ( void* data ) {
	Servidor* server = (Servidor*)data;
	while (true) {
		SOCKET clientSocket = accept(server->listenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
		} else {
			server->aceptarCliente(clientSocket);
		}
	}
}

// Crea un thread destinado a la recepción de nuevos clientes
void Servidor::start(void) {
	SDL_Thread* conector = SDL_CreateThread(conectorClientes, "Client connector", this);
}


// Función que se llama cada vez que se acepta un nuevo cliente
// Acá se define el protocolo de LOG_IN, y se decide si la 
// solicitud de unirse a la partida es válida
void Servidor::aceptarCliente(SOCKET clientSocket) {
	// 1) Recibir solicitud de login
	char buffer[sizeof(struct msg_login)];
	int result = sRead(clientSocket, buffer, sizeof(struct msg_login));
	if (result <= 0) {
		printf("Error leyendo solicitud de login. Terminando conexión");
		closesocket(clientSocket);
		return;
	}
	// Casteo del buffer
	struct msg_login loginInfo = *(struct msg_login*)buffer;


	// 2) Validar solicitud
	bool aceptado = this->validarLogin(loginInfo);
	struct msg_login_response respuesta;
	respuesta.ok = aceptado;

	if (aceptado) {		
		// 2.a) Crear al cliente
		ConexionCliente* nuevoCliente = new ConexionCliente(clientSocket, this);
		this->agregarCliente(nuevoCliente);
		nuevoCliente->start();

		// 2.b) Contestar solicitud afirmativamente
		result = send(clientSocket, (char*)&respuesta, sizeof(respuesta), 0);
		
		if (result <= 0) {
			printf("Error respondiendo solicitud de login. Terminando conexión");
			this->removerCliente(nuevoCliente);
			closesocket(clientSocket);
			return;
		}

		// 2.c) Enviar mapa completo
		result = this->enviarMapa(nuevoCliente);
		if (result <= 0) {
			printf("Error respondiendo solicitud de login. Terminando conexión");
			this->removerCliente(nuevoCliente);
			closesocket(clientSocket);
			return;
		}
		// Envio todas las Entidades
		list<Entidad*> listaEnt = this->partida->escenario->verEntidades();
		list<Entidad*>::iterator i;
		for (i = listaEnt.begin(); i != listaEnt.end(); i++) {
			this->enviarEntidad(nuevoCliente, (*i));
		}

		// Update de login
		msg_update login_update;
		login_update.accion = MSJ_JUGADOR_LOGIN;
		login_update.idEntidad = (unsigned int) loginInfo.playerCode;
		this->agregarUpdate(login_update);
		
		Jugador* player = this->partida->obtenerJugador(loginInfo.playerCode);
		player->settearConexion(true);

	} else {
		// 2.a) Contestar solicitud negativamente
		result = send(clientSocket, (char*)&respuesta, sizeof(respuesta), 0);
		if (result <= 0) {
			printf("Error respondiendo solicitud de login. Terminando conexión");
			return;
		}
		closesocket(clientSocket);
	}
}


// TODO: validar al cliente.
// Esta función debe decidir, en función de los clientes ya conectados
// y de los jugadores activos si la solicitud es válida o no.
// Importante: es posible que tenga que acceder a la lista de clientes
// (aunque no completamente necesario)
bool Servidor::validarLogin(struct msg_login msg) {
	if (msg.playerCode <= 0) {
		printf("Invalid request login.\n");
		return false;
	} else {
		Jugador* player = this->partida->obtenerJugador(msg.playerCode);
		if (!player || player->estaConectado() || ( player->verNombre().c_str() == msg.nombre ) )
			return false;
	}
	return true;
}


// TODO: definir protocolo de envio de mapa
int Servidor::enviarMapa(ConexionCliente *cliente) {
	struct msg_map msg;
	msg.coordX = this->partida->escenario->verTamX();
	msg.coordY = this->partida->escenario->verTamY();
	SDL_SemWait(this->partida_lock);
	int result = send(cliente->clientSocket, (char*)&msg, sizeof(msg), 0);
	SDL_SemPost(this->partida_lock);
	return result;
}

int Servidor::enviarEntidad(ConexionCliente *cliente, Entidad* ent) {
	struct msg_entidad msg;
	SDL_SemWait(this->partida_lock);

	msg.idEntidad = ent->verID();
	msg.playerCode = ent->verJugador()->verID();
	msg.coordX = ent->verPosicion()->getX();
	msg.coordY = ent->verPosicion()->getY();
	msg.estadoEntidad = ent->verEstado();

	int result = send(cliente->clientSocket, (char*)&msg, sizeof(msg), 0);

	SDL_SemPost(this->partida_lock);
	return result;
}


//----------------------------------------------
//----------Fin del aceptor clientes------------
//----------------------------------------------


// Agrego un cliente a la lista de clientes. (Bloqueante de la lista de clientes)
void Servidor::agregarCliente(ConexionCliente* client) {
	SDL_SemWait(this->clientes_lock);
	this->clientes.push_back(client);
	this->cantClientes++;
	printf("Se acepta a un nuevo cliente\n");
	printf("Clientes activos: %d\n", this->cantClientes);
	SDL_SemPost(this->clientes_lock);
}

// Remuevo un cliente. (Idem agregarCliente)
// ESTA FUNCION ESTA MAL... LO CAMBIE A PUNTEROS Y NO LA MODIFIQUÉ
void Servidor::removerCliente(ConexionCliente* client) {
	SDL_SemWait(this->clientes_lock);
	this->clientes.remove(client);
	this->cantClientes--;
	printf("Se elimina a un nuevo cliente\n");
	printf("Clientes activos: %d\n", this->cantClientes);
	SDL_SemPost(this->clientes_lock);
}


//------------------------------------------------
//----------Funciones de la simulación------------
//------------------------------------------------

void Servidor::enviarKeepAlive() {
	struct msg_update ka;
	ka.idEntidad = 0;
	this->updates.push(ka);
}

// Bloquea la cola de eventos, y procesa todos ellos hasta dejarla vacía
void Servidor::procesarEventos(void) {
	this->enviarKeepAlive();
	SDL_SemWait(this->eventos_lock);
	// Voy a vaciar la cola de eventos
	while ( !this->eventos.empty() ) {
		struct msg_event act = this->eventos.front();
		this->eventos.pop();

		this->partida->procesarEvento(act);
	}
	SDL_SemPost(this->eventos_lock);
}

// Encola en la cola de mensajes a enviar de cada cliente
// todos los updates que haya en la cola de updates principal
// Bloquea la lista de updates
void Servidor::enviarUpdates(void) {
	SDL_SemWait(this->updates_lock);
	while ( !this->updates.empty() ) {
		struct msg_update act = this->updates.front();
		this->updates.pop();

		// Bloqueo la lista de clientes para que nadie la modifico mientras
		// se reparten los mensajes
		SDL_SemWait(this->clientes_lock);
		list<ConexionCliente*>::iterator iter;
		for (iter = this->clientes.begin(); iter != this->clientes.end(); iter++) {
			(*iter)->agregarUpdate(act);
		}
		SDL_SemPost(this->clientes_lock);
	}
	SDL_SemPost(this->updates_lock);
}


// Permite a los clientes agregar un nuevo evento
void Servidor::agregarEvento(struct msg_event msg) {
	SDL_SemWait(this->eventos_lock);
	this->eventos.push(msg);
	SDL_SemPost(this->eventos_lock);
}

void Servidor::agregarUpdate(struct msg_update msg) {
	SDL_SemWait(this->updates_lock);
	this->updates.push(msg);
	SDL_SemPost(this->updates_lock);
}

void Servidor::avanzarFrame(void) {
	SDL_SemWait(partida_lock);
	list<msg_update*> updates = this->partida->avanzarFrame();
	while (!updates.empty()) {
		msg_update* msg = updates.front();
		updates.pop_front();

		msg_update toSend = *msg;
		delete msg;

		this->agregarUpdate(toSend);
	}
	SDL_SemPost(partida_lock);
}