#include<string>
#include <SDL.h>
#include <SDL_image.h>
#pragma once

using namespace std;

/* La clase Spritesheet representa la imagen
con todas las posibles subimagenes de una Entidad
y un determinado estado. A su vez, esta clase 
sabe en qu� coordenadas respecto de la vista de
la pantalla dibujar dichas subim�genes.*/

/* NOTA IMPORTANTE: Debemos ponernos de acuerdo
y establecer una clase de convensi�n respecto de
la distribuci�n de las subim�genes en la spritesheet.
Mi propuesta: que cada fila de la spritesheet contenga
en orden todas las subimagenes del movimiento de la unidad
en una misma direcci�n. Luego, la primer fila tendr�a
las subimagenes de la unidad movi�ndose a 0� (hacia la dcha);
la segunda fila, cuando se mueve a 45�, la tercera
fila a 90� (arriba), y as� hasta llegar a la octava
fila que tendr�a las subimagenes de cuando la unidad
se mueve hacia abajo y la derecha.*/
class Spritesheet
{
private:
	int subX, subY;
	int filas, columnas;
	SDL_Surface* sprite;

	// Coordenadas de la im�gen en la pantalla
	int x_pant, y_pant;

public:
	// Constructor y destructor por defecto
	Spritesheet(void);
	~Spritesheet(void);

	// Crea un Spritesheet a partir del nombre
	// de la imagen correspondiente. Se deben
	// especificar adem�s la cantidad de rows
	// y columnas de la imagen respecto de sus
	// subim�genes. Adem�s, se agrega un par de
	// coordenadas subX subY que indican la columna
	// y fila espec�ficas de la imagen que se est� mostrando.
	// NOTA: Para no llenar la memoria, la imagen
	// se obtiene de la BibliotecaDeImagenes.
	Spritesheet(string imagen, int rows, int cols, int subX, int subY);

	// GETTERS
	// Devuelve las coordenadas en pantalla en la que se deben mostrar las im�genes
	int getCoordX(void) {return this->x_pant;}
	int getCoordY(void) {return this->y_pant;}
	int getFilas(void) {return this->filas;}
	int getColumnas(void) {return this->columnas;}
	SDL_Surface* devolverImagenAsociada(void) {return this->sprite;}

	// M�todos para calcular las dimensiones de las subimagenes
	int subImagenWidth();
	int subImagenHeight();


	// Cambia las coordenadas de la pantalla
	// en las que ser� dibujada la im�gen
	void cambirCoord(int x, int y) {
		this->x_pant = x;
		this->y_pant = y;
	}

	// Cambia la imagen asignada al Spritesheet
	// por una imagen nueva, de forma similar
	// que en el constructor.
	void cambiarImagen(string nuevaImagen, int rows, int cols, int subX, int subY);

	// Cambia la subimagen dentro de una misma spritesheet.
	void Spritesheet::cambiarSubImagen(int subX, int subY);

	// Pasa a la siguiente subimagen dentro de latira, si se llega al final vuelve al principio
	void Spritesheet::siguienteFrame();
	
	// Calcula el offset en pixeles en que se encuentra el primer
	// pixel de la subimagen
	int calcularOffsetX(void);
	int calcularOffsetY(void);
};

