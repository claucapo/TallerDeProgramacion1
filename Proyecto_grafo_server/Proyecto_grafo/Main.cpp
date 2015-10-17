#undef UNICODE

#define WIN32_LEAN_AND_MEAN

// Includes de sockets
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SDL.h>
#include <SDL_thread.h>

// Entrada y salida... se pueden borrar al reemplazar por log???
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "ErrorLog.h"

#include "ConfigParser.h" // Para parsear el YAML
#include "FactoryEntidades.h"
#include "Partida.h"
#include "Jugador.h"
#include "Servidor.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_PORT "27015"
#define TESTING_ENABLED false
#define ARCHIVO_YAML "default.yaml"


SOCKET inicializarConexion(void) {
	WSADATA wsaData;
	struct addrinfo* result = NULL;
	struct addrinfo hints;

	// Inicia Winsock
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
		return INVALID_SOCKET;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if ( iResult != 0 ) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return INVALID_SOCKET;
	}

	SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return ListenSocket;
	}


	iResult = bind (ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		freeaddrinfo(result);
		WSACleanup();
		return INVALID_SOCKET;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}
	return ListenSocket;
}

// Carga la información de los jugadores en la partida
void cargarJugadores(Partida* partida, std::list<jugadorInfo_t*> jInfoL) {
	for (std::list<jugadorInfo_t*>::const_iterator iter = jInfoL.begin(); iter != jInfoL.end(); ++iter) {
		jugadorInfo_t act = *(*iter);
		Jugador* newPlayer = new Jugador(act.nombre, act.id, act.color);
		if ( !(partida->agregarJugador(newPlayer)) ) {
			ErrorLog::getInstance()->escribirLog("No se pudo loguear a jugador [" + act.nombre + "].", LOG_ERROR); 
			delete newPlayer;
		}
	}
}

// Carga los moldes apra las entidades dentro del singleton FactoryEntidades
void cargarFactoryEntidades(std::list<entidadInfo_t*> eInfoL) {
	for (std::list<entidadInfo_t*>::const_iterator iter = eInfoL.begin(); iter != eInfoL.end(); ++iter) {
		entidadInfo_t act = *(*iter);
		FactoryEntidades::obtenerInstancia()->agregarEntidad(act);
	}
}

// Carga las instancias de las entidades al escenario
void cargarEscenario(Partida* partida, escenarioInfo_t eInfo) {
	Escenario* scene = new Escenario(eInfo.size_X, eInfo.size_Y);

	for(list<instanciaInfo_t*>::const_iterator it = eInfo.instancias.begin(); it != eInfo.instancias.end(); ++it) {
		Entidad* entidad = FactoryEntidades::obtenerInstancia()->obtenerEntidad((*it)->tipo);
		if (entidad) {
			Posicion posicion = Posicion((float)(*it)->x, (float)(*it)->y);
			Jugador* owner = partida->obtenerJugador((*it)->player);
			if (!owner) {
				ErrorLog::getInstance()->escribirLog("Error asignando jugador a entidad, jugador inexistente. Se le asigna gaia", LOG_WARNING);
				owner = partida->obtenerJugador(0); // Si no existe el jugador, loggeo el evento y le asigno gaia
			}
			if ( !scene->ubicarEntidad(entidad, &posicion) )
				delete entidad;
			else
				entidad->asignarJugador(owner);
		}
	}

		list<Entidad*> lEnt = scene->verEntidades();
		for(list<Entidad*>::iterator it = lEnt.begin(); it != lEnt.end(); ++it) {
			cout << (*it)->verPosicion()->toStrRound()<< "-" << (*it)->name << "-" << (*it)->verID() << endl;
		}
		cout << endl;

	partida->asignarEscenario(scene);
}

/*******************************************

			PROGRAMA PRINCIPAL

********************************************/

#define STEP_DURATION 50 // duración de un paso de la simulación

int main(int argc, char* argv[]) {
	
	// Esto tiene que ser la logica y tiene que incluir players
	ConfigParser parser = ConfigParser();
	parser.setPath(ARCHIVO_YAML);
			
	parser.parsearTodo();
	
	SOCKET ListenSocket = inicializarConexion();
	if (ListenSocket == INVALID_SOCKET) {
		// limpiarPartida();
		// cerrar();
		return 1;
	}
	
	Partida* game = new Partida();
	cargarJugadores(game, parser.verInfoJugadores());
	cargarFactoryEntidades(parser.verInfoEntidades());

	cargarEscenario(game, parser.verInfoEscenario());
	
	printf("Listo para aceptar clientes\n");

	Servidor server = Servidor(ListenSocket, game);
	server.start();

	// Cambiar por alguna condición de corte en los mensajes??
	// No creo, el server siempre vive, no?
	bool exit = false;
	while ( !exit ) {
		float timeA = SDL_GetTicks();

		server.procesarEventos();

		server.avanzarFrame();

		server.enviarUpdates();
		
		float timeB = SDL_GetTicks();
		if((FRAME_DURATION - timeB + timeA) > 0)
			SDL_Delay(FRAME_DURATION - timeB + timeA);
	}
	
	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}