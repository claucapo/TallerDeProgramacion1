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

#include <SDL.h>
#include "Cliente.h"
#include "ConfigParser.h"
#include "BibliotecaDeImagenes.h"
#include "ErrorLog.h"
#include "Partida.h"
#include "FactoryEntidades.h"
#include "Entidad.h"
#include "Jugador.h"
#include "Escenario.h"

#include "Spritesheet.h"
#include "GraficadorPantalla.h"

// Magia negra de VS
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define ARCHIVO_YAML "default.yaml"
#define DEFAULT_PORT "27015"
#define DEFAULT_IP "127.0.0.1"


struct lex_compare {
    bool operator() (const Posicion& p1, const Posicion& p2) {
	if ( ((Posicion)(p1)).getX() == ((Posicion)(p2)).getX() )
		return ((Posicion)(p1)).getY() < ((Posicion)(p2)).getY();
    return ((Posicion)(p1)).getX() < ((Posicion)(p2)).getX();

		return true;
	
	int dist1 = p1.getRoundX() + p1.getRoundY();
	int dist2 = p2.getRoundX() + p2.getRoundY();
	if (dist1 < dist2)
		return true;
	else
		return false; }
};

void procesarMover(Partida* game, GraficadorPantalla* gp, Cliente* client);

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

Partida* generarPartida(mapa_inicial data) {
	while(!data.tipos.empty()) {
		msg_tipo_entidad* act = data.tipos.front();
		data.tipos.pop_front();
		FactoryEntidades::obtenerInstancia()->agregarEntidad(*act);
		delete act;
	}

	Escenario* scene = new Escenario(data.mInfo.coordX, data.mInfo.coordY);
	Partida* game = new Partida();

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
			Jugador* nuevo = new Jugador(jugador_act->jInfo.name, jugador_act->jInfo.id, jugador_act->jInfo.color);
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

				
				nuevo->verVision()->setFromArray(jugador_act->varray);
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
			}
		}
	}

	game->asignarEscenario(scene);
	return game;
}


#define CODE_CONTINUE 1
#define CODE_RESET -2
#define CODE_EXIT -1


#define TOL_SEL_MULT 9 // Si solo dibujo para la seleccion un
// cuadradito de 9x9 no es seleccion multiple sino de una sola unidad
void procesarSeleccion(Partida* game, GraficadorPantalla* gp, Jugador* player){
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	

	float pX = cu->obtenerCoordLogicaX(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
	float pY = cu->obtenerCoordLogicaY(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
	
	if((game->sx == 0)&&(game->sy == 0)){
		game->sx = mx;
		game->sy = my;
		game->sx2 = mx + TOL_SEL_MULT;
		game->sy2 = my + TOL_SEL_MULT;
	}
	
	// Si me paso del borde del screen_frame, me voy...
	if(my > (gp->screen_height*360/480)){
		if(mx < (gp->screen_width*170/640)){ 
			// ...A menos que haya clickeado un boton
			cout << "BOTONNN!!!" << endl;
			}
		else
			return;
		}

	Posicion slct = Posicion(pX, pY);
	game->deseleccionarEntidades();
	if(game->escenario->verMapa()->posicionPertenece(&slct))
		if(player->visionCasilla(slct) != VIS_NO_EXPLORADA)
			if(!game->escenario->verMapa()->posicionEstaVacia(&slct))
				game->seleccionarEntidad(game->escenario->verMapa()->verContenido(&slct), true);

//	cout << "(x, y, x2, y2) = (" << game->sx << ","<< game->sy<< "," << game->sx2<< "," << game->sy2<< ")" <<endl;
}

void procesarSeleccionMultiple(Partida* game, GraficadorPantalla* gp, Jugador* player){
	int dif1 = game->sx2-game->sx;
	int dif2 = game->sy2-game->sy;
	if((dif1>(-TOL_SEL_MULT))&&(dif1<TOL_SEL_MULT)&&(dif2<TOL_SEL_MULT)&&(dif2>(-TOL_SEL_MULT)))
		return;
	
	// Reacomodo las coordenadas del rectangulo
	int ax1 = game->sx, ax2 = game->sx2;
	int ay1 = game->sy, ay2 = game->sy2;

	if((game->sx2 < game->sx) &&(game->sy2 < game->sy)){
		ax2 = game->sx;
		ay2 = game->sy;
		ax1 = game->sx2;
		ay1 = game->sy2;
		}
	else if(game->sx2 < game->sx){
		ax2 = game->sx;
		ax1 = game->sx2;
		}
	else if(game->sy2 < game->sy){
		ay2 = game->sy;
		ay1 = game->sy2;
		}

	int i, j;
	game->deseleccionarEntidades();
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	set<Posicion, lex_compare> usados;

	// Bucle mágico que itera dentro de cada casilla del rectangulo
	for(i = ax1; i < ax2; i+= 26)
		for(j = ay1; j < ay2; j+= 16){
			float pX = cu->obtenerCoordLogicaX(i, j, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
			float pY = cu->obtenerCoordLogicaY(i, j, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
			
			Posicion pAct = Posicion(pX, pY);
		//	cout << "PxPY = " << pX << "," << pY << endl;
			Posicion pAux = Posicion(pAct.getRoundX(), pAct.getRoundY());

			if(game->escenario->verMapa()->posicionPertenece(&pAct))
				if(player->visionCasilla(pAct) != VIS_NO_EXPLORADA)
					if(!game->escenario->verMapa()->posicionEstaVacia(&pAct))
						if(game->escenario->verMapa()->verContenido(&pAct)->verTipo() == ENT_T_UNIT)
							if(player->poseeEntidad(game->escenario->verMapa()->verContenido(&pAct)))
								if(usados.count(pAux) == 0){
								game->seleccionarEntidad(game->escenario->verMapa()->verContenido(&pAct), false);
								usados.insert(pAux);
								}
			}

}


Posicion adyacenteSiguiente(Posicion pos, int i, Escenario* scene){
	Posicion nueva;
	switch(i){
		case 0:
			nueva = Posicion(pos.getX() -1, pos.getY() +1);
			break;
		case 1:
			nueva = Posicion(pos.getX() -1, pos.getY());
			break;
		case 2:
			nueva = Posicion(pos.getX() -1, pos.getY() -1);
			break;
		case 3:
			nueva = Posicion(pos.getX(), pos.getY() -1);
			break;
		case 4:
			nueva = Posicion(pos.getX() +1, pos.getY() -1);
			break;
		case 5:
			nueva = Posicion(pos.getX() +1, pos.getY());
			break;
		case 6:
			nueva = Posicion(pos.getX() +1, pos.getY() +1);
			break;
		case 7:
			nueva = Posicion(pos.getX() , pos.getY() +1);
			break;
		}

	if(!scene->verMapa()->posicionPertenece(&nueva))
		return pos;
	else
		return nueva;
}

void procesarRecolectar(Partida* game, GraficadorPantalla* gp, Cliente* client) {
	list<Entidad*> lasEnt = game->verListaEntidadesSeleccionadas();
	
	// Obtengo la posición del click (quizás convertir en una función ya que se repite en varios lados...
	Posicion dest = *game->seleccionSecundaria;
	float pX = dest.getX();
	float pY = dest.getY();


//	if (game->escenario->verMapa()->posicionPertenece(&dest)) 
//		if(game->escenario->verMapa()->posicionEstaVacia(&dest)){

			for (list<Entidad*>::iterator it=lasEnt.begin(); it != lasEnt.end(); ++it){
				if((*it)->habilidades[ACT_COLLECT]){
					msg_event newEvent;
					newEvent.idEntidad = (*it)->verID();
					newEvent.accion = MSJ_RECOLECTAR;
					int i = 0;
					dest = Posicion(pX, pY);

					// La posta
					Entidad* aux = game->escenario->verMapa()->verContenido(&dest);
					newEvent.extra1 = (float)aux->verID();

				//	newEvent.extra1 = dest.getRoundX() +0.5;
				//	newEvent.extra2 = dest.getRoundY() +0.5;
				//	newEvent.extra1 = dest.getRoundX() + 0.8;
				//	newEvent.extra2 = dest.getRoundY() + 0.8;
					cout<<(*it)->name << " debe recolectar (" << dest.getRoundX() << "," << dest.getRoundY() << ")" << endl;
				
					client->agregarEvento(newEvent);
				}
		
			}
}


void procesarAtacar(Partida* game, GraficadorPantalla* gp, Cliente* client) {
	list<Entidad*> lasEnt = game->verListaEntidadesSeleccionadas();
	
	// Obtengo la posición del click (quizás convertir en una función ya que se repite en varios lados...
	Posicion dest = *game->seleccionSecundaria;
	float pX = dest.getX();
	float pY = dest.getY();


//	if (game->escenario->verMapa()->posicionPertenece(&dest)) 
//		if(game->escenario->verMapa()->posicionEstaVacia(&dest)){
			for (list<Entidad*>::iterator it=lasEnt.begin(); it != lasEnt.end(); ++it){
				if((*it)->habilidades[ACT_ATACK]){
						msg_event newEvent;
						newEvent.idEntidad = (*it)->verID();
						newEvent.accion = MSJ_ATACAR;
						int i = 0;
						dest = Posicion(pX, pY);

						// La posta
						Entidad* aux = game->escenario->verMapa()->verContenido(&dest);
						newEvent.extra1 = (float)aux->verID();

					//	newEvent.extra1 = dest.getRoundX() +0.5;
					//	newEvent.extra2 = dest.getRoundY() +0.5;
					//	newEvent.extra1 = dest.getRoundX() + 0.8;
					//	newEvent.extra2 = dest.getRoundY() + 0.8;
						cout<<(*it)->name << " debe atacar (" << dest.getRoundX() << "," << dest.getRoundY() << ")" << endl;
				
						client->agregarEvento(newEvent);
				//	}
				}
		
			}
}

void procesarSeleccionSecundaria(Partida* game, GraficadorPantalla* gp, Cliente* client, Jugador* player) {
	list<Entidad*> lasEnt = game->verListaEntidadesSeleccionadas();
	if (lasEnt.empty())
		return;
	
	if (lasEnt.front()->verJugador()->verID() != client->playerID)
		return;

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	float pX = cu->obtenerCoordLogicaX(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
	float pY = cu->obtenerCoordLogicaY(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
	Posicion* dest = new Posicion(pX, pY);
	
	if(game->seleccionSecundaria != nullptr){
			delete game->seleccionSecundaria;
			game->seleccionSecundaria = nullptr;
			}
	game->seleccionSecundaria = dest;


	if (game->escenario->verMapa()->posicionPertenece(dest)){ 
		if(game->escenario->verMapa()->posicionEstaVacia(dest)){
			if(game->verEntidadSeleccionada()->verTipo() == ENT_T_UNIT)
				// Accion de mover seleccionado(s)
				procesarMover(game, gp, client);
			else{
				delete game->seleccionSecundaria;
				game->seleccionSecundaria = nullptr;				
				}
			}
		else{ // Si la selec. secundaria es una Entidad...
			Entidad* target = game->escenario->verMapa()->verContenido(dest);
			Posicion* posTarg = target->verPosicion();
			entity_type_t tipo = target->verTipo();
			if(tipo == ENT_T_RESOURCE){
				// Procesar Recolectar
				procesarRecolectar(game, gp, client);
				}
			else if(!player->poseeEntidad(target)){
				// Procesar Atacar
				procesarAtacar(game, gp, client);
				}
			else{
				delete game->seleccionSecundaria;
				game->seleccionSecundaria = nullptr;				
				}
			}
		}

}

void procesarMover(Partida* game, GraficadorPantalla* gp, Cliente* client) {
	list<Entidad*> lasEnt = game->verListaEntidadesSeleccionadas();
	// Obtengo la posición del click (quizás convertir en una función ya que se repite en varios lados...
	Posicion dest = *game->seleccionSecundaria;
	float pX = dest.getX();
	float pY = dest.getY();
	
	set<Posicion, lex_compare> destinos;
	set<Posicion, lex_compare> ocupadas;

//	if (game->escenario->verMapa()->posicionPertenece(&dest)) 
//		if(game->escenario->verMapa()->posicionEstaVacia(&dest)){
			for (list<Entidad*>::iterator it=lasEnt.begin(); it != lasEnt.end(); ++it){
				msg_event newEvent;
				newEvent.idEntidad = (*it)->verID();
				newEvent.accion = MSJ_MOVER;
				int i = 0;
				dest = Posicion(pX, pY);
				// Carga en dest una posicion cercana y disponible
				// Revisar? Posible causa del lag misterioso?
				while(destinos.count(dest)){
					Posicion p = adyacenteSiguiente(dest, i, game->escenario);
					if(ocupadas.count(p))
						i++;
					else if(!game->escenario->verMapa()->posicionEstaVacia(&p))
						ocupadas.insert(p);
					else
						dest = p;
					}

				destinos.insert(dest);
				ocupadas.insert(dest);

				newEvent.extra1 = dest.getRoundX() +0.5;
				newEvent.extra2 = dest.getRoundY() +0.5;
			//	newEvent.extra1 = dest.getRoundX() + 0.8;
			//	newEvent.extra2 = dest.getRoundY() + 0.8;

				client->agregarEvento(newEvent);
		//	}
		cout << "Se generó un mover!!" << endl;
		
	}
}

int procesarEvento(Partida* game, GraficadorPantalla* gp, SDL_Event evento, Cliente* client, Jugador* player){
	switch (evento.type) {
	case SDL_MOUSEBUTTONDOWN:
		if(game->seleccionSecundaria != nullptr){
			delete game->seleccionSecundaria;
			game->seleccionSecundaria = nullptr;
			}

		if( evento.button.button == SDL_BUTTON_LEFT){
			game->algoSeleccionado = true;
			procesarSeleccion(game, gp, player);
			return CODE_CONTINUE;
			}
		else if( evento.button.button == SDL_BUTTON_RIGHT){
			procesarSeleccionSecundaria(game, gp, client, player);
		//	procesarMover(game, gp, client);
		//	game->deseleccionarEntidades();
			return CODE_CONTINUE;
			}

	case SDL_MOUSEBUTTONUP:
		procesarSeleccionMultiple(game, gp, player);
		game->algoSeleccionado = false;
		game->sx = 0;
		game->sy = 0;
		game->sx2 = 0;
		game->sy2 = 0;
		return CODE_CONTINUE;
			
	case SDL_QUIT:
		// enviar a proposito msg de log out?

		return CODE_EXIT; 
	}
}

//------------------------------------
//------------- MAIN -----------------
//------------------------------------



int wmain(int argc, char* argv[]) {

	// Cargar YAML
	ConfigParser parser = ConfigParser();
	parser.setPath(ARCHIVO_YAML);
	parser.parsearTodo();

	ErrorLog::getInstance()->escribirLog("----INICIANDO----");


	// Después modificar el método para que tome como parámetro el puerto y la ip del yaml
	SOCKET connectSocket = inicializarConexion(parser.verInfoRed());
	if (connectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		// Salir elegantemente;
		closesocket(connectSocket);
		return 1;
	}
	
	ErrorLog::getInstance()->escribirLog("Conexion establecida");
	
	// Cargar imagenes del YAML
	cargarBibliotecaImagenes(parser.verInfoEntidades());


	// Intento establecer conexión
	struct mapa_inicial elMapa;
	Partida* game;
	Cliente client = Cliente(connectSocket);
	if ( client.login(parser.verInfoRed()) ) {
		elMapa = client.getEscenario();
		printf("Tam mapa recibido: %d - %d\n", elMapa.mInfo.coordX, elMapa.mInfo.coordY);
		game = generarPartida(elMapa);
	} else {
		printf("Unable to login!\n");
		closesocket(connectSocket);
		return 1;
	}
	
	// Inicializar pantalla y graficador
	pantallaInfo_t pInfo = parser.verInfoPantalla();
	GraficadorPantalla* gp = new GraficadorPantalla(pInfo.screenW, pInfo.screenH, pInfo.fullscreen, "DEFAULT");

	SDL_Window* gameWindow = gp->getVentana();
	SDL_Surface* gameScreen = gp->getPantalla();
	gp->asignarParametrosScroll(parser.verInfoGameplay().scroll_margen, parser.verInfoGameplay().scroll_vel);	
	BibliotecaDeImagenes::obtenerInstancia()->asignarPantalla(gameScreen);

	gp->asignarPartida(game);
	Jugador* playerActual = game->obtenerJugador(parser.verInfoRed().ID);
	gp->asignarJugador(playerActual);
	playerActual->settearConexion(true);
	// En este punto el cliente ya está conectado
	client.start();
	
	/*
	list<Posicion> visionHard;
	for(int i = 0; i < game->escenario->verTamX(); i++)
		for(int j = 0; j < game->escenario->verTamY(); j++){
			Posicion aux(i, j);
			visionHard.push_back(aux);
		}
	game->obtenerJugador(parser.verInfoRed().ID)->agregarPosiciones(visionHard);
	*/

	int codigo_programa = CODE_CONTINUE;

	while (codigo_programa != CODE_EXIT && !client.must_close) {
		float timeA = SDL_GetTicks();
		client.procesarUpdates(game);

		game->avanzarFrame();
	
		gp->dibujarPantalla();
		client.generarKeepAlive();

		SDL_Event evento;
		while(SDL_PollEvent(&evento) != 0){
			// Generar msg_evento: Por ahora sólo te deja scrollear
			codigo_programa = procesarEvento(game, gp, evento, &client, playerActual);
			if(codigo_programa == CODE_EXIT)
				break;
		}

		float timeB = SDL_GetTicks();
		if((FRAME_DURATION - timeB + timeA) > 0)
			SDL_Delay(FRAME_DURATION - timeB + timeA);
		// cout << timeB - timeA << endl;
	}

	closesocket(connectSocket);
	WSACleanup();
	return 0;

}