#undef UNICODE

#define WIN32_LEAN_AND_MEAN

// Includes de sockets
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_thread.h>
#include "PantallaInicio.h"

// Entrada y salida... se pueden borrar al reemplazar por log???
// Informar cosas por la consola no es malo... No quiero que el server sea una pantalla negra xd
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <sstream>

#include "ErrorLog.h"

#include "ConfigParser.h" // Para parsear el YAML
#include "FactoryEntidades.h"
#include "Partida.h"
#include "Jugador.h"
#include "Servidor.h"
#include "GeneradorEscenarios.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define TESTING_ENABLED false
#define ARCHIVO_YAML "WaterTest.yaml"
#define TIMEOUT 20000




SOCKET inicializarConexion(redInfo_t rInfo) {
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

	PCSTR port = rInfo.port.c_str();

	iResult = getaddrinfo(NULL, port, &hints, &result);
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

	DWORD timeout = TIMEOUT;
	setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	
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


// -----------------------------------------------------
// ----------------LOADER DE LA PARTIDA-----------------
// -----------------------------------------------------



// Carga la información de los jugadores en la partida
void cargarJugadores(Partida* partida, std::list<jugadorInfo_t*> jInfoL, int cant_jugadores) {
	if (cant_jugadores > MAX_JUGADORES || cant_jugadores <= 0)
		cant_jugadores = 2;

	int i = 0;
	for (std::list<jugadorInfo_t*>::const_iterator iter = jInfoL.begin(); iter != jInfoL.end() && i < cant_jugadores; ++iter, i++) {
		jugadorInfo_t act = *(*iter);
		Jugador* newPlayer = new Jugador(act.nombre, act.id, act.color);
		if ( !(partida->agregarJugador(newPlayer)) ) {
			ErrorLog::getInstance()->escribirLog("No se pudo loguear a jugador [" + act.nombre + "].", LOG_ERROR); 
			delete newPlayer;
		}
	}
}

// Carga los moldes para las entidades dentro del singleton FactoryEntidades
void cargarFactoryEntidades(std::list<entidadInfo_t*> eInfoL) {
	for (std::list<entidadInfo_t*>::const_iterator iter = eInfoL.begin(); iter != eInfoL.end(); ++iter) {
		entidadInfo_t act = *(*iter);
		FactoryEntidades::obtenerInstancia()->agregarEntidad(act);
	}
}

// Carga las instancias de las entidades al escenario
void cargarEscenario(Partida* partida, escenarioInfo_t eInfo) {
	Escenario* scene = new Escenario(eInfo.size_X, eInfo.size_Y);

	for(list<Jugador*>::iterator it = partida->jugadores.begin(); it != partida->jugadores.end(); ++it) {
		(*it)->asignarVision(eInfo.size_X, eInfo.size_Y);
	}

	for(list<terrenoInfo_t*>::const_iterator it = eInfo.terreno.begin(); it != eInfo.terreno.end(); ++it) {
		terrenoInfo_t act = *(*it);
		Posicion pos = Posicion(act.x, act.y);
		if (act.tipo_terreno == "water") {
			scene->verMapa()->settearTipoTerreno(pos, TERRAIN_WATER);
		} else if (act.tipo_terreno == "grass") {
			scene->verMapa()->settearTipoTerreno(pos, TERRAIN_GRASS);
		} else {
			ErrorLog::getInstance()->escribirLog("Error al generar el mapa, tipo de terreno [" + act.tipo_terreno + "] desconocido", LOG_WARNING);
		}
	}

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

bool cargarEscenarioAleatorio(Partida* partida, int size, tipo_partida_t tipo, int cant_jugadores) {
	// Chequeo de los parámetros (e inicializacion a valores default en caso de error)
	if (cant_jugadores < MIN_JUGADORES || cant_jugadores > MAX_JUGADORES)
		cant_jugadores = 2;

	if (size < MIN_SIZE || size > MAX_SIZE)
		size = 50;
	
	Escenario* scene = new Escenario(size, size);
	tile_content* tiles = generarEscenario(size, cant_jugadores);
	if (!tiles) {
		return false;
	} else {
		std::string ent_name = "unknown";
		int owner;
		Entidad* entidad = nullptr;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				tile_content act = tiles[i * size + j];
				ent_name = tile_content_entity[act];
				owner = tile_content_owner[act];
				if (act >= TILE_CRIT_UNIT1) {
					switch(tipo) {
					case PARTIDA_REGICIDA:
						ent_name = "king"; break;
					case PARTIDA_CAPTURE_FLAG:
						ent_name = "flag"; break;
					default:
						ent_name = "unknown";
					}
				}
				if (!(ent_name == "unknown")) {
					entidad = FactoryEntidades::obtenerInstancia()->obtenerEntidad(ent_name);
					if (entidad) {
						Posicion posicion = Posicion((float)i, (float)j);
						Jugador* player = partida->obtenerJugador(owner);
						if (!player) {
							ErrorLog::getInstance()->escribirLog("Error asignando jugador a entidad, jugador inexistente. Se le asigna gaia", LOG_WARNING);
							player = partida->obtenerJugador(0);
						}
						if ( !scene->ubicarEntidad(entidad, &posicion) )
							delete entidad;
						else
							entidad->asignarJugador(player);
					}
				}
			}
		}
	}
	partida->asignarEscenario(scene);
	return true;
}

/*******************************************
			PROGRAMA PRINCIPAL
********************************************/
int main(int argc, char* argv[]) {
	
	// 1) Parseo del YAML
	ConfigParser parser = ConfigParser();
	parser.setPath(ARCHIVO_YAML);

	parser.parsearTodo();

	ErrorLog::getInstance()->escribirLog("----INICIANDO----");
	
	redInfo_t rInfo = parser.verInfoRed();
	
	// 2) Obtencion de datos de la pantalla

	// NECESITO: Puerto, Cant_Jugadores, Tipo_Partida, Mapa_Aleatorio, Tam_Mapa_Aleatorio
	
	PantallaInicio pantInc;
	struct datosPantInic datos;
	pantInc.pantallaInicio(&datos);

	rInfo.port = datos.puerto;
	rInfo.cant_jugadores = datos.cantPlayers;
	rInfo.tipo_partida = datos.tipoPartida - 1;


	// 3) Inicialización de socket
	SOCKET ListenSocket = inicializarConexion(rInfo);
	if (ListenSocket == INVALID_SOCKET) {
		return 1;
	}
	

	// 4) Creación de la partida
	// 4.1) Chequeo de errores
	if ((rInfo.cant_jugadores <= 0) || (rInfo.cant_jugadores > MAX_JUGADORES))
		rInfo.cant_jugadores = 2;

	if (rInfo.tipo_partida < 0 || rInfo.tipo_partida > PARTIDA_REGICIDA)
		rInfo.tipo_partida = PARTIDA_SUPREMACIA;

	Partida* game = new Partida();
	cargarJugadores(game, parser.verInfoJugadores(), rInfo.cant_jugadores);
	cargarFactoryEntidades(parser.verInfoEntidades());

	if (!rInfo.random_map) {
		cargarEscenario(game, parser.verInfoEscenario());
	} else {
		if (!cargarEscenarioAleatorio(game, parser.verInfoEscenario().size_X, tipo_partida_t(rInfo.tipo_partida), rInfo.cant_jugadores)) {
			cargarEscenario(game, parser.verInfoEscenario());
		}
	}

	switch(rInfo.tipo_partida) {
	case PARTIDA_REGICIDA:
		game->inicializarCondicionVictoria(FactoryEntidades::obtenerInstancia()->obtenerTypeID("king"), LOSE_UNITS); break;
	case PARTIDA_SUPREMACIA:
		game->inicializarCondicionVictoria(FactoryEntidades::obtenerInstancia()->obtenerTypeID("town center"), LOSE_ALL); break;
	case PARTIDA_CAPTURE_FLAG:
		game->inicializarCondicionVictoria(FactoryEntidades::obtenerInstancia()->obtenerTypeID("flag"), TRANSFER_ALL); break;
	}
	// FIN de la configuracion de la partida

	printf("Listo para aceptar clientes\n");


	
	bool exit = false;
	
	system("cls");
	cout << "SERVER INITIALIZED"<< endl;
	cout << "DO NOT CLOSE THIS WINDOW UNTIL THE GAME HAS FINISHED!" << endl;
	for (int i = 0; i < parser.verInfoEscenario().size_X; i++) {
		for (int j = 0; j < parser.verInfoEscenario().size_Y; j++) {
			if (!game->escenario->verMapa()->posicionEstaVacia(&Posicion(i,j)))
				cout<< "X";
			else
				cout << "_";
		}
		cout<< i<<endl;
	}



	Servidor server = Servidor(ListenSocket, game, parser.verInfoRed().cant_jugadores);
	server.start();

	list<msg_update*> updates;
	while ( !exit ) {
		float timeA = SDL_GetTicks();
		server.procesarEventos();
		
		float timeC = SDL_GetTicks();
		updates = server.avanzarFrame();
	
		float timeD = SDL_GetTicks();
		server.enviarUpdates(updates);
		
		float timeB = SDL_GetTicks();
		if((FRAME_DURATION - timeB + timeA) > 0)
			SDL_Delay(FRAME_DURATION - timeB + timeA); 

		std::stringstream s;
		s << "E: " << timeC- timeA << " - AF: " << timeD - timeC << " - U: "<< timeB-timeD << " - T:" << timeB-timeA;
		ErrorLog::getInstance()->escribirLog(s.str());
	

	}
	
	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}