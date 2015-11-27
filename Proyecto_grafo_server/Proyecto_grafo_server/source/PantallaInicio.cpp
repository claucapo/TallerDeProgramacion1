#include "PantallaInicio.h"
#include "ErrorLog.h"
#include "Enumerados.h"
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <sstream>

PantallaInicio::PantallaInicio(void){
}


PantallaInicio::~PantallaInicio(void){
}


SDL_Surface* loadSurface(std::string path, SDL_Surface* pantalla) {
	SDL_Surface* optimizedSurface = NULL;

	//Carga la imagen del path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if(!loadedSurface)
		ErrorLog::getInstance()->escribirLog("SDL_image Error:  con imagen" +  path, LOG_ERROR);

	// Si hay una pantalla, optimizo el formato de la
	// imagen cargada al de la pantalla
	else if(pantalla){
		optimizedSurface = SDL_ConvertSurface(loadedSurface, pantalla->format, NULL);
		if(!optimizedSurface)
			ErrorLog::getInstance()->escribirLog("Error al optimizar: " + path, LOG_ERROR);
		SDL_FreeSurface(loadedSurface);
		return optimizedSurface;
	}
	return loadedSurface;
}

string procesarEventoTexto(SDL_Event evento){
	if(evento.type != SDL_KEYDOWN)
		return "error";
	
	string p;
	switch(evento.key.keysym.scancode){
	case SDL_SCANCODE_0:
		p = "0"; break;
	case SDL_SCANCODE_1:
		p = "1"; break;
	case SDL_SCANCODE_2:
		p = "2"; break;
	case SDL_SCANCODE_3:
		p = "3"; break;
	case SDL_SCANCODE_4:
		p = "4"; break;
	case SDL_SCANCODE_5:
		p = "5"; break;
	case SDL_SCANCODE_6:
		p = "6"; break;
	case SDL_SCANCODE_7:
		p = "7"; break;
	case SDL_SCANCODE_8:
		p = "8"; break;
	case SDL_SCANCODE_9:
		p = "9"; break;
	case SDL_SCANCODE_BACKSPACE:
		p = "erase"; break;
	case SDL_SCANCODE_RETURN:
		p = "finish"; break;	
	default:
		return "error";
	}
		
	return p;
}

SDL_Surface* renderNumbers(string msj, SDL_Surface* font, SDL_Surface* pantalla){
	int largo = msj.length();
	SDL_Surface* elTexto = SDL_CreateRGBSurface(0, largo*31, 34,32,0,0,0,0); 
	if(elTexto == NULL)
		return NULL;
	SDL_Rect aux;
	aux.x = 0;
	aux.y = 0;
	aux.h = 34;
	aux.w = largo*31;
	SDL_FillRect(elTexto, &aux, SDL_MapRGB(elTexto->format, 255, 0, 255));
	
	// Convierto caracter por caracter:
	int i;
	SDL_Rect rectOrig, rectDest;
	rectOrig.h = 34;
	rectOrig.w = 31;
	rectDest.x = 0;
	rectDest.y = 0;
	for(i = 0; i < largo; i++){
		bool printIt = false;
		if((msj[i] >= '0') && (msj[i] <= '9')) {
			rectOrig.y = 0;
			rectOrig.x = (msj[i] - '0') * 31 + 2;
			printIt = true;
		}
		if(printIt)
			SDL_BlitSurface(font, &rectOrig, elTexto, &rectDest);
		rectDest.x += rectOrig.w;
	}
	SDL_Surface* texto = SDL_ConvertSurface(elTexto, pantalla->format, NULL);
	SDL_FreeSurface(elTexto);
	SDL_SetColorKey(texto, true, SDL_MapRGB(texto->format, 255, 0, 255));
	return texto;
}

void PantallaInicio::pantallaInicio(struct datosPantInic *datos){
	// Cargar SDL
	Uint32 flags = SDL_WINDOW_SHOWN;
	SDL_Window* ventana = NULL;
	SDL_Surface* pantalla = NULL;

	if(SDL_Init( SDL_INIT_EVERYTHING ) < 0)	{
		ErrorLog::getInstance()->escribirLog("CRÍTICO: SDL no pudo inicializarse." , LOG_ERROR);
		printf( "SDL Error: %s\n", SDL_GetError() );
		return;
	} else {
		string title = "AGE OF TALLER SERVER";
		ventana = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 671, 442, flags);
		if(!ventana) {
			ErrorLog::getInstance()->escribirLog("Ventana no pudo crearse" , LOG_ERROR);
			return;
		} else {
			int flags = IMG_INIT_PNG;
			if(!(IMG_Init(flags) & flags)) {
				ErrorLog::getInstance()->escribirLog("CRÍTICO: SDL no pudo inicializarse." , LOG_ERROR);
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				return;
			} else {
				pantalla = SDL_GetWindowSurface(ventana);
			}
		}
	}


	SDL_Surface* imagenActual = loadSurface("server_window.png", pantalla);
	if(!imagenActual) {
		ErrorLog::getInstance()->escribirLog("Error al cargar imágen.¡Imagen inexistente!", LOG_ERROR);
		return;
	}
	SDL_Surface* iPort = loadSurface("insert_port.png", pantalla);
	if(!iPort) {
		ErrorLog::getInstance()->escribirLog("Error al cargar imágen.¡Imagen inexistente!", LOG_ERROR);
		return;
	}
	SDL_SetColorKey(iPort, true, SDL_MapRGB(iPort->format, 255, 0, 255) );
	
	SDL_Surface* iNumbPlayers = loadSurface("insert_number_of_players.png", pantalla);
	if(!iNumbPlayers) {
		ErrorLog::getInstance()->escribirLog("Error al cargar imágen.¡Imagen inexistente!", LOG_ERROR);
		return;
	}
	SDL_SetColorKey(iNumbPlayers, true, SDL_MapRGB(iNumbPlayers->format, 255, 0, 255) );

	SDL_Surface* iObjectives = loadSurface("choose_game_objective.png", pantalla);
	if(!iObjectives) {
		ErrorLog::getInstance()->escribirLog("Error al cargar imágen.¡Imagen inexistente!", LOG_ERROR);
		return;
	}
	SDL_SetColorKey(iObjectives, true, SDL_MapRGB(iObjectives->format, 255, 0, 255) );
	// Font
	SDL_Surface* font = loadSurface("server_numbers.png", pantalla);
	if(!font) {
		ErrorLog::getInstance()->escribirLog("Error al cargar imágen.¡Imagen inexistente!", LOG_ERROR);
		return;
	}
	SDL_SetColorKey(font, true, SDL_MapRGB(font->format, 255, 0, 255) );

	SDL_Surface* imgTexto = NULL;
	// Pantalla y cargar datos:
	string insAux;
	bool sigue = true;
	int cont = 0;
	SDL_Rect pos;
	while(sigue && (cont < 3)){
		SDL_FillRect(pantalla, NULL, 0x000000);
		SDL_BlitSurface(imagenActual, NULL, pantalla, NULL);

		pos.x = 70;
		pos.y = 90;
		SDL_BlitSurface(iPort, NULL, pantalla, &pos);
		if(cont == 0){
			pos.y = 100;
			pos.x = 270;
			imgTexto = renderNumbers(insAux, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
		}
		else if(cont == 1){
			pos.y = 100;
			pos.x = 270;
			imgTexto = renderNumbers(datos->puerto, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
			pos.y = 144;
			pos.x = 490;
			imgTexto = renderNumbers(insAux, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
		}
		else{
			pos.y = 100;
			pos.x = 270;
			imgTexto = renderNumbers(datos->puerto, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
			pos.y = 144;
			pos.x = 490;
			stringstream convert;
			convert << datos->cantPlayers;
			imgTexto = renderNumbers((convert.str()), font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
			pos.y = 194;
			pos.x = 445;
			imgTexto = renderNumbers(insAux, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
		}

		if(cont > 0){
			pos.x = 70;
			pos.y = 140;
			SDL_BlitSurface(iNumbPlayers, NULL, pantalla, &pos);
		}

		if(cont > 1){
			pos.x = 70;
			pos.y = 190;
			SDL_BlitSurface(iObjectives, NULL, pantalla, &pos);
		}

		SDL_UpdateWindowSurface(ventana);
		SDL_Event evento;
		while(SDL_PollEvent(&evento) != 0){
			string q = procesarEventoTexto(evento);
			if(q!= "error"){
				if(q == "finish"){
					if(cont == 0)
						datos->puerto = insAux;
					else if(cont == 1){
						datos->cantPlayers = std::stoi(insAux);
						}
					else{
						datos->tipoPartida = std::stoi(insAux);
						}
					insAux.clear();
					cont++;
					}
				else{
					if(q == "erase"){
						if(insAux.size())
							insAux.resize(insAux.size() -1);
						}
					else if(cont == 0)
						insAux += q;
					else if(cont == 1){
						insAux += q;
						}
					else{
						insAux += q;
						}
					}
			}
			if((evento.type) == SDL_QUIT)
				sigue = false;
			}
		SDL_Delay(FRAME_DURATION);
		}

	SDL_FreeSurface(iPort);
	SDL_FreeSurface(iObjectives);
	SDL_FreeSurface(iNumbPlayers);
	SDL_FreeSurface(pantalla);
	SDL_FreeSurface(imagenActual);
	SDL_Quit();
//	SDL_UpdateWindowSurface(ventana);
//	getch();

}