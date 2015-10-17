#define WIN32_LEAN_AND_MEAN

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

int wmain(int argc, char* argv[]) {

	// parsearYAML()
	// cargarBibliotecaDeImagenes()
	// Después modificar el método para que tome como parámetro el puerto y la ip del yaml
	SOCKET connectSocket = inicializarConexion();
	if (connectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		// Salir elegantemente;
		closesocket(connectSocket);
		return 1;
	}

	printf( "Conexion establecida\n" );

	Cliente client = Cliente(connectSocket);
	if ( client.login() ) {
		// Aca no debería ir la recepción del escenario? O pensas
		// que debería ir dentro del login?
		struct msg_map elMapa = client.getEscenario();
		printf("Tam mapa recibido: %d - %d\n", elMapa.coordX, elMapa.coordY);
		// Escenario scene = generarEscenario( client.getEscenario() );
	} else {
		printf("Unable to login!\n");
		closesocket(connectSocket);
		return 1;
	}
	
	// En este punto el cliente ya está conectado
	client.start();
	int variableHarcodeada = 0;
	while (true) {
		client.procesarUpdates();
		// graficar()

		struct msg_event eventoHardcodeado;
		eventoHardcodeado.something = variableHarcodeada;
		client.agregarEvento(eventoHardcodeado);
		variableHarcodeada++;
		// generarKeepAlive();
		// while ( !SDL_Events.empty() ) { client.agregarEvento() }... o algo así
		// Debe usarase con la cola el yaml
		
		SDL_Delay(2000);
	}

	closesocket(connectSocket);
	WSACleanup();
	return 0;

}