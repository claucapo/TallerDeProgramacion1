#include <SDL.h>
#include <SDL_mixer.h>
#include "GraficadorPantalla.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include "BibliotecaDeImagenes.h"
#include "Enumerados.h"
#pragma once

class PantallaInicio
{
public:
	PantallaInicio(void);
	~PantallaInicio(void);
	void pantallaInicio(struct datosPantInic *datos, SDL_Surface* pantalla, SDL_Window* ventana);
	string procesarEventoLetras(SDL_Event evento);
};

