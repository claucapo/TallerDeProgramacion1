#include "Servidor.h"
#include "ErrorLog.h"
#include "ConexionCliente.h"
#include "FactoryEntidades.h"
#include "Partida.h"
#include "Protocolo.h"
#include "Jugador.h"

#include <queue>
#include <list>
#include <sstream>

using namespace std;

// Defino la función de lectura personalizada
int sRead(SOCKET source, char* buffer, int length);

Servidor::Servidor(SOCKET ls, Partida* partida, unsigned int maxJugadores) {
	this->listenSocket = ls;
	this->partida = partida;

	this->eventos = queue<struct msg_event_ext>();
	this->maxClientes = maxJugadores;
	this->send_signal = SDL_CreateCond();

	this->cantClientes = 0;
	this->clientes = list<ConexionCliente*>();
	this->lobby_upd = list<msg_lobby>();
	
	// Creo los semáforos, que sólo permiten el acceso a las colas de a un thread por vez
	this->eventos_lock = SDL_CreateSemaphore(1);
	this->clientes_lock = SDL_CreateSemaphore(1);
	this->partida_lock = SDL_CreateSemaphore(1);
	this->lobby_lock = SDL_CreateSemaphore(1);
}

// Destructor
Servidor::~Servidor(void) {
	SDL_DestroySemaphore(this->clientes_lock);
	SDL_DestroySemaphore(this->eventos_lock);
	SDL_DestroyCond(this->send_signal);

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
			ConexionCliente* cc = server->aceptarCliente(clientSocket);
			if ( cc != nullptr ) {
				server->agregarUpdateLobby(LOBBY_CONNECT, cc->getPlayerID(), cc->getPlayerName());
			}
		}
	}
	return 0;
}



void Servidor::agregarUpdateLobby(CodigoLobby code, unsigned int playerID, char aux[50]) {
	SDL_SemWait(this->lobby_lock);
	msg_lobby newUpd = msg_lobby();
	newUpd.code = code;
	newUpd.playerID = playerID;
	if (aux != nullptr)
		for (int i = 0; i < 50; i++) {
			newUpd.name[i] = aux[i];
		}
	this->lobby_upd.push_back(newUpd);
	SDL_SemPost(this->lobby_lock);
}


// Crea un thread destinado a la recepción de nuevos clientes
void Servidor::start(void) {
	cout << "Empiezo a aceptar clientes" << endl;
	SDL_Thread* conector = SDL_CreateThread(conectorClientes, "Client connector", this);
	bool continuar = true;

	while (continuar) {
		continuar = this->debeAceptarClientes();
		this->agregarUpdateLobby(LOBBY_KEEP_ALIVE, 0, nullptr);
		SDL_SemWait(this->clientes_lock);
		SDL_SemWait(this->lobby_lock);
		while (!this->lobby_upd.empty()) {
			msg_lobby upd = this->lobby_upd.front();
			this->lobby_upd.pop_front();

			list<ConexionCliente*>::const_iterator iter, next;
			for (iter = this->clientes.begin(); iter != this->clientes.end(); iter = next) {
				next = iter;
				next++;

				int result = send((*iter)->clientSocket, (char*)(&upd), sizeof(upd), 0);
				if (result <= 0) {		
					msg_lobby newUpd = msg_lobby();
					newUpd.code = LOBBY_DISCONECT;
					newUpd.playerID = (*iter)->getPlayerID();
					this->lobby_upd.push_back(newUpd);


					printf("Error lobby updates. Terminando conexion\n");
					this->partida->obtenerJugador((*iter)->getPlayerID())->settearConexion(false);
					this->clientes.erase(iter);
					this->cantClientes--;
					printf("Se elimina a un cliente. Clientes activos: %d\n", this->cantClientes);

				}
			}
		}
		SDL_SemPost(this->lobby_lock);
		SDL_SemPost(this->clientes_lock);
		SDL_Delay(20);
	}
	
	SDL_SemWait(this->clientes_lock);
	list<ConexionCliente*>::const_iterator iter, next;
	for (iter = this->clientes.begin(); iter != this->clientes.end(); iter = next) {
		ErrorLog::getInstance()->escribirLog("Envio un start partida");
		next = iter;
		next++;

		msg_lobby newUpd = msg_lobby();
		newUpd.code = LOBBY_START_GAME;

		int result = send((*iter)->clientSocket, (char*)(&newUpd), sizeof(newUpd), 0);
		if (result <= 0) {
			printf("Error lobby updates. Terminando conexion\n");
		}
		(*iter)->start();
	}
	SDL_SemPost(this->clientes_lock);
}


// Función que se llama cada vez que se acepta un nuevo cliente
// Acá se define el protocolo de LOG_IN, y se decide si la 
// solicitud de unirse a la partida es válida
ConexionCliente* Servidor::aceptarCliente(SOCKET clientSocket) {
	// 1) Recibir solicitud de login
	char buffer[sizeof(struct msg_login)];
	int result = sRead(clientSocket, buffer, sizeof(struct msg_login));
	if (result <= 0) {
		printf("Error leyendo solicitud de login. Terminando conexión");
		closesocket(clientSocket);
		return nullptr;
	}
	// Casteo del buffer
	struct msg_login loginInfo = *(struct msg_login*)buffer;


	// 2) Validar solicitud
	DisconectCause_t aceptado = this->validarLogin(loginInfo);
	struct msg_login_response respuesta;

	if (aceptado == KICK_OK) {
		respuesta.ok = true;
		respuesta.cause = KICK_OK;
		// 2.a) Crear al cliente
		ConexionCliente* nuevoCliente = new ConexionCliente(clientSocket, this, loginInfo.playerCode, loginInfo.nombre);

		// 2.b) Contestar solicitud afirmativamente
		result = send(clientSocket, (char*)&respuesta, sizeof(respuesta), 0);
		
		if (result <= 0) {
			printf("Error respondiendo solicitud de login. Terminando conexión");
			closesocket(clientSocket);
			return nullptr;
		}

		// 2.c) Enviar mapa completo
		result = this->enviarMapa(nuevoCliente);
		if (result <= 0) {
			printf("Error enviando el mapa de login. Terminando conexión\n");
			closesocket(clientSocket);
			return nullptr;
		}
				
		
		// 3) Esperar confirmacion
		char buffer2[sizeof(struct msg_client_ready)];
		int result = sRead(clientSocket, buffer2, sizeof(struct msg_client_ready));
		if (result <= 0) {
			printf("Error leyendo ack del cliente. Terminando conexion %d\n", WSAGetLastError());
			closesocket(clientSocket);
			return nullptr;
		}
		
		Jugador* player = this->partida->obtenerJugador(loginInfo.playerCode);
		player->settearConexion(true);

		struct msg_client_ready client_ready = *(struct msg_client_ready*)buffer2;
		if (client_ready.ok) {
			this->agregarCliente(nuevoCliente);
			this->partida->obtenerJugador(nuevoCliente->getPlayerID())->asingarNombre(std::string(loginInfo.nombre));
			return nuevoCliente;
		} else {		
			printf("Error inesperado en el cliente. Terminando conexion");
			closesocket(clientSocket);
			return nullptr;
		}

	} else {
		respuesta.ok = false;
		respuesta.cause = aceptado;
		// 2.a) Contestar solicitud negativamente
		result = send(clientSocket, (char*)&respuesta, sizeof(respuesta), 0);
		if (result <= 0) {
			printf("Error respondiendo solicitud de login. Terminando conexión");
			return nullptr;
		}
		closesocket(clientSocket);
	}
}


// TODO: validar al cliente.
// Esta función debe decidir, en función de los clientes ya conectados
// y de los jugadores activos si la solicitud es válida o no.
// Importante: es posible que tenga que acceder a la lista de clientes
// (aunque no completamente necesario)
DisconectCause_t Servidor::validarLogin(struct msg_login msg) {
	if (msg.playerCode <= 0) {
		printf("Invalid request login.\n");
		return KICK_INVALID_ID;
	} else {
		Jugador* player = this->partida->obtenerJugador(msg.playerCode);
		if (!player)
			return KICK_INVALID_ID;
		
		if (player->estaConectado())
			return KICK_ID_IN_USE;
		
		if (!this->debeAceptarClientes())
			return KICK_FULL;

		if (this->partida->nombreExiste(std::string(msg.nombre)))
			return KICK_NAME_IN_USE;
	}
	return KICK_OK;
}


// Función auxiliar para convertir entidad en mensaje
msg_instancia Servidor::enviarEntidad(Entidad* ent) {
	struct msg_instancia msg;

	int last = ent->verNombre().copy(msg.name, 49, 0);
	msg.name[last] = '\0';

	msg.idEntidad = ent->verID();
	msg.playerCode = ent->verJugador()->verID();
	msg.coordX = ent->verPosicion()->getX();
	msg.coordY = ent->verPosicion()->getY();
	msg.estadoEntidad = ent->verEstado();
		
	return msg;
}

// -------------------------------------------------
// ----------Protocolo de envio de partida----------
// -------------------------------------------------
// TODO: modularizar
int Servidor::enviarMapa(ConexionCliente *cliente) {
	SDL_SemWait(this->partida_lock);
	
	struct msg_map msg;
	msg.coordX = this->partida->escenario->verTamX();
	msg.coordY = this->partida->escenario->verTamY();

	list<msg_tipo_entidad*> tipos = FactoryEntidades::obtenerInstancia()->obtenerListaTipos();
	msg.cantTipos = tipos.size();
	
	list<Entidad*> instancias = this->partida->escenario->verEntidades();
	msg.cantInstancias = instancias.size();

	list<Jugador*> jugadores = this->partida->jugadores;
	msg.cantJugadores = jugadores.size();
	
	// Mando el tamaño del mapa y la cantidad de tipos e instancias
	int result = send(cliente->clientSocket, (char*)&msg, sizeof(msg), 0);
	if (result <= 0) {
		while (!tipos.empty()) {
			delete tipos.front();
			tipos.pop_front();
		}
		tipos.clear();
		SDL_SemPost(this->partida_lock);
		return result;
	}

	// Mando la información de los jugadores
	list<Jugador*>::iterator it;
	for (it = jugadores.begin(); it != jugadores.end(); it++) {
		msg_jugador act;
		int last = (*it)->verNombre().copy(act.name, 49, 0);
		act.name[last] = '\0';

		last = (*it)->verNombreDef().copy(act.name_def, 49, 0);
		act.name_def[last] = '\0';

		last = (*it)->verColor().copy(act.color, 49, 0);
		act.color[last] = '\0';

		act.conectado = (*it)->estaConectado();
		act.id = (*it)->verID();
		struct recursos_jugador_t rj = (*it)->verRecurso();
		act.recursos.comida = rj.comida;
		act.recursos.madera = rj.madera;
		act.recursos.oro = rj.oro;
		act.recursos.piedra = rj.piedra;

		result = send(cliente->clientSocket, (char*)&act, sizeof(act), 0);

	}

	// Mando los tipos
	while (!tipos.empty() && result > 0) {
		msg_tipo_entidad* act = tipos.front();
		tipos.pop_front();
		result = send(cliente->clientSocket, (char*)act, sizeof(*act), 0);
		
		// Acá verifico si hay lista de entrenables
		// En caso positivo el protocolo indica que lo que se mande a continuación
		// tendrá un tamaño de 50 * cant_entrenables * sizeof(char); y que corresponderá
		// a un arreglo de char[50] donde cada elemento es el nombre de la entidad que se puede entrenar.
		if (act->cant_entrenables > 0) {
			cout << "Voy a madnar una lista de entrenables para: " << act->name << " - " << act->cant_entrenables << endl;
			char* entrenables = act->entrenables;
			result = send(cliente->clientSocket, (char*)entrenables, sizeof(char) * 50 * act->cant_entrenables, 0);
			delete[] entrenables;
		}

		delete act;
	}
	if (result <= 0) {
		while (!tipos.empty()) {
			delete tipos.front();
			tipos.pop_front();
		}
		tipos.clear();
		SDL_SemPost(this->partida_lock);
		return result;
	}

	// Envio todas las Entidades
	list<Entidad*>::iterator it2;
	for (it2 = instancias.begin(); it2 != instancias.end(); it2++) {
		msg_instancia msg = enviarEntidad(*it2);
		result = send(cliente->clientSocket, (char*)&msg, sizeof(msg), 0);
		if (result <= 0){
			SDL_SemPost(this->partida_lock);
			return result;
		} else {
			// ErrorLog::getInstance()->escribirLog("Se envio una instancia de entidad.", LOG_ALLWAYS);
		}
	}
	
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
	this->partida->obtenerJugador(client->getPlayerID())->resetearNombre();
	this->clientes.remove(client);
	this->cantClientes--;
	printf("Se elimina a un cliente. Clientes activos: %d\n", this->cantClientes);
	SDL_SemPost(this->clientes_lock);
}


bool Servidor::debeAceptarClientes(void) {
	SDL_SemWait(this->clientes_lock);
	bool acpetar = this->cantClientes < this->maxClientes;
	SDL_SemPost(this->clientes_lock);
	return acpetar;
}


//------------------------------------------------
//----------Funciones de la simulación------------
//------------------------------------------------

// Bloquea la cola de eventos, y procesa todos ellos hasta dejarla vacía
void Servidor::procesarEventos(void) {
	SDL_SemWait(this->eventos_lock);
	// Voy a vaciar la cola de eventos
	while ( !this->eventos.empty() ) {
		struct msg_event_ext act = this->eventos.front();
		this->eventos.pop();

		this->partida->procesarEvento(act.msg, act.source);
	}
	SDL_SemPost(this->eventos_lock);
}

// Encola en la cola de mensajes a enviar de cada cliente
// todos los updates que haya en la cola de updates principal
// Bloquea la lista de updates
void Servidor::enviarUpdates(list<msg_update*> updates) {

	struct msg_update* ka = new msg_update();
	ka->accion = MSJ_KEEP_ALIVE;
	ka->idEntidad = 0;
	updates.push_back(ka);

	while ( !updates.empty() ) {
		struct msg_update* act = updates.front();
		updates.pop_front();

		// Bloqueo la lista de clientes para que nadie la modifico mientras
		// se reparten los mensajes
		SDL_SemWait(this->clientes_lock);
		list<ConexionCliente*>::iterator iter;
		for (iter = this->clientes.begin(); iter != this->clientes.end(); iter++) {
			(*iter)->agregarUpdate(*act);
		}
		SDL_SemPost(this->clientes_lock);

		delete act;
	}

	SDL_CondSignal(this->send_signal);
}


// Permite a los clientes agregar un nuevo evento
void Servidor::agregarEvento(struct msg_event_ext msg) {
	SDL_SemWait(this->eventos_lock);
	this->eventos.push(msg);
	SDL_SemPost(this->eventos_lock);
}


list<msg_update*> Servidor::avanzarFrame(void) {
	SDL_SemWait(partida_lock);
	list<msg_update*> updates = this->partida->avanzarFrame();
	SDL_SemPost(partida_lock);
	return updates;
}


void Servidor::desconectarJugador(unsigned int playerID) {
	SDL_SemWait(this->partida_lock);
	Jugador* player = this->partida->obtenerJugador(playerID);
	if (player) {
		player->settearConexion(false);
		this->partida->escenario->desconectarJugador(playerID);
	}
	SDL_SemPost(this->partida_lock);
}