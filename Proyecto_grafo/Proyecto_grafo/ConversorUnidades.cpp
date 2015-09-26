#include "ConversorUnidades.h"
#include <string>
#include "Enumerados.h"
#include <stdio.h>



bool ConversorUnidades::hay_instancia = false;

ConversorUnidades* ConversorUnidades::singleton = NULL;

ConversorUnidades::ConversorUnidades(void)
{
	escala_mostrar = 1;
}


ConversorUnidades::~ConversorUnidades(void)
{
}


float ConversorUnidades::convertPixelsToUL(float pixels)
{
	return pixels/(LARGO_TILE * escala_mostrar);
}


float ConversorUnidades::convertULToPixels(float logic_units)
{
	return logic_units*LARGO_TILE  * escala_mostrar;
}


float ConversorUnidades::convertFramesToMiliseconds(float frames)
{
	return frames*FRAME_DURATION;
}


float ConversorUnidades::convertMilisecondsToFrames(float miliseconds)
{
	return miliseconds/FRAME_DURATION;
}

// Super hardcodeado por ahora
float ConversorUnidades::obtenerCoordPantallaX(float x_UL, float y_UL, float view_x, float view_y, float ancho_borde)
{
	float v = 0.866 * (y_UL - x_UL) * LARGO_TILE;
	return (v - view_x + ancho_borde * 0.5) * escala_mostrar;
}

// Tambien super hardcodeado
float ConversorUnidades::obtenerCoordPantallaY(float x_UL, float y_UL, float view_x, float view_y, float ancho_borde)
{
	float u = 0.5 * (y_UL + x_UL) * LARGO_TILE;
	return (u - view_y)  * escala_mostrar;
}


ConversorUnidades* ConversorUnidades::obtenerInstancia(void)
{
	if(!hay_instancia){
		singleton = new ConversorUnidades();
		hay_instancia = true;
		}

	return singleton;
}


float ConversorUnidades::obtenerCoordLogicaX(float x_pant, float y_pant, float view_x, float view_y, float ancho_borde)
{
	float u = y_pant / escala_mostrar+ view_y;
	float v = x_pant / escala_mostrar+ view_x - 0.5*ancho_borde;
	return (u - 0.57735*v)/LARGO_TILE;
}


float ConversorUnidades::obtenerCoordLogicaY(float x_pant, float y_pant, float view_x, float view_y, float ancho_borde)
{
	float u = y_pant / escala_mostrar+ view_y;
	float v = x_pant / escala_mostrar+ view_x - 0.5*ancho_borde;
	return (u + 0.57735*v)/(LARGO_TILE);
}


void ConversorUnidades::asignarEscalaMostrar(float escala_mostrar){
	this->escala_mostrar = escala_mostrar;
}
