#include "PantallaInicio.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include "GraficadorPantalla.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include "BibliotecaDeImagenes.h"
#include "Enumerados.h"

PantallaInicio::PantallaInicio(void){
}


PantallaInicio::~PantallaInicio(void){
}



string PantallaInicio::procesarEventoLetras(SDL_Event evento){
	if(evento.type != SDL_KEYDOWN)
		return "error";

	string p;
	switch(evento.key.keysym.scancode){
	case SDL_SCANCODE_A:
		p = "A"; break;
	case SDL_SCANCODE_B:
		p = "B"; break;
	case SDL_SCANCODE_C:
		p = "C"; break;
	case SDL_SCANCODE_D:
		p = "D"; break;
	case SDL_SCANCODE_E:
		p = "E"; break;
	case SDL_SCANCODE_F:
		p = "F"; break;
	case SDL_SCANCODE_G:
		p = "G"; break;
	case SDL_SCANCODE_H:
		p = "H"; break;
	case SDL_SCANCODE_I:
		p = "I"; break;
	case SDL_SCANCODE_J:
		p = "J"; break;
	case SDL_SCANCODE_K:
		p = "K"; break;
	case SDL_SCANCODE_L:
		p = "L"; break;
	case SDL_SCANCODE_M:
		p = "M"; break;
	case SDL_SCANCODE_N:
		p = "N"; break;
	case SDL_SCANCODE_O:
		p = "O"; break;
	case SDL_SCANCODE_P:
		p = "P"; break;
	case SDL_SCANCODE_Q:
		p = "Q"; break;
	case SDL_SCANCODE_R:
		p = "R"; break;
	case SDL_SCANCODE_S:
		p = "S"; break;
	case SDL_SCANCODE_T:
		p = "T"; break;
	case SDL_SCANCODE_U:
		p = "U"; break;
	case SDL_SCANCODE_V:
		p = "V"; break;
	case SDL_SCANCODE_W:
		p = "W"; break;
	case SDL_SCANCODE_X:
		p = "X"; break;
	case SDL_SCANCODE_Y:
		p = "Y"; break;
	case SDL_SCANCODE_Z:
		p = "Z"; break;
	case SDL_SCANCODE_SPACE:
		p = " "; break;
	case SDL_SCANCODE_BACKSPACE:
		p = "erase"; break;
	case SDL_SCANCODE_RETURN:
		p = "finish"; break;
	default:
		return "error";
	}
		
	return p;
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
	case SDL_SCANCODE_PERIOD:
		p = '9' + 1; break;
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
		if((msj[i] >= '0') && (msj[i] <= ('9' + 1))) {
			rectOrig.y = 0;
			rectOrig.x = (msj[i] - '0') * 31 + 2;
			printIt = true;
			if(msj[i] == ('9' + 1))
				rectDest.x -= 10;
		}
		if(printIt)
			SDL_BlitSurface(font, &rectOrig, elTexto, &rectDest);
		rectDest.x += rectOrig.w;
		if(msj[i] == ('9' + 1))
			rectDest.x -= 10;
	}
	SDL_Surface* texto = SDL_ConvertSurface(elTexto, pantalla->format, NULL);
	SDL_FreeSurface(elTexto);
	SDL_SetColorKey(texto, true, SDL_MapRGB(texto->format, 255, 0, 255));
	return texto;
}

void PantallaInicio::pantallaInicio(struct datosPantInic *datos, SDL_Surface* pantalla, SDL_Window* ventana){
	BibliotecaDeImagenes* bu = BibliotecaDeImagenes::obtenerInstancia();
	SDL_Surface* imagenActual = bu->devolverImagen("client_window");
	
	SDL_Surface* iPort = bu->devolverImagen("insert_port");
	SDL_SetColorKey(iPort, true, SDL_MapRGB(iPort->format, 255, 0, 255) );
	
	SDL_Surface* iIp = bu->devolverImagen("insert_ip");
	SDL_SetColorKey(iIp, true, SDL_MapRGB(iIp->format, 255, 0, 255) );

	SDL_Surface* plzW8 = bu->devolverImagen("please_wait");
	SDL_SetColorKey(plzW8, true, SDL_MapRGB(plzW8->format, 255, 0, 255) );
	
	// Font
	SDL_Surface* font = bu->devolverImagen("client_numbers");
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

		pos.x = 30;
		pos.y = 90;
		SDL_BlitSurface(iIp, NULL, pantalla, &pos);
		if(cont == 0){
			pos.y = 97;
			pos.x = 295;
			imgTexto = renderNumbers(insAux, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
		}
		else if(cont == 1){
			pos.y = 97;
			pos.x = 295;
			imgTexto = renderNumbers(datos->ip, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
			pos.y = 147;
			pos.x = 230;
			imgTexto = renderNumbers(insAux, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
		}
		else{
			pos.y = 97;
			pos.x = 295;
			imgTexto = renderNumbers(datos->ip, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
			pos.y = 147;
			pos.x = 230;
			imgTexto = renderNumbers(datos->puerto, font, pantalla);
			SDL_BlitSurface(imgTexto, NULL, pantalla, &pos);
			SDL_FreeSurface(imgTexto);
		}

		if(cont > 0){
			pos.x = 30;
			pos.y = 140;
			SDL_BlitSurface(iPort, NULL, pantalla, &pos);

		}

		if(cont > 1){
			pos.x = 207;
			pos.y = 260;
			SDL_BlitSurface(plzW8, NULL, pantalla, &pos);
			cont++;
		}

		SDL_UpdateWindowSurface(ventana);
		SDL_Event evento;
		while(SDL_PollEvent(&evento) != 0){
			string q = procesarEventoTexto(evento);
			if(q!= "error"){
				if(q == "finish"){
					if(cont == 0)
						datos->ip = insAux;
					else if(cont == 1){
						datos->puerto = insAux;
						}
					else{
						
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

	stringstream s;
	char temp[50];
	int last = datos->ip.copy(temp, 49, 0);
	temp[last] = 0;
	for (int i = 0; i < 50; i++){
		if (temp[i] == ':')
			temp[i] = '.';
	}
	datos->ip = string(temp);

//	SDL_UpdateWindowSurface(ventana);
//	getch();

}
