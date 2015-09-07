#include "ConversorUnidades.h"
#include <string>
#include "Enumerados.h"
#include <stdio.h>



bool ConversorUnidades::hay_instancia = false;

ConversorUnidades* ConversorUnidades::singleton = NULL;

ConversorUnidades::ConversorUnidades(void)
{
}


ConversorUnidades::~ConversorUnidades(void)
{
}


float ConversorUnidades::convertPixelsToUL(float pixels)
{
	return pixels/LARGO_TILE;
}


float ConversorUnidades::convertULToPixels(float logic_units)
{
	return logic_units*LARGO_TILE;
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
float ConversorUnidades::obtenerCoordPantallaX(float x_UL, float y_UL)
{
	return ((y_UL - x_UL)*LARGO_TILE +369.504)/1.1547 -26;;
}

// Tambien super hardcodeado
float ConversorUnidades::obtenerCoordPantallaY(float x_UL, float y_UL)
{
	return ((y_UL + x_UL)*0.5*LARGO_TILE);
}


ConversorUnidades* ConversorUnidades::obtenerInstancia(void)
{
	if(!hay_instancia){
		singleton = new ConversorUnidades();
		hay_instancia = true;
		}

	return singleton;
}
