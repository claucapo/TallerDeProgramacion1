#define WIN32_LEAN_AND_MEAN

// Winsock
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// printf... borrar en el futuro?
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <sstream>
#include <set>
#include <string.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "Cliente.h"
#include "ConfigParser.h"
#include "BibliotecaDeImagenes.h"
#include "ErrorLog.h"
#include "Partida.h"
#include "FactoryEntidades.h"
#include "Entidad.h"
#include "Jugador.h"
#include "Escenario.h"
#include "ProcesadorEventos.h"
#include "PantallaInicio.h"
#include "Spritesheet.h"
#include "GraficadorPantalla.h"
#include "Enumerados.h"

// Magia negra de VS
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define ARCHIVO_YAML "default.yaml"
#define DEFAULT_PORT "27015"
#define DEFAULT_IP "127.0.0.1"





SOCKET inicializarConexion(redInfo_t rInfo) {
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

	PCSTR ip = rInfo.ip.c_str();
	
	PCSTR port = rInfo.port.c_str();

	iResult = GetAddrInfo(ip, port, &hints, &result);
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

void cargarBibliotecaImagenes(std::list<entidadInfo_t*> eInfoL) {
	for (std::list<entidadInfo_t*>::const_iterator iter = eInfoL.begin(); iter != eInfoL.end(); ++iter) {
		entidadInfo_t act = *(*iter);
		DatosImagen* data = new DatosImagen();

		data->path = act.spritePath;
		data->origenX = act.pixel_align_X;
		data->origenY = act.pixel_align_Y;
		// cout<< act.spritePath << " offset xy: " << data->origenX <<"." << data->origenY <<endl;
		data->columnas = act.subX;
		data->filas = act.subY;
		data->fps = act.fps;
		data->delay = act.delay;

		BibliotecaDeImagenes::obtenerInstancia()->cargarDatosImagen(act.nombre, data);
	}
}

void cargarSonidosEntidad(msg_tipo_entidad* act){
	BibliotecaDeImagenes* bi = BibliotecaDeImagenes::obtenerInstancia();
	bi->cargarEfectoSonido(act->name);
	
	if(act->tipo == ENT_T_UNIT){
		string n = act->name; string p = n + "_move";
		char* movName = &p[0u];
		bi->cargarEfectoSonido(movName);
		p = n + "_die";
		cout << p << endl;
		movName = &p[0u];
		bi->cargarEfectoSonido(movName);
		if(act->habilidades[ACT_ATACK]){
			p = n + "_atk";
			cout << p << endl;
			movName = &p[0u];
			bi->cargarEfectoSonido(movName);
			p = n + "_atk2";
			cout << p << endl;
			movName = &p[0u];
			bi->cargarEfectoSonido(movName);
			}
		if(act->habilidades[ACT_BUILD]){
			p = n + "_build";
			movName = &p[0u];
			bi->cargarEfectoSonido(movName);
			}
		if(act->habilidades[ACT_COLLECT]){
			p = n + "_collect";
			movName = &p[0u];
			bi->cargarEfectoSonido(movName);
			p = n + "_mine";
			movName = &p[0u];
			bi->cargarEfectoSonido(movName);
			p = n + "_chop";
			movName = &p[0u];
			bi->cargarEfectoSonido(movName);
			}
		}
}

void agregarSonidosExtra(){
	BibliotecaDeImagenes::obtenerInstancia()->cargarEfectoSonido("villager_create");

	
	BibliotecaDeImagenes::obtenerInstancia()->cargarEfectoSonido("victory_win");
	BibliotecaDeImagenes::obtenerInstancia()->cargarEfectoSonido("underAttack");

	BibliotecaDeImagenes::obtenerInstancia()->cargarEfectoSonido("ent_create");
}

Partida* generarPartida(mapa_inicial data) {

	while(!data.tipos.empty()) {
		msg_tipo_entidad* act = data.tipos.front();
		data.tipos.pop_front();
		FactoryEntidades::obtenerInstancia()->agregarEntidad(*act);
		
		cargarSonidosEntidad(act);

		delete act;
	}

	agregarSonidosExtra();

	Escenario* scene = new Escenario(data.mInfo.coordX, data.mInfo.coordY);
	Partida* game = new Partida();

	// Setteo los tipos de terreno
	while (!data.terreno.empty()) {
		msg_terreno* act = data.terreno.front();
		data.terreno.pop_front();
		scene->verMapa()->settearTipoTerreno(Posicion(act->coord_x, act->coord_y), act->tipo);
		delete act;
	}

	// Agrego los jugadores
	while (!data.jugadores.empty()) {
		jugador_info* jugador_act = data.jugadores.front();
		data.jugadores.pop_front();

		// Hardcodeo el caso especial (gaia)
		if (jugador_act->jInfo.id == 0 && jugador_act->jInfo.name == "gaia") {
			Jugador* gaia = game->obtenerJugador(0);
			gaia->asignarVision(data.mInfo.coordX, data.mInfo.coordY);
			gaia->verVision()->setFromArray(jugador_act->varray);
			delete jugador_act;
		} else {
			Jugador* nuevo = new Jugador(jugador_act->jInfo.name_def, jugador_act->jInfo.id, jugador_act->jInfo.color);
			nuevo->asingarNombre(jugador_act->jInfo.name);
			if (!game->agregarJugador(nuevo)) {
				delete nuevo;
				delete[] jugador_act->varray;
				delete jugador_act;
			} else {
				nuevo->asignarVision(data.mInfo.coordX, data.mInfo.coordY);

				// Hacer esto de manera más elegante... mediante una función
				recursos_jugador_t rj = nuevo->verRecurso();
				rj.comida = jugador_act->jInfo.recursos.comida;
				rj.oro = jugador_act->jInfo.recursos.oro;
				rj.madera = jugador_act->jInfo.recursos.madera;
				rj.piedra = jugador_act->jInfo.recursos.piedra;

				
				//nuevo->verVision()->setFromArray(jugador_act->varray);
				nuevo->settearConexion(jugador_act->jInfo.conectado);
				delete jugador_act;
			}
		}
	}

	// Ahora las entidades
	while (!data.instancias.empty()) {
		msg_instancia* inst_act = data.instancias.front();
		data.instancias.pop_front();

		Entidad* entidad = FactoryEntidades::obtenerInstancia()->obtenerEntidad(inst_act->name, inst_act->idEntidad);
		entidad->settearEstado(inst_act->estadoEntidad);
		if (entidad) {
		//	entidad->asignarSprite(new Spritesheet(entidad->verNombre()));
			Posicion posicion = Posicion((float)inst_act->coordX, (float)inst_act->coordY);
			Jugador* owner = game->obtenerJugador(inst_act->playerCode);
			if (!owner) {
				ErrorLog::getInstance()->escribirLog("Error asignando jugador a entidad, jugador inexistente. Se le asigna gaia", LOG_WARNING);
				owner = game->obtenerJugador(0); // Si no existe el jugador, loggeo el evento y le asigno gaia
			}
			if ( !scene->ubicarEntidad(entidad, &posicion) )
				delete entidad;
			else {
				entidad->asignarJugador(owner);
				string nombreEnt = entidad->verNombre();
				if(owner->verID() == 2)
					nombreEnt = nombreEnt + '2';
				if(owner->verID() == 3)
					nombreEnt = nombreEnt + '3';
				Spritesheet* cas = new Spritesheet(nombreEnt);
				entidad->asignarSprite(cas);
				// Inicializo spritesheet random
				if(entidad->verTipo() == ENT_T_RESOURCE)
					cas->cambiarSubImagen(0,(rand() % 8));
			}
		}

	}

	game->asignarEscenario(scene);
	return game;
}

Mix_Music* reproducirMusicaFondo(char* path){
	Mix_Music *music = NULL;
	Mix_VolumeMusic(MIX_MAX_VOLUME * 0.3);
	music = Mix_LoadMUS(path);
	if(music != NULL){
		Mix_PlayMusic( music, -1 );
	}
	return music;
}

void actualizarMusicaFondo(Partida* game, bool habiaViolencia){
	if((!habiaViolencia) && game->hayViolencia)
		reproducirMusicaFondo("recursos\\bck_snd2.wav");
	if(habiaViolencia && (!game->hayViolencia))
		reproducirMusicaFondo("recursos\\bck_snd1.wav");	

}


void pantallaMenu(GraficadorPantalla* gp, struct datosPantInic* datos){

	PantallaInicio pantIni;
	Mix_Music *music = reproducirMusicaFondo("recursos\\main_menu.wav");

	SDL_Event evento;
	bool loopear = true;
	string playerName;
	do{
		while((SDL_PollEvent(&evento) != 0)){
			if(evento.type == SDL_KEYDOWN)
				if(evento.key.keysym.scancode == SDL_SCANCODE_SPACE)
					loopear = false;
				if((evento.type) == SDL_QUIT)
					return;

		}
		gp->mostrarPantallaInicio(false, playerName);
		SDL_Delay(FRAME_DURATION);
	}while(loopear);

	loopear = true;
	do{
		while((SDL_PollEvent(&evento) != 0)){
			if(evento.type == SDL_KEYDOWN){
				string q = pantIni.procesarEventoLetras(evento);
				if(q != "error"){
					if(q == "erase"){
						if(playerName.size())
							playerName.resize(playerName.size() -1);
							}
					else if(q == "finish")
						loopear = false;
					else 
						playerName += q;
				}
			}
			if((evento.type) == SDL_QUIT)
					return;
			
		gp->mostrarPantallaInicio(true, playerName);
		SDL_Delay(FRAME_DURATION);
		}
	}while(loopear);

	datos->pName = playerName;

	Mix_FreeMusic( music );
	Mix_HaltMusic();

	pantIni.pantallaInicio(datos, gp->getPantalla(), gp->getVentana());
}

#define CODE_CONTINUE 1
#define CODE_RESET -2
#define CODE_EXIT -1


//------------------------------------
//------------- MAIN -----------------
//------------------------------------


int wmain(int argc, char* argv[]) {

	// Cargar YAML
	ConfigParser parser = ConfigParser();
	parser.setPath(ARCHIVO_YAML);
	parser.parsearTodo();

	ErrorLog::getInstance()->escribirLog("----INICIANDO----");
		
	// Cargar imagenes del YAML
	cargarBibliotecaImagenes(parser.verInfoEntidades());

	// Graficos lindos xd
	
	pantallaInfo_t pInfo = parser.verInfoPantalla();
	GraficadorPantalla* gp = new GraficadorPantalla(pInfo.screenW, pInfo.screenH, pInfo.fullscreen, "DEFAULT");

	SDL_Window* gameWindow = gp->getVentana();
	SDL_Surface* gameScreen = gp->getPantalla();
	

	inicio:
	struct datosPantInic datos;
	pantallaMenu(gp, &datos);

	redInfo_t rInfo = parser.verInfoRed();
	rInfo.port = datos.puerto;
	rInfo.name = datos.pName;
	rInfo.ip = datos.ip;

	// Después modificar el método para que tome como parámetro el puerto y la ip del yaml
	SOCKET connectSocket = inicializarConexion(rInfo);
	if (connectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		// Salir elegantemente;
		closesocket(connectSocket);
		return 1;
	}
	
	ErrorLog::getInstance()->escribirLog("Conexion establecida");

	// Intento establecer conexión
	struct mapa_inicial elMapa;
	Partida* game;
	Cliente client = Cliente(connectSocket);
	if ( client.login(rInfo) ) {
		elMapa = client.getEscenario();
		printf("Tam mapa recibido: %d - %d\n", elMapa.mInfo.coordX, elMapa.mInfo.coordY);
		game = generarPartida(elMapa);
	} else {
		printf("Unable to login!\n");
		closesocket(connectSocket);
		return 1;
	}
	
	// Inicializar pantalla y graficador
	gp->asignarParametrosScroll(parser.verInfoGameplay().scroll_margen, parser.verInfoGameplay().scroll_vel);	
	BibliotecaDeImagenes::obtenerInstancia()->asignarPantalla(gameScreen);

	gp->asignarPartida(game);
	Jugador* playerActual = game->obtenerJugador(parser.verInfoRed().ID);
	gp->asignarJugador(playerActual);
	playerActual->settearConexion(true);
	
	if (!client.sendReadySignal(true)){
		return 1;	
	}
	


	if (!client.startLobby(game)) {
		return 1;
	}


	// En este punto el cliente ya está conectado
	client.start();

	int codigo_programa = CODE_CONTINUE;

	ProcesadorEventos pE(game, gp);

	Mix_Music *music = reproducirMusicaFondo("recursos\\bck_snd1.wav");
	bool habiaViolencia = false;
	game->hayViolencia = false;

	while (codigo_programa != CODE_EXIT && !client.must_close) {
		
		float timeA = SDL_GetTicks();
		playerActual->poblacionMax = 0;
		playerActual->poblacionAct = 0;
		bool partidaOver = client.procesarUpdates(game, playerActual->verID());
		if(partidaOver)
			break;
		float timeC = SDL_GetTicks();

		game->avanzarFrame(playerActual->verID());

		gp->dibujarPantalla();
		client.generarKeepAlive();
		
		float timeD = SDL_GetTicks();

		SDL_Event evento;
		while(SDL_PollEvent(&evento) != 0){
			// Generar msg_evento: Por ahora sólo te deja scrollear
			codigo_programa = pE.procesarEvento(evento, &client, playerActual);
			if(codigo_programa == CODE_EXIT)
				break;
		}

		actualizarMusicaFondo(game, habiaViolencia);
		habiaViolencia = game->hayViolencia;

		float timeB = SDL_GetTicks();
		if((FRAME_DURATION - timeB + timeA) > 0)
			SDL_Delay(FRAME_DURATION - timeB + timeA);

		std::stringstream s;
		s << "U: " << timeC- timeA << " - AF: " << timeD - timeC << " - E: "<< timeB-timeD << " - T:" << timeB-timeA;
//		ErrorLog::getInstance()->escribirLog(s.str());
		// cout << timeB - timeA << endl;
	}

	Mix_HaltMusic();

	closesocket(connectSocket);
	WSACleanup();

	Mix_FreeMusic( music );

	if(codigo_programa != CODE_EXIT){
		
		while(true){
			gp->pantallaFinal(game->escenario->jugadorFueDerrotado(playerActual->verID()));
			SDL_Event eevee;
			while(SDL_PollEvent(&eevee) != 0){
				if(eevee.type == SDL_KEYDOWN)
					goto inicio;
				if(eevee.type == SDL_QUIT)
					break;
				}
			SDL_Delay(FRAME_DURATION);
		}
	}
	
	
	delete gp;

	Mix_CloseAudio();
	SDL_Quit();
	 

	return 0;

}