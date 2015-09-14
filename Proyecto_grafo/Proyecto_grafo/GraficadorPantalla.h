#include "Escenario.h"
#include <SDL.h>
#include <SDL_image.h>
#include "ConversorUnidades.h"

#pragma once
class GraficadorPantalla
{
private:
	SDL_Window* ventana;
	SDL_Surface* pantalla;
	Escenario* escenario;
	float view_x, view_y;
	float ancho_borde, alto_borde;
	float screen_width, screen_height;

public:
	GraficadorPantalla(void);
	~GraficadorPantalla(void);
	GraficadorPantalla(Escenario* escenario, int pant_width, int pant_height);
	SDL_Surface* getPantalla(void);
	void dibujarPantalla(void);
private:
	void reajustarCamara(void);
public:
	bool cargarSDL(void);
};

