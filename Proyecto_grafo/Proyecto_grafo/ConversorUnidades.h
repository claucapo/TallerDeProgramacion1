
#pragma once


class ConversorUnidades
{
private:
	ConversorUnidades(void);

	// Este atributo es el singleton
	static ConversorUnidades* singleton;

	// Flag para saber si hay instancia o no
	static bool hay_instancia;
public:

	~ConversorUnidades(void);
	float convertPixelsToUL(float pixels);
	float convertULToPixels(float logic_units);
	float convertFramesToMiliseconds(float frames);
	float convertMilisecondsToFrames(float miliseconds);
	float obtenerCoordPantallaX(float x_UL, float y_UL);
	float obtenerCoordPantallaY(float x_UL, float y_UL);
	static ConversorUnidades* obtenerInstancia(void);
};

