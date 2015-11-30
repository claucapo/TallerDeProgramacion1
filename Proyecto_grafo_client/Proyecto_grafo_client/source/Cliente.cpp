#include "Cliente.h"
#include "ErrorLog.h"
#include "Jugador.h"
#include "ConfigParser.h"

int sRead(SOCKET source, char* buffer, int length);

#include <queue>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mixer.h>
#include "BibliotecaDeImagenes.h"

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

	DWORD timeout =  5000;
	if (setsockopt(this->clientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout, sizeof(timeout)))
		printf("Error on setting timeout");

	if (setsockopt(this->clientSocket,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout, sizeof(timeout)))
		printf("Error on setting timeout");
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
	} while (result > 0 && !cliente->must_close);
	printf("Se ciera thread de lectura\n");
	return result;
}


// El despachador de eventos
int clientSender( void* data ) {
	printf("Se lanza thread envio\n");
	Cliente* cliente = (Cliente*)data;
	int result = 1;
	while (result > 0 && !cliente->must_close) {
		SDL_SemWait(cliente->eventos_lock);
		// Si hay un evento, lo envio
		if ( !cliente->eventos.empty() ) {
			struct msg_event ev = cliente->eventos.front();
			cliente->eventos.pop();
			result = send(cliente->clientSocket, (char*)&ev, sizeof(struct msg_event), 0);
			if (result <= 0) {
				printf("Error enviando evento. Terminando conexion\n");
				cliente->shutdown();
			}
		}
		SDL_SemPost(cliente->eventos_lock);
	}
	printf("Salgo del loop envio\n");
	return result;
}


bool Cliente::startLobby(Partida* game) {
	int result;
	Jugador* player;
	char buffer[sizeof(struct msg_lobby)];
	do {
		result = sRead(this->clientSocket, buffer, sizeof(struct msg_lobby));
		if (result <= 0) {
			printf("Error de recepci�n. Terminando conexi�n");
			closesocket(this->clientSocket);
			return false;
		} else {
			msg_lobby upd = *(struct msg_lobby*)buffer;
			// Procesar upd...
			switch (upd.code) {
			case LOBBY_START_GAME:
				cout << "Recibi un LOBBY_START_GAME" << endl;
				return true;

			case LOBBY_CONNECT:
				cout << "Recibi un LOBBY_CONNECT" << endl;
				player = game->obtenerJugador(upd.playerID);
				if (player) {
					player->settearConexion(true);
					player->asingarNombre(string(upd.name));
				}
				break;

			case LOBBY_DISCONECT:
				cout << "Recibi un LOBBY_DISCONECT" << endl;
				player = game->obtenerJugador(upd.playerID);
				if (player) {
					player->settearConexion(false);
					player->resetearNombre();
				}
				break;
			}
			if (upd.code != LOBBY_KEEP_ALIVE) {
				cout << endl << "----------- LOBBY STATUS ----------" << endl;
				for (int i = 1; i < game->jugadores.size(); i++) {
					player = game->obtenerJugador(i);
					if (player) {
						cout << "Player: " << i << " - ";
						if (player->estaConectado())
							cout << "CONECTADO - ";
						else
							cout << "DESCONECTADO - ";

						cout << "Alias: [" << player->verNombre() << "]";
						if (player->verID() == this->playerID)
							cout << " (YOU)";
						cout << endl;
					}
				}
				cout << endl;
			}

		}
	} while (result > 0);
	return false;
}


// M�todo que dispara los threads
void Cliente::start() {
	this->must_close = false;

	this->myReader = SDL_CreateThread(clientReader, "A client reader", this);
	this->mySender = SDL_CreateThread(clientSender, "A client sender", this);
}


void Cliente::shutdown() {
	this->must_close = true;

	closesocket(this->clientSocket);

	SDL_DetachThread(this->myReader);
	SDL_DetachThread(this->mySender);

}


//------------------------------------
//-----------FIN THREADS--------------
//------------------------------------

// Env�a la solicitud de logueo (con la informaci�n del jugador con el que se est� 
// tratando de entrar a la partida), y espera una respuesta.
bool Cliente::login(redInfo_t rInfo) {
	// 1) Enviar informaci�n de logueo
	struct msg_login solicitud;
	solicitud.playerCode = rInfo.ID;

	int last = rInfo.name.copy(solicitud.nombre, 49, 0);
	solicitud.nombre[last] = '\0';

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
	if(rsp.ok) {
		printf( "Solicitud OK\n");
		this->playerID = solicitud.playerCode;
	} else {
		switch (rsp.cause) {
		case KICK_ID_IN_USE:
			ErrorLog::getInstance()->escribirLog("Error login, otro jugador ya tiene esa ID");
			break;
		case KICK_FULL:
			ErrorLog::getInstance()->escribirLog("Error login, la partida no acepta m�s jugadores");
			break;
		case KICK_INVALID_ID:
			ErrorLog::getInstance()->escribirLog("Error login, ID especificada invalida");
			break;
		case KICK_NAME_IN_USE:
			ErrorLog::getInstance()->escribirLog("Error login, otro jugador ya tiene ese nombre");
			break;
		default:
			ErrorLog::getInstance()->escribirLog("Error desconocido en el login");
		}
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



// Proces todos los updates que estan en el momento de la invoaci�n
// en la cola. La estructura se bloquea hasta que se acaben los updates.
bool Cliente::procesarUpdates(Partida* game, unsigned int actPlayer) {
	SDL_SemWait(this->updates_lock);
	// printf("Hay %d updates\n", this->updates.size());

	Jugador* jAct;
	while( !this->updates.empty() ) {
		struct msg_update upd;
		upd = this->updates.front();
		this->updates.pop();
		
		switch (upd.accion) {
		case MSJ_JUGADOR_DERROTADO:
			game->procesarUpdate(upd, actPlayer);
			if (upd.idEntidad == actPlayer) {
				cout << "Has sido derrotado!!!!" << endl;
				SDL_SemPost(this->updates_lock);
				SDL_Delay(1000);
				return true;
			}
			break;

		case MSJ_JUGADOR_GANADOR:
			if (upd.idEntidad == actPlayer) {
				cout << "Has ganado!!!!" << endl;
				Mix_Chunk* snd = BibliotecaDeImagenes::obtenerInstancia()->devolverSonido("victory_win");
				Mix_PlayChannel( -1, snd, 0 );
				SDL_SemPost(this->updates_lock);
				SDL_Delay(1000);
				return true;
			}
			break;
		case MSJ_STATE_CHANGE:
		case MSJ_MOVER:
		case MSJ_RES_CHANGE:
		case MSJ_VIDA_CHANGE:
		case MSJ_ASIGNAR_JUGADOR:
		case MSJ_AVANZAR_PRODUCCION:
		case MSJ_PRODUCIR_UNIDAD:
		case MSJ_FINALIZAR_EDIFICIO:
		case MSJ_FINALIZAR_PRODUCCION:
		case MSJ_ELIMINAR:
			game->procesarUpdate(upd, actPlayer); break;
		case MSJ_JUGADOR_LOGIN:
			game->obtenerJugador(upd.idEntidad)->settearConexion(true); break;
		case MSJ_JUGADOR_LOGOUT:
			game->obtenerJugador(upd.idEntidad)->settearConexion(false); break;

		//	game->escenario->quitarEntidad(upd.idEntidad); break;
		case MSJ_RECURSO_JUGADOR:
			jAct = game->obtenerJugador(upd.idEntidad);
			if (jAct) {
				resource_type_t tipoR = RES_T_NONE;
				switch ((int)upd.extra2) {
				case 0: tipoR = RES_T_GOLD; break;
				case 1: tipoR = RES_T_WOOD; break;
				case 2: tipoR = RES_T_FOOD; break;
				case 3: tipoR = RES_T_STONE; break;
				}
				jAct->settearRecurso(tipoR, (int)upd.extra1);
			}
			break;
		default:
			if (upd.accion >= MSJ_SPAWN)
				game->procesarUpdate(upd, actPlayer);
		}

	}
	SDL_SemPost(this->updates_lock);
	return false;
}


struct mapa_inicial Cliente::getEscenario(void) {
	int result;
	struct mapa_inicial scene_info;
	result = sRead(this->clientSocket, (char*)&(scene_info.mInfo), sizeof(scene_info.mInfo));
	if ( result <= 0 ) {
		ErrorLog::getInstance()->escribirLog("Error recibiendo mapa.", LOG_ERROR);
		return scene_info;
	}
	
	// Se la cantidad de tipos de terreno que voy a recibir
	for (int i = 0; i < scene_info.mInfo.cantTerrenosEspeciales; i++) {
		msg_terreno* terreno_act = new msg_terreno();
		result = sRead(this->clientSocket, (char*)terreno_act, sizeof(msg_terreno));
		if ( result <= 0 ) {
			ErrorLog::getInstance()->escribirLog("Error recibiendo informacion de terreno.", LOG_ERROR);
			return scene_info;
		}
		scene_info.terreno.push_back(terreno_act);
	}

	// Se la cantidad de jugadores que voy a recibir, y el tama�o de sus visiones
	for (int i = 0; i < scene_info.mInfo.cantJugadores; i++) {
		jugador_info* jugador_act = new jugador_info();
		result = sRead(this->clientSocket, (char*)&(jugador_act->jInfo), sizeof(jugador_act->jInfo));
		if ( result <= 0 ) {
			ErrorLog::getInstance()->escribirLog("Error recibiendo jugador.", LOG_ERROR);
			return scene_info;
		}

		scene_info.jugadores.push_back(jugador_act);
	}


	// Se la cantidad de tipos que voy a recibir
	for (int i = 0; i < scene_info.mInfo.cantTipos; i++) {
		msg_tipo_entidad* tipo_act = new msg_tipo_entidad();
		result = sRead(this->clientSocket, (char*)(tipo_act), sizeof(*tipo_act));


		// Ac� verifico si hay lista de entrenables
		// En caso positivo el protocolo indica que lo que se mande a continuaci�n
		// tendr� un tama�o de 50 * cant_entrenables * sizeof(char); y que corresponder�
		// a un arreglo de char[50] donde cada elemento es el nombre de la entidad que se puede entrenar.
		if (tipo_act->cant_entrenables > 0) {
			cout << "Voy a recibir una lista de entrenables para: " << tipo_act->name << " - " << tipo_act->cant_entrenables << endl;
			char* entrenables_act = new char[50 * tipo_act->cant_entrenables];
			result = sRead(this->clientSocket, (char*)(entrenables_act), sizeof(char) * 50 * tipo_act->cant_entrenables);
			tipo_act->entrenables = entrenables_act;
		} else {
			tipo_act->entrenables = NULL;
		}


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


bool Cliente::sendReadySignal(bool ready) {
	struct msg_client_ready msg;
	msg.ok = ready;

	int result = send(this->clientSocket, (char*)&msg, sizeof(msg), 0);
	if (result <= 0) {
		ErrorLog::getInstance()->escribirLog("Error enviando ack cliente.", LOG_ERROR);
		return false;
	}
	return true;
}

void Cliente::generarKeepAlive(void) {
	msg_event msg;
	msg.accion = MSJ_KEEP_ALIVE;
	this->agregarEvento(msg);
}
