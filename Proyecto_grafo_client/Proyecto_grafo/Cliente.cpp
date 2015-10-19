#include "Cliente.h"
#include "ErrorLog.h"
#include "ConfigParser.h"

int sRead(SOCKET source, char* buffer, int length);

#include <queue>
#include <SDL.h>
#include <SDL_thread.h>

using namespace std;

Cliente::Cliente(SOCKET sock) {
	this->clientSocket = sock;
	this->playerID = 0;

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
			printf("Error de recepción. Terminando conexión");
			cliente->shutdown();
			printf("Se ciera thread de lectura\n");
			return result;
		} else {
			upd = *(struct msg_update*)buffer;
			cliente->agregarUpdate(upd);
		}
	} while (result > 0);

					SDL_Delay(10);

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
				// printf("Envie un %d\n", ev.idEntidad);
			}
		}
		SDL_SemPost(cliente->eventos_lock);
	}
	printf("Salgo del loop envio\n");
				
	SDL_Delay(10);

	return result;
}


// Método que dispara los threads
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

// Envía la solicitud de logueo (con la información del jugador con el que se está 
// tratando de entrar a la partida), y espera una respuesta.
bool Cliente::login(redInfo_t rInfo) {
	// 1) Enviar información de logueo
	struct msg_login solicitud;
	solicitud.playerCode = rInfo.ID;

	int last = rInfo.name.copy(solicitud.nombre, 49, 0);
	solicitud.nombre[last] = '\0';

	int result = send(clientSocket, (char*)&solicitud, sizeof(solicitud), 0);
	if (result <= 0) {
		printf("Error enviando solicitud de login. Terminando conexión");
		closesocket(this->clientSocket);
		return false;
	}

	// 2) Recibir la respuesta
	char buffer[sizeof(struct msg_login_response)];
	result = sRead(this->clientSocket, buffer, sizeof(struct msg_login_response));
	if (result <= 0) {
		printf("Error recibiendo respuesta al login. Terminando conexión");
		closesocket(clientSocket);
		return false;
	}
	struct msg_login_response rsp = *(struct msg_login_response*)buffer;
	if(rsp.ok) {
		printf( "Solicitud OK\n");
		this->playerID = solicitud.playerCode;
	}
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



// Proces todos los updates que estan en el momento de la invoación
// en la cola. La estructura se bloquea hasta que se acaben los updates.
void Cliente::procesarUpdates(Partida* game) {
	SDL_SemWait(this->updates_lock);
	// printf("Hay %d updates\n", this->updates.size());
	while( !this->updates.empty() ) {
		struct msg_update upd;
		upd = this->updates.front();
		this->updates.pop();
		
		if (upd.accion == MSJ_MOVER)
			printf("Leego un update!\n");
		// printf("Recibi un %d\n", upd.idEntidad);
		game->procesarUpdate(upd);
	}
	SDL_SemPost(this->updates_lock);
}


struct mapa_inicial Cliente::getEscenario(void) {
	int result;
	struct mapa_inicial scene_info;
	result = sRead(this->clientSocket, (char*)&(scene_info.mInfo), sizeof(scene_info.mInfo));
	if ( result <= 0 ) {
		ErrorLog::getInstance()->escribirLog("Error recibiendo mapa.", LOG_ERROR);
		return scene_info;
	}


	// Se la cantidad de jugadores que voy a recibir, y el tamaño de sus visiones
	for (int i = 0; i < scene_info.mInfo.cantJugadores; i++) {
		jugador_info* jugador_act = new jugador_info();
		result = sRead(this->clientSocket, (char*)&(jugador_act->jInfo), sizeof(jugador_act->jInfo));
		if ( result <= 0 ) {
			ErrorLog::getInstance()->escribirLog("Error recibiendo jugador.", LOG_ERROR);
			return scene_info;
		}
		estado_vision_t* varray = new estado_vision_t[scene_info.mInfo.coordX * scene_info.mInfo.coordY];
		result = sRead(this->clientSocket, (char*)varray, scene_info.mInfo.coordX * scene_info.mInfo.coordY * sizeof(estado_vision_t));
		if ( result <= 0 ) {
			ErrorLog::getInstance()->escribirLog("Error recibiendo vision.", LOG_ERROR);
			return scene_info;
		}
		jugador_act->varray = varray;
		scene_info.jugadores.push_back(jugador_act);
	}


	// Se la cantidad de tipos que voy a recibir
	for (int i = 0; i < scene_info.mInfo.cantTipos; i++) {
		msg_tipo_entidad* tipo_act = new msg_tipo_entidad();
		result = sRead(this->clientSocket, (char*)(tipo_act), sizeof(*tipo_act));
		if ( result <= 0 ) {
			ErrorLog::getInstance()->escribirLog("Error recibiendo tipo.", LOG_ERROR);
			return scene_info;
		}

		scene_info.tipos.push_back(tipo_act);
	}

	// Se la cantidad de instancais que voy a recibir
	for (int i = 0; i < scene_info.mInfo.cantInstancias; i++) {
		msg_instancia* inst_act = new msg_instancia();
		result = sRead(this->clientSocket, (char*)(inst_act), sizeof(*inst_act));
		if ( result <= 0 ) {
			ErrorLog::getInstance()->escribirLog("Error recibiendo instancia.", LOG_ERROR);
			return scene_info;
		}

		scene_info.instancias.push_back(inst_act);
	}

	return scene_info;
}


/*
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
*/

// NO USAR
/*
int Cliente::enviarEventos(void) {
	SDL_SemWait(this->eventos_lock);
	msg_event msjAct = this->eventos.front();
	SDL_SemPost(this->eventos_lock);
	return 0;
}*/
