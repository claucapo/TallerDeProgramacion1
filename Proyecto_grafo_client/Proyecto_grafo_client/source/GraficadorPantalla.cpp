#include "GraficadorPantalla.h"
#include "Spritesheet.h"
#include "Escenario.h"
#include "Partida.h"
#include "Recurso.h"
#include "Edificio.h"
#include "ErrorLog.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Unidad.h"
#include "ConversorUnidades.h"
#include "BibliotecaDeImagenes.h"
#include "FactoryEntidades.h"
#include <list>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <queue>
#include <string.h>

using namespace std;

GraficadorPantalla::GraficadorPantalla(void): pantalla(NULL) {}

GraficadorPantalla::~GraficadorPantalla(void){}

#define VIEW_X_DEFAULT (ancho_borde - screen_width)*0.5
#define VIEW_Y_DEFAULT 0
#define VEL_ZERO_DEFAULT 19
#define MARGEN_SCROLL_DEFAULT 66
GraficadorPantalla::GraficadorPantalla(int pant_width, int pant_height, bool full_screen, string title) {
	this->partida = nullptr;
	this->screen_height = pant_height;
	this->screen_width = pant_width;
	this->vel_scroll = VEL_ZERO_DEFAULT;
	this->margen_scroll = MARGEN_SCROLL_DEFAULT;
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	if(!cargarSDL(full_screen, title))
		delete this;
}

void GraficadorPantalla::asignarPartida(Partida* partida) {
	if (partida) {
		ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
		this->partida = partida;
		this->alto_borde = cu->convertULToPixels(partida->escenario->verTamX());
		this->ancho_borde = 1.732 * cu->convertULToPixels(partida->escenario->verTamY());

		this->view_x = VIEW_X_DEFAULT;
		this->view_y = VIEW_Y_DEFAULT;
	}
}

void GraficadorPantalla::dibujarPantalla(void) {
	// PASOS DEL DIBUJO DE LA PANTALLA
	if (!this->partida->escenario) {
		ErrorLog::getInstance()->escribirLog("FATAL: No hay Escenario que graficar!" , LOG_ERROR);
		return;
		}
	if (!this->partida->obtenerJugador(0)) {
		ErrorLog::getInstance()->escribirLog("FATAL: No hay Jugador al que graficar!" , LOG_ERROR);
		return;
		}
	// 0) Limpiar pantalla
	SDL_FillRect(pantalla, NULL, 0x000000);

	// 1) Scroll del mapa (cambiar view_x y view_y)
	reajustarCamara();

	// 2) Dibujar el Escenario
	renderizarTerreno();

	// 3) Dibujar los edificios
	renderizarEntidades();
	
	// 5) Dibujar el screen frame alrededor de la pantalla
	int minimapX, minimapY, minimapW, minimapH;
	dibujarMarcoPantalla(&minimapX, &minimapY, &minimapW, &minimapH);

	if(this->partida->modoUbicarEdificio)
		dibujarEdificioUbicandose();

	// 6) Dibujar el minimapa
	dibujarMinimapa(minimapX, minimapY, minimapW, minimapH);
				
	// 7) Window_Update
	SDL_UpdateWindowSurface(ventana);
}


// PASO 1: reajustar la c�mara de acuerdo a la posici�n del mouse
#define K_SCREEN 0.2	// Constante de scroll
void GraficadorPantalla::reajustarCamara(void) {
	float new_view_x = view_x, new_view_y = view_y;

	int mx = 5 , my = 9;
	SDL_GetMouseState(&mx, &my);

	if(mx < margen_scroll)
		new_view_x += -vel_scroll + mx * K_SCREEN;
	else if(mx > screen_width - margen_scroll)
		new_view_x += vel_scroll*(1 - screen_width/margen_scroll) + vel_scroll*mx/(margen_scroll);

	if(my < margen_scroll)
		new_view_y += -vel_scroll + my * K_SCREEN;
	else if(my> screen_height - margen_scroll)
		new_view_y += vel_scroll*(1 - screen_height/margen_scroll) + vel_scroll*my/(margen_scroll);

	// Obtengo las coordenadas del punto central de la pantalla
	float x_centPant = screen_width / 2;
	float y_centPant = screen_height / 2;
	
	ConversorUnidades* cu =  ConversorUnidades::obtenerInstancia();
	float x_central = cu->obtenerCoordLogicaX(x_centPant, y_centPant, new_view_x, new_view_y, ancho_borde);
	float y_central = cu->obtenerCoordLogicaY(x_centPant, y_centPant, new_view_x, new_view_y, ancho_borde);
	
	// Si la posicion central no tiene una casilla en pantalla, return
	Posicion pos_central = Posicion(x_central, y_central);
	if(!partida->escenario->verMapa()->posicionPertenece(&pos_central))
		return;

	view_x = new_view_x;
	view_y = new_view_y;
}


// PASO 2: renderizar terreno
#define TRANSPARENCIA_COLOR_MOD 160 
void GraficadorPantalla::renderizarTerreno(void) {
	SDL_Rect rectangulo;

	// Tile regular
	SDL_Surface* imgTile = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tile");
	SDL_SetColorKey(imgTile, true, SDL_MapRGB(imgTile->format, 255, 255, 255));

	// Tile regular sombreado
	SDL_Surface* imgAux = SDL_CreateRGBSurface(0, imgTile->w, imgTile->h,32,0,0,0,0); 
	SDL_SetSurfaceColorMod(imgTile, TRANSPARENCIA_COLOR_MOD , TRANSPARENCIA_COLOR_MOD , TRANSPARENCIA_COLOR_MOD );
	SDL_BlitSurface( imgTile, NULL, imgAux, NULL );

	SDL_Surface* imgTileFog = SDL_ConvertSurface(imgAux, pantalla->format, NULL);
	SDL_FreeSurface(imgAux);
	SDL_SetSurfaceColorMod(imgTile, 255 , 255 , 255);
	SDL_SetColorKey(imgTileFog, true, SDL_MapRGB(imgTileFog->format, 0, 0, 0));

	// Tile de agua
	SDL_Surface* imgTileWater = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tileWater");
	SDL_SetColorKey(imgTileWater, true, SDL_MapRGB(imgTile->format, 255, 255, 255));

	// Tile de agua sombreado
	imgAux = SDL_CreateRGBSurface(0, imgTileWater->w, imgTileWater->h,32,0,0,0,0); 
	SDL_SetSurfaceColorMod(imgTileWater, TRANSPARENCIA_COLOR_MOD , TRANSPARENCIA_COLOR_MOD , TRANSPARENCIA_COLOR_MOD );
	SDL_BlitSurface( imgTileWater, NULL, imgAux, NULL );
	
	SDL_Surface* imgTileWaterFog = SDL_ConvertSurface(imgAux, pantalla->format, NULL);
	SDL_FreeSurface(imgAux);
	SDL_SetSurfaceColorMod(imgTileWater, 255 , 255 , 255);
	SDL_SetColorKey(imgTileWaterFog, true, SDL_MapRGB(imgTileWaterFog->format, 0, 0, 0));


	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	Vision* vis = this->player->verVision();

	for(int i = 0; i < partida->escenario->verTamX(); i++) {
		for(int j = 0; j < partida->escenario->verTamY(); j++){
			Posicion pAct = Posicion(i,j);
			if(vis->visibilidadPosicion(pAct) != VIS_NO_EXPLORADA){
				rectangulo.x = cu->obtenerCoordPantallaX(i, j, view_x, view_y, ancho_borde) - 26;

				// Esto chequea si la casilla cae dentro del la pantalla... si no resulta visible, no la grafica
				if ((rectangulo.x) < (this->screen_width) && (rectangulo.x + imgTile->w) > 0) {
					rectangulo.y = cu->obtenerCoordPantallaY(i, j, view_x, view_y, ancho_borde);
					if ((rectangulo.y) < (this->screen_height) && (rectangulo.y + imgTile->h) > 0) {
						if(vis->visibilidadPosicion(Posicion(i,j)) == VIS_VISITADA){
							if (partida->escenario->verMapa()->verTipoTerreno(pAct) == TERRAIN_WATER) {
								SDL_BlitSurface( imgTileWaterFog, NULL, pantalla, &rectangulo );
							} else {
								SDL_BlitSurface( imgTileFog, NULL, pantalla, &rectangulo );
							}
						} else {
							if (partida->escenario->verMapa()->verTipoTerreno(pAct) == TERRAIN_WATER) {
								SDL_BlitSurface( imgTileWater, NULL, pantalla, &rectangulo );
							} else {
								SDL_BlitSurface( imgTile, NULL, pantalla, &rectangulo );
							}
						}
					}
				}
			}
		}
	}

	SDL_FreeSurface(imgTileFog);
	SDL_FreeSurface(imgTileWaterFog);

	// Muestro las posiciones de las entidades seleccionadas
	//SDL_SetSurfaceColorMod(imgTile, 110 , 255 , 110 );

	list<Posicion*> selec = this->partida->verSeleccionados();
	if(!selec.empty()){
		imgAux = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tileSlct2");
		if(this->partida->seleccionSecundaria != nullptr){
			Posicion pAct = *this->partida->seleccionSecundaria;
			rectangulo.x = cu->obtenerCoordPantallaX(pAct.getRoundX(), pAct.getRoundY(), view_x, view_y, ancho_borde) - 26;
			rectangulo.y = cu->obtenerCoordPantallaY(pAct.getRoundX(), pAct.getRoundY(), view_x, view_y, ancho_borde);
			SDL_BlitSurface( imgAux, NULL, pantalla, &rectangulo );	
		
			if(!this->partida->escenario->verMapa()->posicionEstaVacia(&pAct)){
				Entidad* target = this->partida->escenario->verMapa()->verContenido(&pAct);
				Posicion* pTar = target->verPosicion();
				int i, j;
				for(i = 0; i < target->verTamX();i++)
					for(j = 0; j < target->verTamY();j++){
						rectangulo.x = cu->obtenerCoordPantallaX(pTar->getRoundX() +i, pTar->getRoundY() +j, view_x, view_y, ancho_borde) - 26;
						rectangulo.y = cu->obtenerCoordPantallaY(pTar->getRoundX() +i, pTar->getRoundY() +j, view_x, view_y, ancho_borde);
						SDL_BlitSurface( imgAux, NULL, pantalla, &rectangulo );	
						}

				}
		}
		imgAux = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("tileSlct");

		list<Entidad*> entSel = this->partida->verListaEntidadesSeleccionadas();
		if(entSel.size() == 1){
			for(list<Posicion*>::iterator it=selec.begin(); it != selec.end(); it++){
				Posicion* pAct = (*it);
				rectangulo.x = cu->obtenerCoordPantallaX(pAct->getRoundX(), pAct->getRoundY(), view_x, view_y, ancho_borde) - 26;
				rectangulo.y = cu->obtenerCoordPantallaY(pAct->getRoundX(), pAct->getRoundY(), view_x, view_y, ancho_borde);
				SDL_BlitSurface( imgAux, NULL, pantalla, &rectangulo );	
			}
		}
		else{
			for(list<Entidad*>::iterator it=entSel.begin(); it != entSel.end(); it++){
				Posicion* pAct = (*it)->verPosicion();
				rectangulo.x = cu->obtenerCoordPantallaX(pAct->getRoundX(), pAct->getRoundY(), view_x, view_y, ancho_borde) - 26;
				rectangulo.y = cu->obtenerCoordPantallaY(pAct->getRoundX(), pAct->getRoundY(), view_x, view_y, ancho_borde);
				SDL_BlitSurface( imgAux, NULL, pantalla, &rectangulo );	
			}
		}

	}

						
}


// PASO 4: renderizar entidades
void GraficadorPantalla::renderizarEntidades(void) {
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	SDL_Rect rectangulo;
	vector<Entidad*> lEnt = partida->escenario->verEntidades();
	int cant_entidades = partida->escenario->verCantEntidades();
	for (int i = 0; i < cant_entidades; i++){
		Entidad** it = &lEnt[i];
		Posicion* posEntAct1 = (*it)->verPosicion();	

		bool must_show = !(*it)->habilidades[ACT_INVISIBLE]; // must_show dice si debe mostrarse (caso invisibilidad)
		must_show |= ((*it)->verEstado() == EST_ATACANDO);
		must_show |= this->player->poseeEntidad(*it);

		if(must_show){

			int tx = (*it)->verTamX(); int ty = (*it)->verTamY();
			Posicion posEntAct2 = Posicion(posEntAct1->getX() + tx - 1, posEntAct1->getY());
			Posicion posEntAct3 = Posicion(posEntAct1->getX(), posEntAct1->getY() + ty - 1);
			Posicion posEntAct4 = Posicion(posEntAct1->getX() + tx - 1, posEntAct1->getY() + ty - 1); 
			Vision* vis = this->player->verVision();
			// Si se ve alguna de las cuatro posiciones del borde, dibujo la entidad
			bool entEsVisible= false;
			if((*it)->verTipo() == ENT_T_UNIT){
				entEsVisible |= (vis->visibilidadPosicion(*posEntAct1)== VIS_OBSERVADA);
				entEsVisible |= (vis->visibilidadPosicion(posEntAct2)== VIS_OBSERVADA);
				entEsVisible |= (vis->visibilidadPosicion(posEntAct3)== VIS_OBSERVADA);
				entEsVisible |= (vis->visibilidadPosicion(posEntAct4)== VIS_OBSERVADA);
		
			}
			else{
				entEsVisible |= (vis->visibilidadPosicion(*posEntAct1)!=VIS_NO_EXPLORADA);
				entEsVisible |= (vis->visibilidadPosicion(posEntAct2)!=VIS_NO_EXPLORADA);
				entEsVisible |= (vis->visibilidadPosicion(posEntAct3)!=VIS_NO_EXPLORADA);
				entEsVisible |= (vis->visibilidadPosicion(posEntAct4)!=VIS_NO_EXPLORADA);
			}
			if(entEsVisible){


				Spritesheet* entAct = (*it)->verSpritesheet();
				SDL_Surface* spEnt = entAct->devolverImagenAsociada();

				if(this->puntoEstaEnPantalla(entAct->getCoordX(), entAct->getCoordY()))
					if((*it)->verEstado() == EST_ATACANDO)
						if(!Mix_Playing(1))
							if((rand() % 14)==1){ // El 14 es un numero magico para que todo quede bien!!
						Mix_Chunk* snd = BibliotecaDeImagenes::obtenerInstancia()->devolverSonido((*it)->verNombre() + "_atk2");
						Mix_PlayChannel( 1, snd, 0 );
						}

				SDL_SetColorKey( spEnt, true, SDL_MapRGB(spEnt->format, 255, 0, 255) );
	
				int newX = (int) cu->obtenerCoordPantallaX((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
				int newY = (int) cu->obtenerCoordPantallaY((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
				entAct->cambirCoord(newX, newY);
				rectangulo.x = entAct->getCoordX();
				rectangulo.y = entAct->getCoordY();
				entity_type_t tipo = (*it)->verTipo(); 
				if((tipo == ENT_T_RESOURCE)||(tipo == ENT_T_UNIT))
					rectangulo.y += 12;
				
				if((*it)->habilidades[ACT_ARCHERY]){
					Unidad* elArcher = (Unidad*) (*it);
					if(elArcher->verEstado() == EST_ATACANDO)
						this->dibujarProyectil(elArcher);
					else
						if(elArcher->proyectil != nullptr){
							delete elArcher->proyectil;
							elArcher->proyectil = nullptr;
							}
				}
					

				SDL_Rect recOr;
				recOr.x = entAct->calcularOffsetX();
				recOr.y = entAct->calcularOffsetY();
				recOr.w = entAct->subImagenWidth();
				recOr.h = entAct->subImagenHeight();

				Jugador* playerOwner = (*it)->verJugador();
				if(playerOwner != nullptr)
					if(!playerOwner->estaConectado()){
						string nameGris = "G" + (*it)->verNombre();
						spEnt = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(nameGris);
						SDL_SetColorKey( spEnt, true, SDL_MapRGB(spEnt->format, 255, 0, 255) );
					}

		
				SDL_BlitSurface( spEnt, &recOr, pantalla, &rectangulo );	
			}
		}
	}
}


//	PASO 5: screen frame
#define POS_REL_MINIMAP_X 0.805
#define POS_REL_MINIMAP_Y 0.775
#define	HEIGHT_REL_MINIMAP 0.225
#define	WIDTH_REL_MINIMAP 0.331
void GraficadorPantalla::dibujarMarcoPantalla(int* minimapX, int* minimapY, int* minimapW, int* minimapH){
	DatosImagen* marcoData = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen("screen_frame");
	if(marcoData->path == "default.png")
		return;
	SDL_Surface* borde = marcoData->imagen;
	SDL_SetColorKey(borde, true, SDL_MapRGB(borde->format, 255, 0, 255));
	SDL_Rect rectangulo;
	rectangulo.x = 0;
	rectangulo.y = 0;
	rectangulo.h = screen_height;
	rectangulo.w = screen_width;
	SDL_BlitScaled( borde, NULL, pantalla, &rectangulo );
	if(minimapX)
		*minimapX = rectangulo.w * POS_REL_MINIMAP_X;
	if(minimapY)
		*minimapY = rectangulo.h * POS_REL_MINIMAP_Y;
	if(minimapH)
		*minimapH = rectangulo.h * HEIGHT_REL_MINIMAP;
	if(minimapW)
		*minimapW = rectangulo.w * WIDTH_REL_MINIMAP;

	list<Posicion*> selct = this->partida->verSeleccionados();
	if(!selct.empty()){
		SDL_Rect hb;
		int vidaMax, vidaAct;
		int newX, newY;

		Entidad* entPri = this->partida->verEntidadSeleccionada();
		if(this->player->poseeEntidad(entPri)){
			if(entPri->habilidades[ACT_BUILD]){
				list<string> edifConst = FactoryEntidades::obtenerInstancia()->verListaEdificios();
				SDL_Rect button;
				button.x = 5 * screen_width*0.0015625;
				button.y = 372 * screen_height*0.00208333;
				for (list<string>::iterator it=edifConst.begin(); it != edifConst.end(); ++it){
					string icName = (*it) + "_icon";
					SDL_Surface* icono = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(icName);
					SDL_BlitSurface(icono, NULL, pantalla, &button);
					// Si el mouse est� sobre el boton, muestro sus recursos
					int moX, moY;
					SDL_GetMouseState(&moX, &moY);
					bool mouseSobreBoton = moX > button.x;
					mouseSobreBoton &= moX < (button.x + BUTTON_SIZE);
					mouseSobreBoton &= moY > button.y;
					mouseSobreBoton &= moY < (button.y + BUTTON_SIZE);

					if(mouseSobreBoton){
						Entidad* prot = FactoryEntidades::obtenerInstancia()->obtenerEntidad(*it, 65500);
						std::stringstream food;
						food << prot->costo.comida;
						std::stringstream wood;
						wood << prot->costo.madera;
						std::stringstream gold;
						gold << prot->costo.oro;
						std::stringstream stone;
						stone << prot->costo.piedra;
						SDL_Surface* surfCosto = this->renderText(wood.str());
						rectangulo.x = 431 * screen_width*0.0015625;
						rectangulo.y = 2 * screen_height*0.00208333;
						rectangulo.h = 17;
						rectangulo.w = wood.str().length()* 11;
						SDL_BlitScaled(surfCosto, NULL, pantalla, &rectangulo);
						SDL_FreeSurface(surfCosto);
						surfCosto = this->renderText(food.str());
						rectangulo.x = 490 * screen_width*0.0015625;
						rectangulo.w = food.str().length()* 11;
						SDL_BlitScaled(surfCosto, NULL, pantalla, &rectangulo);
						SDL_FreeSurface(surfCosto);
						surfCosto = this->renderText(gold.str());
						rectangulo.x = 548 * screen_width*0.0015625;
						rectangulo.w = gold.str().length()* 11;
						SDL_BlitScaled(surfCosto, NULL, pantalla, &rectangulo);
						SDL_FreeSurface(surfCosto);
						surfCosto = this->renderText(stone.str());
						rectangulo.x = 600 * screen_width*0.0015625;
						rectangulo.w = stone.str().length()* 11;
						SDL_BlitScaled(surfCosto, NULL, pantalla, &rectangulo);
						SDL_FreeSurface(surfCosto);

						delete prot;
					}

					button.x += BUTTON_SIZE;
					if(button.x >= 165){
						button.x = 5 * screen_width*0.0015625;
						button.y += BUTTON_SIZE;
						}

					}

				}
			else if(entPri->verTipo() == ENT_T_BUILDING){
				Edificio* edif = (Edificio*) entPri;
				list<string> entL;
				for(int i = 0; i < MAX_ENTRENABLES; i++){
				//	cout << "Este edif. entrena: " << edif->entrenables[i] << endl;
					if(edif->entrenables[i] != "unknown" )
						entL.push_front(edif->entrenables[i]);
				}
				
				if(entL.size()){ // Si el edificio entrena...
					SDL_Rect button;
					button.x = 5 * screen_width*0.0015625;
					button.y = 372 * screen_height*0.00208333;
					for (list<string>::iterator it=entL.begin(); it != entL.end(); ++it){
						string icName = (*it) + "_icon";
					//	cout << icName << " sdfalj" << endl;
						SDL_Surface* icono = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(icName);
						SDL_BlitSurface(icono, NULL, pantalla, &button);
						// Si el mouse est� sobre el boton, muestro sus recursos
						int moX, moY;
						SDL_GetMouseState(&moX, &moY);
						bool mouseSobreBoton = moX > button.x;
						mouseSobreBoton &= moX < (button.x + BUTTON_SIZE);
						mouseSobreBoton &= moY > button.y;
						mouseSobreBoton &= moY < (button.y + BUTTON_SIZE);

						if(mouseSobreBoton){
							Entidad* prot = FactoryEntidades::obtenerInstancia()->obtenerEntidad(*it, 65500);
							std::stringstream food;
							food << prot->costo.comida;
							std::stringstream wood;
							wood << prot->costo.madera;
							std::stringstream gold;
							gold << prot->costo.oro;
							std::stringstream stone;
							stone << prot->costo.piedra;
							SDL_Surface* surfCosto = this->renderText(wood.str());
							rectangulo.x = 431 * screen_width*0.0015625;
							rectangulo.y = 2 * screen_height*0.00208333;
							rectangulo.h = 17;
							rectangulo.w = wood.str().length()* 11;
							SDL_BlitScaled(surfCosto, NULL, pantalla, &rectangulo);
							SDL_FreeSurface(surfCosto);
							surfCosto = this->renderText(food.str());
							rectangulo.x = 490 * screen_width*0.0015625;
							rectangulo.w = food.str().length()* 11;
							SDL_BlitScaled(surfCosto, NULL, pantalla, &rectangulo);
							SDL_FreeSurface(surfCosto);
							surfCosto = this->renderText(gold.str());
							rectangulo.x = 548 * screen_width*0.0015625;
							rectangulo.w = gold.str().length()* 11;
							SDL_BlitScaled(surfCosto, NULL, pantalla, &rectangulo);
							SDL_FreeSurface(surfCosto);
							surfCosto = this->renderText(stone.str());
							rectangulo.x = 600 * screen_width*0.0015625;
							rectangulo.w = stone.str().length()* 11;
							SDL_BlitScaled(surfCosto, NULL, pantalla, &rectangulo);
							SDL_FreeSurface(surfCosto);

							delete prot;
						}
						button.x += BUTTON_SIZE;
						if(button.x >= 165 * screen_width*0.0015625){
							button.x = 5 * screen_width*0.0015625;
							button.y += BUTTON_SIZE;
							}
						}

				}
			}
		}

		if(this->partida->verListaEntidadesSeleccionadas().size() == 1){
			SDL_Surface* texto;
			Posicion* unaPos = selct.front();
			Entidad* entSlct = partida->escenario->verMapa()->verContenido(unaPos);
			if(entSlct != nullptr){
			Jugador* playerOwner = entSlct->verJugador();
			if(playerOwner->verID() != 0){// Si el jugador no es gaia
				texto = this->renderText(playerOwner->verNombre());
				//rectangulo.x = 184 * screen_width/640;
				rectangulo.x = 184 * screen_width*0.0015625;
				//rectangulo.y = 429 * screen_height/480;
				rectangulo.y = 429 * screen_height*0.00208333;
				rectangulo.h = 18;
				rectangulo.w = playerOwner->verNombre().length()* 14;
				SDL_BlitScaled( texto, NULL, pantalla, &rectangulo );
				SDL_FreeSurface(texto);
			}
			string laEnt;
			string entName = partida->escenario->verMapa()->verContenido(unaPos)->verNombre();
			laEnt = entName;
			entName[0] = toupper(entName[0]);
			texto = this->renderText(entName);
		//	rectangulo.x = 184 * screen_width/640;
			rectangulo.x = 184 * screen_width*0.0015625;
		//	rectangulo.y = 454 * screen_height/480;
			rectangulo.y = 454 * screen_height*0.00208333;
			rectangulo.h = 18;
			rectangulo.w = entName.length()* 14;
			SDL_BlitScaled( texto, NULL, pantalla, &rectangulo );
			SDL_FreeSurface(texto);

			this->mostrarStatsEntidad(partida->escenario->verMapa()->verContenido(unaPos));
			// Icono
		//	rectangulo.x = 190 * screen_width/640;
			rectangulo.x = 190 * screen_width*0.0015625;
		//	rectangulo.y = 380 * screen_height/480;
			rectangulo.y = 380 * screen_height*0.00208333;
			SDL_Surface* icono = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(laEnt +"_icon");
			SDL_BlitSurface(icono, NULL, pantalla, &rectangulo);

			// Health bar
			entity_type_t tipo = partida->escenario->verMapa()->verContenido(unaPos)->verTipo();
			if((tipo == ENT_T_BUILDING)||(tipo == ENT_T_UNIT) || (tipo == ENT_T_CONSTRUCTION)){
				SDL_Rect hb;
			//	hb.x = 190 * screen_width/640;
				hb.x = 190 * screen_width*0.0015625;
			//	hb.y = 415 * screen_height/480;
				hb.y = 415 * screen_height*0.00208333;
				hb.h = 4;
				vidaMax = partida->escenario->verMapa()->verContenido(unaPos)->vidaMax;
				vidaAct = partida->escenario->verMapa()->verContenido(unaPos)->vidaAct;
				hb.w = vidaAct*36/vidaMax;
		
				SDL_FillRect(this->pantalla, &hb, SDL_MapRGB(this->pantalla->format, 16, 245, 4));
				
				hb.x += hb.w;
				hb.w = 36 - hb.w;
				
				SDL_FillRect(this->pantalla, &hb, SDL_MapRGB(this->pantalla->format, 251, 0, 0));

				// Encima del personaje:
				ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
				newX = (int) cu->obtenerCoordPantallaX(unaPos->getX(), unaPos->getY(), view_x, view_y, ancho_borde);
				newY = (int) cu->obtenerCoordPantallaY(unaPos->getX(), unaPos->getY(), view_x, view_y, ancho_borde);
				hb.x = newX - 10;
				hb.y = newY - 40;
				if((hb.y < 365) && (hb.y > 20)){
					hb.h = 2;
					hb.w = vidaAct*20/vidaMax;
					SDL_FillRect(this->pantalla, &hb, SDL_MapRGB(this->pantalla->format, 16, 245, 4));
					hb.x += hb.w;	
					hb.w = 19 - hb.w;
					SDL_FillRect(this->pantalla, &hb, SDL_MapRGB(this->pantalla->format, 251, 0, 0));
					}	
				}
			}
		} // Si hay mas de una entidad seleccionada
		else{
			list<Entidad*> lEnt = this->partida->verListaEntidadesSeleccionadas();
			int row = 0, col = 0;

			for (list<Entidad*>::iterator it=lEnt.begin(); it != lEnt.end(); ++it){
			// Icon
			//	rectangulo.x = 177 * screen_width/640 + col*28;
				rectangulo.x = 177 * screen_width*0.0015625 + col*28;
			//	rectangulo.y = 375 * screen_height/480 + row*28;
				rectangulo.y = 375 * screen_height*0.00208333 + row*28;
				rectangulo.w = 28;
				rectangulo.h = 28;
				SDL_Surface* icono = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen((*it)->verNombre() +"_icon");
				SDL_BlitScaled(icono, NULL, pantalla, &rectangulo);
				// Health bar
				entity_type_t tipo = (*it)->verTipo();
				if((tipo == ENT_T_BUILDING)||(tipo == ENT_T_UNIT)){
				SDL_Rect hb;
			//	rectangulo.x = 177 * screen_width/640 + col*28;
				hb.x = 177 * screen_width*0.0015625 + col*28;
			//	rectangulo.y = 372 * screen_height/480 + (row+1)*28;
				hb.y = 372 * screen_height*0.00208333 + (row+1)*28;
					hb.h = 4;
					vidaMax = (*it)->vidaMax;
					vidaAct = (*it)->vidaAct;
					hb.w = vidaAct*28/vidaMax;
		
					SDL_FillRect(this->pantalla, &hb, SDL_MapRGB(this->pantalla->format, 16, 245, 4));
				
					hb.x += hb.w;
					hb.w = 28 - hb.w;
				
					SDL_FillRect(this->pantalla, &hb, SDL_MapRGB(this->pantalla->format, 251, 0, 0));

					// Encima del personaje:
					ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
					newX = (int) cu->obtenerCoordPantallaX((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
					newY = (int) cu->obtenerCoordPantallaY((*it)->verPosicion()->getX(), (*it)->verPosicion()->getY(), view_x, view_y, ancho_borde);
					hb.x = newX - 10;
					hb.y = newY - 40;
					if((hb.y < 365) && (hb.y > 20)){
						hb.h = 2;
						hb.w = vidaAct*20/vidaMax;
						SDL_FillRect(this->pantalla, &hb, SDL_MapRGB(this->pantalla->format, 16, 245, 4));
						hb.x += hb.w;	
						hb.w = 19 - hb.w;
						SDL_FillRect(this->pantalla, &hb, SDL_MapRGB(this->pantalla->format, 251, 0, 0));
						}
					}
				col++;
				if(col > 7){
					col = 0;
					row++;
					}
			}
		}

	}

	// Muestro los recursos
	SDL_Surface* surfRes;
	stringstream pobAct, pobMax, recursitoM, recursitoC, recursitoO, recursitoP;

	// Madera
	recursitoM << this->player->verRecurso().madera;
	string recurso = recursitoM.str();
	surfRes = this->renderText(recurso);
	rectangulo.x = 27 * screen_width*0.0015625;
	rectangulo.y = 2 * screen_height*0.00208333;
	rectangulo.h = 17;
	rectangulo.w = recurso.length()* 11;
	SDL_BlitScaled( surfRes, NULL, pantalla, &rectangulo );
	SDL_FreeSurface(surfRes);
	recursitoM.seekp(0);
	recursitoM.clear();
	// Comida
	recursitoC << this->player->verRecurso().comida;
	recurso = recursitoC.str();
	surfRes = this->renderText(recurso);
	rectangulo.x = 90 * screen_width*0.0015625;
	rectangulo.w = recurso.length()* 11;
	SDL_BlitScaled( surfRes, NULL, pantalla, &rectangulo );
	SDL_FreeSurface(surfRes);
	recursitoC.seekp(0);
	recursitoC.clear();
	// Oro
	recursitoO << this->player->verRecurso().oro;
	recurso = recursitoO.str();
	surfRes = this->renderText(recurso);
	rectangulo.x = 155 * screen_width*0.0015625;
	rectangulo.w = recurso.length()* 11;
	SDL_BlitScaled( surfRes, NULL, pantalla, &rectangulo );
	SDL_FreeSurface(surfRes);
	recursitoO.seekp(0);
	recursitoO.clear();
	// Roca
	recursitoP << this->player->verRecurso().piedra;
	recurso = recursitoP.str();
	surfRes = this->renderText(recurso);
	rectangulo.x = 214 * screen_width*0.0015625;
	rectangulo.w = recurso.length()* 11;
	SDL_BlitScaled( surfRes, NULL, pantalla, &rectangulo );
	SDL_FreeSurface(surfRes);
	recursitoP.seekp(0);
	recursitoP.clear();
	// Population
	pobAct << this->player->poblacionAct;
	recurso = pobAct.str();
	surfRes = this->renderText(recurso);
	rectangulo.x = 270 * screen_width*0.0015625;
	rectangulo.w = recurso.length()* 11;
	SDL_BlitScaled( surfRes, NULL, pantalla, &rectangulo );
	SDL_FreeSurface(surfRes);
	pobMax << this->player->poblacionMax;
	recurso = pobMax.str();
	surfRes = this->renderText(recurso);
	rectangulo.x = 291 * screen_width*0.0015625;
	rectangulo.w = recurso.length()* 11;
	SDL_BlitScaled( surfRes, NULL, pantalla, &rectangulo );
	SDL_FreeSurface(surfRes);

	// Rectangulo de selecion
	if(partida->algoSeleccionado){

		int mx, my;
		SDL_GetMouseState(&mx, &my);
		partida->sx2 = mx;
		partida->sy2 = my;

		int ax1 = partida->sx, ax2 = mx;
		int ay1 = partida->sy, ay2 = my;

		if((mx < partida->sx) &&(my < partida->sy)){
			ax2 = partida->sx;
			ay2 = partida->sy;
			ax1 = mx;
			ay1 = my;
			}
		else if(mx < partida->sx){
			ax2 = partida->sx;
			ax1 = mx;
			}
		else if(my < partida->sy){
			ay2 = partida->sy;
			ay1 = my;
			}

		SDL_Rect pixel;
		pixel.h = 1;
		pixel.w = 1;
		int i, j;

		pixel.x = ax1;
		pixel.y = ay1;

		while(pixel.x <= ax2){
			SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 164));
			pixel.x += 1;
		}

		while(pixel.y <= ay2){
			SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 164));
			pixel.y += 1;
		}

		pixel.x = ax1;
		pixel.y = ay1;

		while(pixel.y <= ay2){
			SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 164));
			pixel.y += 1;
		}

		while(pixel.x <= ax2){
			SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 164));
			pixel.x += 1;
		}
	}

}

void GraficadorPantalla::dibujarEdificioUbicandose(){
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	if(my > (screen_height*300/480))
		return;

	float px = cu->obtenerCoordLogicaX(mx, my, view_x, view_y, ancho_borde);
	float py = cu->obtenerCoordLogicaY(mx, my, view_x, view_y, ancho_borde);
	Posicion pos(px, py);

	if(!this->partida->escenario->verMapa()->posicionPertenece(&pos))
		return;

	SDL_Rect rectangulo;
	rectangulo.x = cu->obtenerCoordPantallaX(pos.getRoundX(), pos.getRoundY(), view_x, view_y, ancho_borde);
	rectangulo.y = cu->obtenerCoordPantallaY(pos.getRoundX(), pos.getRoundY(), view_x, view_y, ancho_borde);

	DatosImagen* data = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen(partida->edificioAubicar);

	rectangulo.x -= data->origenX;
	rectangulo.y -= data->origenY;

	SDL_Surface* edif = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen(partida->edificioAubicar);
		
	SDL_Surface* aux = SDL_CreateRGBSurface(0,edif->w,edif->h,32,0,0,0,0);

	SDL_SetColorKey(edif, true, SDL_MapRGB(edif->format, 255, 0, 255) );
	SDL_SetColorKey(aux, true, SDL_MapRGB(aux->format, 255, 0, 255) );


	SDL_BlitSurface(edif, NULL, aux, NULL);
	
	SDL_SetColorKey(aux, true, SDL_MapRGB(aux->format, 0, 0, 0) );


	if(this->partida->edificioUbicablePuedeConstruirse(pos))
			SDL_SetSurfaceColorMod(aux, 40, 200, 40);
	else
			SDL_SetSurfaceColorMod(aux, 200, 40, 40);

	SDL_BlitSurface(aux, NULL, pantalla, &rectangulo);

	SDL_FreeSurface( aux);
}


//	PASO 6: dibujar el minimapa
void GraficadorPantalla::dibujarMinimapa(int minimapX, int minimapY, int minimapW, int minimapH){
	SDL_Rect pixel;
	pixel.h = 2;
	pixel.w = 2;
	int i, j;
	
	for(i = 0; i < this->partida->escenario->verTamX(); i++)
		for(j = 0; j < this->partida->escenario->verTamX(); j++){
			Posicion pAct = Posicion(i, j);
			if(this->player->visionCasilla(pAct) != VIS_NO_EXPLORADA){
				pixel.x = minimapX - 4;
				pixel.y = minimapY;
				ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
				pixel.x += floor(cu->obtenerCoordPantallaX(i, j, 0, 0, minimapW)/LARGO_TILE);
				pixel.y += floor(cu->obtenerCoordPantallaY(i, j, 0, 0, minimapW)/LARGO_TILE);

				if(this->partida->escenario->verMapa()->posicionEstaVacia(&pAct)){
					if(this->partida->escenario->verMapa()->verTipoTerreno(pAct) == TERRAIN_GRASS)
						SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 51, 151, 37));
					else
						SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 34, 94, 180));
					}
				else if(this->partida->escenario->verMapa()->verContenido(&pAct)->verTipo() == ENT_T_RESOURCE){
					Recurso* recur = (Recurso*) this->partida->escenario->verMapa()->verContenido(&pAct);
					if(recur->tipoR== RES_T_FOOD)
						SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 162, 221, 131));
					else if(recur->tipoR== RES_T_WOOD)
						SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 64, 98, 38));
					else if(recur->tipoR== RES_T_GOLD)
						SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 219, 220, 30));
					else
						SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 171, 186, 183));
				
				}
				else if(this->partida->escenario->verMapa()->verContenido(&pAct)->verJugador()->verID() == 0) // gaia
					SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 155, 236, 191));
				else if(this->partida->escenario->verMapa()->verContenido(&pAct)->verJugador()->verID() == 1) // player1: blue
					SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 10, 38, 163));
				else if(this->partida->escenario->verMapa()->verContenido(&pAct)->verJugador()->verID() == 2) // player2: red
					SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 71, 2));
				else if(this->partida->escenario->verMapa()->verContenido(&pAct)->verJugador()->verID() == 3) // player3: yellow
					SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 250, 10));
				else{
					SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 250, 200, 16));
				}
					
			}
		}
	// LA CAMARA	
	pixel.h = 1;
	pixel.w = 1;

	// Obtengo las coordenadas del minimapa de la view
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	float viewXlogico = cu->obtenerCoordLogicaX(0, 0, view_x, view_y, ancho_borde);
	float viewYlogico = cu->obtenerCoordLogicaY(0, 0, view_x, view_y, ancho_borde);
	
	float ultimoXlogico = cu->obtenerCoordLogicaX(this->screen_width, this->screen_height, view_x, view_y, ancho_borde);
	float ultimoYlogico = cu->obtenerCoordLogicaY(this->screen_width, this->screen_height, view_x, view_y, ancho_borde);
	
	float topeX = floor(cu->obtenerCoordPantallaX(ultimoXlogico, ultimoYlogico, 0, 0, minimapW)/LARGO_TILE);
	float topeY = floor(cu->obtenerCoordPantallaY(ultimoXlogico, ultimoYlogico, 0, 0, minimapW)/LARGO_TILE);
	
	pixel.x = minimapX - 4;
	pixel.y = minimapY;
	pixel.x += floor(cu->obtenerCoordPantallaX(viewXlogico, viewYlogico, 0, 0, minimapW)/LARGO_TILE);
	pixel.y += floor(cu->obtenerCoordPantallaY(viewXlogico, viewYlogico, 0, 0, minimapW)/LARGO_TILE);
	SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));

	while(pixel.x <= (topeX+minimapX - 4)){
		SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
		pixel.x += 1;
	}

	while(pixel.y <= (topeY+minimapY)){
		SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
		pixel.y += 1;
	}
		
	pixel.x = minimapX - 4;
	pixel.y = minimapY;
	pixel.x += floor(cu->obtenerCoordPantallaX(viewXlogico, viewYlogico, 0, 0, minimapW)/LARGO_TILE);
	pixel.y += floor(cu->obtenerCoordPantallaY(viewXlogico, viewYlogico, 0, 0, minimapW)/LARGO_TILE);

	while(pixel.y <= (topeY+minimapY)){
		SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
		pixel.y += 1;
	}

	while(pixel.x <= (topeX+minimapX - 4)){
		SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
		pixel.x += 1;
	}
	

SDL_FillRect(this->pantalla, &pixel, SDL_MapRGB(this->pantalla->format, 255, 255, 255));
				
}

// METODOS DE INICIALIZACION Y GETTERS
bool GraficadorPantalla::cargarSDL(bool full_screen, string title) {
	Uint32 flags = SDL_WINDOW_SHOWN;
	if (full_screen) 
		flags = flags || SDL_WINDOW_FULLSCREEN;

	if(SDL_Init( SDL_INIT_EVERYTHING ) < 0)	{
		ErrorLog::getInstance()->escribirLog("CR�TICO: SDL no pudo inicializarse." , LOG_ERROR);
		printf( "SDL Error: %s\n", SDL_GetError() );
		return false;
	} else {
		if (title.length() == 0) {
			title = "AGE OF TALLER DE PROGRAMACION I";
		}
		ventana = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, flags);
		if(!ventana) {
			ErrorLog::getInstance()->escribirLog("Ventana no pudo crearse" , LOG_ERROR);
			return false;
		} else {
			int flags = IMG_INIT_PNG;
			if(!(IMG_Init(flags) & flags)) {
				ErrorLog::getInstance()->escribirLog("CR�TICO: SDL no pudo inicializarse." , LOG_ERROR);
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				return false;
			} else {
				pantalla = SDL_GetWindowSurface(ventana);

				//Initialize SDL_mixer
				if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ){
					return false;    
				}
			}
		}
	}
			
	return true;
}


void GraficadorPantalla::asignarParametrosScroll(int margen, int velocidad) {
	if (velocidad > 0)
		this->vel_scroll = velocidad;
	else
		ErrorLog::getInstance()->escribirLog("Se intento asignar una velocidad de scroll inv�lida." , LOG_WARNING);

	if (margen <= 0) {
		ErrorLog::getInstance()->escribirLog("Se intento asignar un margen de scroll nulo." , LOG_WARNING);	
	} else if(margen > (this->screen_height/3) || margen > (this->screen_width/3) ) {
		ErrorLog::getInstance()->escribirLog("Se intento asignar un margen de scroll demasiado grande." , LOG_WARNING);	
	} else {
		this->margen_scroll = margen;
	}
}

SDL_Window* GraficadorPantalla::getVentana(void) {
	return this->ventana;
}

SDL_Surface* GraficadorPantalla::getPantalla(void) {
	return pantalla;
}

float GraficadorPantalla::getViewX(void){
	return this->view_x;
}

float GraficadorPantalla::getViewY(void){
	return this->view_y;
}

float GraficadorPantalla::getAnchoBorde(void) {
	return this->ancho_borde;
}


SDL_Surface* GraficadorPantalla::renderText(string msj){
	DatosImagen* fuenteData = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen("font");
	if(fuenteData->path == "default.png")
		return NULL;
	SDL_Surface* font = fuenteData->imagen;
	int largo = msj.length();
	SDL_Surface* elTexto = SDL_CreateRGBSurface(0, largo*16, 20,32,0,0,0,0); 
	if(elTexto == NULL)
		return NULL;
	// Relleno de blanco
	SDL_Rect aux;
	aux.x = 0;
	aux.y = 0;
	aux.h = 20;
	aux.w = largo*16;
	SDL_FillRect(elTexto, &aux, SDL_MapRGB(elTexto->format, 255, 255, 255));
	
	// Convierto caracter por caracter:
	int i;
	SDL_Rect rectOrig, rectDest;
	rectOrig.h = 20;
	rectOrig.w = 12;
	rectDest.h = 20;
	rectDest.w = 12;
	rectDest.y = 0;
	rectDest.x = 0;
	for(i = 0; i < largo; i++){
		bool printIt = false;
		if((msj[i] >= 'A') && (msj[i] <= 'Z')) {
			rectOrig.y = 0;
			rectOrig.x = (msj[i] - 'A') * 12 + 1;
			printIt = true;
			rectOrig.w = 12;
		}
		else if((msj[i] >= 'a') && (msj[i] <= 'z')) {
			rectOrig.y = 19;
			rectOrig.x = (msj[i] - 'a') * 12 + 2;
			rectOrig.w = 12;
			printIt = true;
		}
		else if((msj[i] >= '0') && (msj[i] <= '9')) {
			rectOrig.y = 39;
			rectOrig.x = (msj[i] - '0') * 12 + 2;
			rectOrig.w = 11;
			printIt = true;
		}
		if(printIt)
			SDL_BlitSurface(font, &rectOrig, elTexto, &rectDest);
		rectDest.x += rectOrig.w;
	}
	SDL_Surface* texto = SDL_ConvertSurface(elTexto, pantalla->format, NULL);
	SDL_FreeSurface(elTexto);
	SDL_SetColorKey(texto, true, SDL_MapRGB(texto->format, 255, 255, 255));
	return texto;
}

void GraficadorPantalla::mostrarPantallaInicio(bool show_name, string name){
	SDL_Surface* img = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("menu_bck");
	SDL_BlitSurface(img, NULL, pantalla, NULL);
	SDL_Rect dest;
	dest.y = 400;
	DatosImagen* data;
	if(show_name){
		data = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen("sign_name");
		dest.x = 320 - data->origenX;
		SDL_BlitSurface(data->imagen, NULL, pantalla, &dest);
		SDL_Surface* surfNombre = this->renderText(name);
		dest.x += 155; dest.y += 5;
		dest.h = 18;
		dest.w = 14 * name.length();	
		SDL_BlitScaled( surfNombre, NULL, pantalla, &dest );
		SDL_FreeSurface(surfNombre);
	}
	else{
		data = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen("sign_menu");
		dest.x = 320 - data->origenX;
		SDL_BlitSurface(data->imagen, NULL, pantalla, &dest);
	}
	SDL_UpdateWindowSurface(ventana);
}

bool GraficadorPantalla::puntoEstaEnPantalla(int xPant, int yPant){
	bool enPant = xPant > 0;
	enPant &= yPant > 0;
	enPant &= xPant < this->screen_width;
	enPant &= yPant < this->screen_height;
	return enPant;
}

void GraficadorPantalla::mostrarStatsEntidad(Entidad* ent){
		stringstream strStat; 
		SDL_Rect rectangulo;
		SDL_Surface* stat = NULL;
		BibliotecaDeImagenes* bu = BibliotecaDeImagenes::obtenerInstancia();
 		
		if((ent->verTipo() == ENT_T_BUILDING)||(ent->verTipo() == ENT_T_UNIT) || (ent->verTipo() == ENT_T_CONSTRUCTION)){
			// HP
			stat = this->renderText("HP");
			rectangulo.x = 230 * screen_width*0.0015625;
			rectangulo.y = 380 * screen_height*0.00208333;
			rectangulo.h = 18;
			rectangulo.w = 28;	
			SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );
			SDL_FreeSurface(stat);

			strStat << ent->vidaAct;
			stat = this->renderText(strStat.str());
			rectangulo.x = 257 * screen_width*0.0015625;
			rectangulo.y = 382 * screen_height*0.00208333;
			rectangulo.h = 17;
			rectangulo.w = strStat.str().length() * 11;	
			SDL_SetSurfaceColorMod(stat, 38, 37, 32);
			SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );
			SDL_FreeSurface(stat);
		
			strStat.seekp(0);
			strStat << "      ";
			strStat.seekp(0);

			// Armor
			if(ent->verTipo() == ENT_T_UNIT){
				stat = bu->devolverImagen("armor_icon");
				rectangulo.x = 227 * screen_width*0.0015625;
				rectangulo.y = 399 * screen_height*0.00208333;
				rectangulo.h = 18;
				rectangulo.w = 28;
				SDL_SetColorKey(stat, true, SDL_MapRGB(stat->format, 255, 255, 255));
				SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );

				strStat << ent->defensa;
				stat = this->renderText(strStat.str());
				rectangulo.x = 257 * screen_width*0.0015625;
				rectangulo.y = 401 * screen_height*0.00208333;
				rectangulo.h = 17;
				rectangulo.w = strStat.str().length() * 11;	
				SDL_SetSurfaceColorMod(stat, 38, 37, 32);
				SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );
				SDL_FreeSurface(stat);

				strStat.seekp(0);
				strStat << "      ";
				strStat.seekp(0);
				
				// Luck
				stat = bu->devolverImagen("luck_icon");
				rectangulo.x = 289 * screen_width*0.0015625;
				rectangulo.y = 380 * screen_height*0.00208333;
				rectangulo.h = 18;
				rectangulo.w = 28;	
				SDL_SetColorKey(stat, true, SDL_MapRGB(stat->format, 255, 255, 255));
				SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );

				strStat << ent->luck;
				stat = this->renderText(strStat.str());
				rectangulo.x = 323 * screen_width*0.0015625;
				rectangulo.y = 382 * screen_height*0.00208333;
				rectangulo.h = 17;
				rectangulo.w = strStat.str().length() * 11;	
				SDL_SetSurfaceColorMod(stat, 38, 37, 32);
				SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );
				SDL_FreeSurface(stat);
				strStat.seekp(0);
				strStat << "      ";
				strStat.seekp(0);
				}

			// Attack
			if(ent->habilidades[ACT_ATACK]){
				stat = bu->devolverImagen("attack_icon");
				rectangulo.x = 289 * screen_width*0.0015625;
				rectangulo.y = 399 * screen_height*0.00208333;
				rectangulo.h = 18;
				rectangulo.w = 28;	
				SDL_SetColorKey(stat, true, SDL_MapRGB(stat->format, 255, 255, 255));
				SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );

				strStat << ent->ataque;
				stat = this->renderText(strStat.str());
				rectangulo.x = 323 * screen_width*0.0015625;
				rectangulo.y = 401 * screen_height*0.00208333;
				rectangulo.h = 17;
				rectangulo.w = strStat.str().length() * 11;	
				SDL_SetSurfaceColorMod(stat, 38, 37, 32);
				SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );
				SDL_FreeSurface(stat);
				strStat.seekp(0);
				strStat << "      ";
				strStat.seekp(0);

				// Range
				if(ent->habilidades[ACT_ARCHERY]){
					stat = bu->devolverImagen("range_icon");
					rectangulo.x = 351 * screen_width*0.0015625;
					rectangulo.y = 380 * screen_height*0.00208333;
					rectangulo.h = 18;
					rectangulo.w = 28;	
					SDL_SetColorKey(stat, true, SDL_MapRGB(stat->format, 255, 255, 255));
					SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );

					strStat << (ent->verRango() - 1);
					stat = this->renderText(strStat.str());
					rectangulo.x = 385 * screen_width*0.0015625;
					rectangulo.y = 382 * screen_height*0.00208333;
					rectangulo.h = 17;
					rectangulo.w = strStat.str().length() * 11;	
					SDL_SetSurfaceColorMod(stat, 38, 37, 32);
					SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );
					strStat.seekp(0);
					strStat << "      ";
					strStat.seekp(0);
					SDL_FreeSurface(stat);
				}
			}
			// Construccion
			if(ent->verTipo() == ENT_T_BUILDING){
				Edificio* edi = (Edificio*) ent;
				if(edi->porcentajeEntrenado() != -1){
					stat = bu->devolverImagen(edi->cola_produccion.front().name + "_icon");
					rectangulo.x = 351 * screen_width*0.0015625;
					rectangulo.y = 380 * screen_height*0.00208333;
					rectangulo.h = 18;
					rectangulo.w = 28;	
					SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );

				}
			}
		}
		else if(ent->verTipo() == ENT_T_RESOURCE){
			switch(((Recurso*)ent)->tipoR){
			case RES_T_FOOD:
				stat = bu->devolverImagen("food_icon"); break;
			case RES_T_GOLD:
				stat = bu->devolverImagen("gold_icon"); break;
			case RES_T_STONE:
				stat = bu->devolverImagen("stone_icon"); break;
			case RES_T_WOOD:
				stat = bu->devolverImagen("wood_icon"); break;
				}
			rectangulo.x = 184 * screen_width*0.0015625;
			rectangulo.y = 429 * screen_height*0.00208333;
			rectangulo.h = 18;
			rectangulo.w = 28;
			SDL_SetColorKey(stat, true, SDL_MapRGB(stat->format, 255, 255, 255));
			SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );

			strStat << ((Recurso*)ent)->recursoAct;
			stat = this->renderText(strStat.str());
			rectangulo.x = 217 * screen_width*0.0015625;
			rectangulo.y = 429 * screen_height*0.00208333;
			rectangulo.h = 17;
			rectangulo.w = strStat.str().length() * 11;	
			SDL_SetSurfaceColorMod(stat, 38, 37, 32);
			SDL_BlitScaled( stat, NULL, pantalla, &rectangulo );
			SDL_FreeSurface(stat);
			strStat.seekp(0);
			strStat << "      ";
			strStat.seekp(0);

		}
}

void GraficadorPantalla::pantallaFinal(bool pantDerrota){
	SDL_Surface* img = NULL;
	if(pantDerrota)
		img = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("defeat_bck");
	else
		img = BibliotecaDeImagenes::obtenerInstancia()->devolverImagen("victory_bck");
	SDL_BlitSurface(img, NULL, pantalla, NULL);
	
	SDL_UpdateWindowSurface(ventana);
}


void GraficadorPantalla::dibujarProyectil(Unidad* uni){
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	
	if(uni->proyectil == nullptr){
		uni->proyectil = new Spritesheet(uni->verNombre() + "_arrow");
		uni->proyectil->cambirCoord(uni->verPosicion()->getX() *100, uni->verPosicion()->getY()*100);
		//uni->proyectil->setAnimationDelay(
	}

	
	Entidad* target = partida->escenario->obtenerEntidad(uni->targetID);
	if(target == nullptr)
		return;

	DatosImagen* dataIm = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen(uni->verNombre() + "_arrow");

	float x_viejo = (float)uni->proyectil->getCoordX()/100 + (float)dataIm->origenX/100;
	float y_viejo = (float)uni->proyectil->getCoordY()/100 + (float)dataIm->origenY/100;

	cout << uni->proyectil->getCoordX() << "-" << uni->proyectil->getCoordY() <<endl;
	cout << ((float)uni->proyectil->getCoordX())/100 << "-" << (float)uni->proyectil->getCoordY()/100 <<endl;


	float delta_x = (target->verPosicion()->getX() - (float)uni->proyectil->getCoordX()/100 - (float)dataIm->origenX/100);
	float delta_y = (target->verPosicion()->getY() - (float)uni->proyectil->getCoordY()/100 - (float)dataIm->origenY/100);
	
	delta_x /= 8;
	delta_y /= 8;

	float x_nuevo = (float)uni->proyectil->getCoordX()/100 + delta_x + (float)dataIm->origenX/100;
	float y_nuevo = (float)uni->proyectil->getCoordY()/100 + delta_y + (float)dataIm->origenY/100;
	
	int panX = cu->obtenerCoordPantallaX(x_nuevo, y_nuevo, view_x, view_y, ancho_borde);
	int panY = cu->obtenerCoordPantallaY(x_nuevo, y_nuevo, view_x, view_y, ancho_borde);

	

	float radX = target->verPosicion()->getX() - x_nuevo;
	float radY = target->verPosicion()->getY() - y_nuevo;

	float avance = radX * radX + radY * radY;

	bool seMantuvoLaPosicion = ((x_nuevo - x_viejo)*(x_nuevo - x_viejo)) < 0.03;
	seMantuvoLaPosicion &= ((y_nuevo - y_viejo)*(y_nuevo - y_viejo)) < 0.03;

	cout << "Viejo: (" << x_viejo << "," << y_viejo << ") Nuevo: ("<< x_nuevo << "," << y_nuevo << ")" << endl;

	if((avance < 0.0005) || seMantuvoLaPosicion){
		uni->proyectil->cambirCoord(uni->verPosicion()->getX()*100, uni->verPosicion()->getY()*100);
	}
	else
		uni->proyectil->cambirCoord(x_nuevo*100, y_nuevo*100);



	SDL_Surface* proyectil =  uni->proyectil->devolverImagenAsociada();
	SDL_SetColorKey(proyectil, true, SDL_MapRGB(proyectil->format, 255, 0, 255));

	SDL_Rect rectangulo;
	rectangulo.x = panX;
	rectangulo.y = panY;
	SDL_Rect recOr;
	uni->proyectil->cambiarSubImagen(0, uni->verDireccion());
	recOr.x = uni->proyectil->calcularOffsetX();
	recOr.y = uni->proyectil->calcularOffsetY();
	recOr.w = uni->proyectil->subImagenWidth();
	recOr.h = uni->proyectil->subImagenHeight();

	SDL_BlitSurface(proyectil, &recOr, pantalla, &rectangulo);

}

/*
void GraficadorPantalla::dibujarProyectil(Unidad* uni){
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	DatosImagen* dataIm = BibliotecaDeImagenes::obtenerInstancia()->devolverDatosImagen(uni->verNombre() + "_arrow");

	if(uni->proyectil == nullptr){
		uni->proyectil = new Spritesheet(uni->verNombre() + "_arrow");
		uni->proyectil->cambirCoord(uni->verPosicion()->getX() *100, uni->verPosicion()->getY()*100);
		//uni->proyectil->setAnimationDelay(
	}

	
	Entidad* target = partida->escenario->obtenerEntidad(uni->targetID);
	if(target == nullptr)
		return;

	
	float x_viejo = uni->proyectil->getCoordX()/100 + dataIm->origenX/100;
	float y_viejo = uni->proyectil->getCoordY()/100 + dataIm->origenY/100;

	float x_nuevo = x_viejo;
	float y_nuevo = y_viejo;

	switch(uni->verDireccion()){
	case DIR_DOWN:
		x_nuevo++; y_nuevo++; break;
	case DIR_DOWN_RIGHT:
		y_nuevo++; break;
	case DIR_DOWN_LEFT:
		x_nuevo++; break;
	case DIR_TOP:
		x_nuevo--; y_nuevo--; break;
	case DIR_TOP_LEFT:
		y_nuevo--; break;
	case DIR_TOP_RIGHT:
		x_nuevo--; break;
	case DIR_LEFT:
		x_nuevo++; y_nuevo--; break;
	case DIR_RIGHT:
		x_nuevo--; y_nuevo++; break;

	}

	int panX = cu->obtenerCoordPantallaX(x_nuevo, y_nuevo, view_x, view_y, ancho_borde);
	int panY = cu->obtenerCoordPantallaY(x_nuevo, y_nuevo, view_x, view_y, ancho_borde);

	

	float radX = target->verPosicion()->getX() - x_nuevo;
	float radY = target->verPosicion()->getY() - y_nuevo;

	float avance = radX * radX + radY * radY;

	bool seMantuvoLaPosicion = ((x_nuevo - x_viejo)*(x_nuevo - x_viejo)) < 0.03;
	seMantuvoLaPosicion &= ((y_nuevo - y_viejo)*(y_nuevo - y_viejo)) < 0.03;

	cout << "Viejo: (" << x_viejo << "," << y_viejo << ") Nuevo: ("<< x_nuevo << "," << y_nuevo << ")" << endl;

	if((avance < 0.0005) || seMantuvoLaPosicion){
		uni->proyectil->cambirCoord(uni->verPosicion()->getX()*100, uni->verPosicion()->getY()*100);
	}
	else
		uni->proyectil->cambirCoord(x_nuevo*100, y_nuevo*100);



	SDL_Surface* proyectil =  uni->proyectil->devolverImagenAsociada();
	SDL_SetColorKey(proyectil, true, SDL_MapRGB(proyectil->format, 255, 0, 255));

	SDL_Rect rectangulo;
	rectangulo.x = panX;
	rectangulo.y = panY;
	SDL_Rect recOr;
	uni->proyectil->cambiarSubImagen(0, uni->verDireccion());
	recOr.x = uni->proyectil->calcularOffsetX();
	recOr.y = uni->proyectil->calcularOffsetY();
	recOr.w = uni->proyectil->subImagenWidth();
	recOr.h = uni->proyectil->subImagenHeight();

	SDL_BlitSurface(proyectil, &recOr, pantalla, &rectangulo);

}*/