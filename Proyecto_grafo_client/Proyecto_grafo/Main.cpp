#define WIN32_LEAN_AND_MEAN

#include "Escenario.h"
#include "Enumerados.h"
#include "Entidad.h"
#include "Unidad.h"
#include "ConversorUnidades.h"
#include "BibliotecaDeImagenes.h"
#include "Posicion.h"
#include "ErrorLog.h"
#include "GraficadorPantalla.h"
#include "ConfigParser.h"
#include "FactoryEntidades.h"
#include "DatosImagen.h"
#include "Jugador.h"
#include <iostream>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include "Tests.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include <SDL.h>

#include "Cliente.h"

// Magia negra de VS
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 100
#define DEFAULT_PORT "27015"
#define DEFAULT_IP "127.0.0.1"

using namespace std;

#define TESTING_ENABLED false

#define ARCHIVO_YAML "test0.yaml"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SOCKET inicializarConexion(void) {
	WSADATA wsaData;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
		return INVALID_SOCKET;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = GetAddrInfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return INVALID_SOCKET;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;
	ptr = result;
	bool try_again = true;
	while((ptr != NULL) && try_again) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return INVALID_SOCKET;
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			ptr=ptr->ai_next;
		} else {
			try_again = false;
		}
	}

	freeaddrinfo(result);
	return ConnectSocket;
}

/*********************************************
FUNCIONES AUXILIARES QUE YA NO SE NI QUE HACEN
**********************************************/

void cargarBibliotecaImagenes(std::list<entidadInfo_t*> eInfoL) {
	for (std::list<entidadInfo_t*>::const_iterator iter = eInfoL.begin(); iter != eInfoL.end(); ++iter) {
		entidadInfo_t act = *(*iter);
		DatosImagen* data = new DatosImagen();
		data->path = act.spritePath;
		data->casillasX = act.tamX;
		data->casillasY = act.tamY;
		data->origenX = act.pixel_align_X;
		data->origenY = act.pixel_align_Y;
		data->columnas = act.subX;
		data->filas = act.subY;
		data->fps = act.fps;
		data->delay = act.delay;
		BibliotecaDeImagenes::obtenerInstancia()->cargarDatosImagen(act.nombre, data);
		FactoryEntidades::obtenerInstancia()->agregarEntidad(act);
	}
}

Escenario* cargarEscenario(escenarioInfo_t escenarioInfo){
	Escenario* scene = new Escenario(escenarioInfo.size_X,escenarioInfo.size_Y);

	for(list<instanciaInfo_t*>::const_iterator it = escenarioInfo.instancias.begin(); it != escenarioInfo.instancias.end(); ++it) {
		Entidad* entidad = FactoryEntidades::obtenerInstancia()->obtenerEntidad((*it)->tipo);
		if (entidad) {
			Posicion posicion = Posicion((float)(*it)->x, (float)(*it)->y);
			if ( scene->ubicarEntidad(entidad, &posicion) ) {
				Spritesheet* cas = new Spritesheet(entidad->verTipo());
				entidad->asignarSprite(cas);
			} else {
				delete entidad;
			}
		}

		// Print de la lista de entidades ordenadas
		/*
		list<Entidad*> lEnt = scene->verEntidades();
		for(list<Entidad*>::iterator it = lEnt.begin(); it != lEnt.end(); ++it) {
			cout << (*it)->verPosicion()->getRoundX() << " - " << (*it)->verPosicion()->getRoundY() << " - " << (*it)->name << endl;
		}
		cout << endl;
		*/
	}

	Unidad* unit = FactoryEntidades::obtenerInstancia()->obtenerUnidad(escenarioInfo.protagonista.tipo);
	Posicion posP = Posicion(escenarioInfo.protagonista.x, escenarioInfo.protagonista.y);
	if (!scene->asignarProtagonista(unit, &posP)) {
		scene->asignarProtagonista(unit, &Posicion(0,0));
	}
	Spritesheet* sprite = new Spritesheet(unit->verTipo());
	unit->asignarSprite(sprite);
	unit->setVelocidad(1);
	return scene;
}

void cargarDatosEscenario(struct msg_map* elMapa, escenarioInfo_t* sceneInfo){
	sceneInfo->size_X = elMapa->coordX;
	sceneInfo->size_Y = elMapa->coordY;
}

/*******************************************

			PROGRAMA PRINCIPAL

********************************************/


int wmain(int argc, char* argv[]) {

			// Esto tiene que ser solo la parte de las imagenes
			ConfigParser parser = ConfigParser();
			parser.setPath(ARCHIVO_YAML);
			
			parser.parsearTodo();
	
			// Inicializar pantalla y graficador
			pantallaInfo_t pInfo = parser.verInfoPantalla();
			GraficadorPantalla* gp = new GraficadorPantalla(pInfo.screenW, pInfo.screenH, pInfo.fullscreen, parser.verInfoEscenario().name);
	SDL_Window* gameWindow = gp->getVentana();
	SDL_Surface* gameScreen = gp->getPantalla();
			gp->asignarParametrosScroll(parser.verInfoGameplay().scroll_margen, parser.verInfoGameplay().scroll_vel);		
			BibliotecaDeImagenes::obtenerInstancia()->asignarPantalla(gameScreen);

			// Cargar imagenes del YAML
			cargarBibliotecaImagenes(parser.verInfoEntidades());

			// Crear y asignar escenario
			escenarioInfo_t sceneInfo = parser.verInfoEscenario();
		
		

	// parsearYAML()
	// cargarBibliotecaDeImagenes()
	// Después modificar el método para que tome como parámetro el puerto y la ip del yaml
	SOCKET connectSocket = inicializarConexion();
	if (connectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		// Salir elegantemente;	<-- Por favor, el metodo se puede llamar asi? xd
		closesocket(connectSocket);
		return 1;
	}

	printf( "Conexion establecida\n" );

	Escenario* scene;
	Jugador* player = new Jugador("gaia");

	Cliente client = Cliente(connectSocket);
	if ( client.login() ) {
		// Se genera el Escenario según lo recibido
		struct msg_map elMapa = client.getEscenario();
		cargarDatosEscenario(&elMapa, &sceneInfo);
		scene = cargarEscenario(sceneInfo);
		scene->verProtagonista()->setVelocidad(parser.verInfoGameplay().velocidad);
		gp->asignarEscenario(scene);
		scene->verProtagonista()->asignarJugador(player);
		gp->asignarJugador(player);
		player->settearConexion(true);
		player->asignarVision(scene->verTamX(), scene->verTamY());
	} else {
		printf("Unable to login!\n");
		closesocket(connectSocket);
		return 1;
	}
			// Hardcodeo para que el player vea todas las posiciones
		/*	int i;
			int j;
			list<Posicion> miHermana;
			for(int i = 0; i < scene->verTamX(); i++)
				for(int j = 0; j < scene->verTamY(); j++){
					Posicion pAct = Posicion(i, j);
					miHermana.push_back(pAct);
				}
			
			player->agregarPosiciones(miHermana);
			*/

	// En este punto el cliente ya está conectado
	client.start();
	int variableHarcodeada = 0;
	float dTot = 0;
	while (true) {
				float timeA = SDL_GetTicks();

		//client.procesarUpdates();
		// graficar()

		struct msg_event eventoHardcodeado;
		eventoHardcodeado.idEntidad = variableHarcodeada;
		//client.agregarEvento(eventoHardcodeado);
		variableHarcodeada++;
		// generarKeepAlive();
		// while ( !SDL_Events.empty() ) { client.agregarEvento() }... o algo así
		// Debe usarase con la cola el yaml
				player->reiniciarVision();
				player->agregarPosiciones(scene->verMapa()->posicionesVistas(scene->verProtagonista()));
			
				SDL_Event e;
				while(SDL_PollEvent(&e) != 0){
					int mx, my;	
					if( e.button.button == SDL_BUTTON_RIGHT){
						SDL_GetMouseState(&mx, &my);

						ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
						float pX = cu->obtenerCoordLogicaX(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
						float pY = cu->obtenerCoordLogicaY(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
						Posicion destino = Posicion(pX, pY);
					
						scene->asignarDestinoProtagonista(&destino);
					}
				}

				scene->avanzarFrame();
				gp->dibujarPantalla();

				float timeB = SDL_GetTicks();
			
				dTot += (timeB - timeA);
				cout << "Time:" << timeB-timeA << endl;
				if((FRAME_DURATION -timeB + timeA) > 0)
					SDL_Delay(FRAME_DURATION -timeB + timeA);
	}

	closesocket(connectSocket);
	WSACleanup();
	return 0;

}