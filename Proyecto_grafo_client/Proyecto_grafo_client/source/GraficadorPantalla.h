#include "Escenario.h"
#include <SDL.h>
#include <SDL_image.h>
#include "ConversorUnidades.h"
#include "Jugador.h"
#include "Partida.h"
#include <string.h>

#pragma once
class GraficadorPantalla {
private:
	SDL_Window* ventana;
	SDL_Surface* pantalla;
	Partida* partida;
	Jugador* player;
	float view_x, view_y;
	float ancho_borde, alto_borde;
	void mostrarStatsEntidad(Entidad* ent);

	int vel_scroll, margen_scroll;
public:
	float screen_width, screen_height;
	void mostrarPantallaInicio(bool show_name, string name);
	bool puntoEstaEnPantalla(int xPant, int yPant);
	GraficadorPantalla(void);
	~GraficadorPantalla(void);
	GraficadorPantalla(int pant_width, int pant_height, bool full_screen = false, string title = "AGE OF TALLER DE PROGRAMACION I");
	SDL_Surface* getPantalla(void);
	void dibujarPantalla(void);
	void asignarPartida(Partida* partida);
	void asignarJugador(Jugador* player) {this->player = player;}
	void asignarParametrosScroll(int margen, int velocidad);
private:
	void reajustarCamara(void);
	void renderizarTerreno(void);
	void renderizarProtagonista(void);
	void renderizarEntidades(void);
	void dibujarEdificioUbicandose();
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
public:
	void pantallaFinal(bool pantDerrota);
};