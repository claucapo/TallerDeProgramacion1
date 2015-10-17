#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <SDL.h>
#include <SDL_thread.h>

#include "Servidor.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_PORT "27015"

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


int main(int argc, char* argv[]) {

	// parseaYAML();       Parseo del yaml, y obtención de información crucial para la conexión, como el puerto.
	// cargarPartida();    Acá se tiene que poder inicializar la partida a partir del yaml, definiendo los jugadores y sus nombres, así como el escenario

	SOCKET ListenSocket = inicializarConexion();
	if (ListenSocket == INVALID_SOCKET) {
		// limpiarPartida();
		// cerrar();
		return 1;
	}
	
	printf("Listo para aceptar clientes\n");

	Servidor server = Servidor(ListenSocket /*, Partida */);
	server.start();

	// Cambiar por alguna condición de corte en los mensajes??
	// No creo, el server siempre vive, no?
	while ( true ) {
		server.procesarEventos();

		server.enviarUpdates();

		SDL_Delay(1000);
	}
	
	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}