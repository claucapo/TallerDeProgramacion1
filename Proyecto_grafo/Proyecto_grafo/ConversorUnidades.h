
#pragma once


class ConversorUnidades
{
private:
	ConversorUnidades(void);

	// Este atributo es el singleton
	static ConversorUnidades* singleton;

	// Flag para saber si hay instancia o no
	static bool hay_instancia;

	float escala_mostrar;
public:

	~ConversorUnidades(void);
	float convertPixelsToUL(float pixels);
	float convertULToPixels(float logic_units);
	float convertFramesToMiliseconds(float frames);
	float convertMilisecondsToFrames(float miliseconds);
	float obtenerCoordPantallaX(float x_UL, float y_UL, float view_x, float view_y, float ancho_borde);
	float obtenerCoordPantallaY(float x_UL, float y_UL, float view_x, float view_y, float ancho_borde);
	static ConversorUnidades* obtenerInstancia(void);
	float obtenerCoordLogicaX(float x_pant, float y_pant, float view_x, float view_y, float ancho_borde);
	float obtenerCoordLogicaY(float x_pant, float y_pant, float view_x, float view_y, float ancho_borde);
	void asignarEscalaMostrar(float escala_mostrar);
};

