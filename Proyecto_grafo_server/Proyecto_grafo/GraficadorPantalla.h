#include "Escenario.h"
#include <SDL.h>
#include <SDL_image.h>
#include "ConversorUnidades.h"
#include "Jugador.h"

#pragma once
class GraficadorPantalla {
private:
	SDL_Window* ventana;
	SDL_Surface* pantalla;
	Escenario* escenario;
	float view_x, view_y;
	float ancho_borde, alto_borde;
	float screen_width, screen_height;
	int vel_scroll, margen_scroll;
	Jugador* player;
public:
	GraficadorPantalla(void);
	~GraficadorPantalla(void);
	GraficadorPantalla(int pant_width, int pant_height, bool full_screen = false, string title = "AGE OF TALLER DE PROGRAMACION I");
	SDL_Surface* getPantalla(void);
	void dibujarPantalla(void);
	void asignarEscenario(Escenario* scene);
	void asignarJugador(Jugador* jugador);
	void asignarParametrosScroll(int margen, int velocidad);
private:
	void reajustarCamara(void);
	void renderizarTerreno(void);
	void renderizarProtagonista(void);
	void renderizarEntidades(void);
	void dibujarMarcoPantalla(int* minimapX, int* minimapY, int* minimapW, int* minimapH);
	void dibujarMinimapa(int minimapX, int minimapY, int minimapW, int minimapH);
public:
	bool cargarSDL(bool full_screen = false, string title = "AGE OF TALLER DE PROGRAMACION I");
	SDL_Window* getVentana(void);
	float getViewX(void);
	float getViewY(void);
	float getAnchoBorde(void);
private:
	SDL_Surface* renderText(string msj);
};
