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
	int offsetX, offsetY;
	int rows, columnas;

	int x_pant, y_pant;

	SDL_Surface* sprite;

public:
	// Constructor y destructor por defecto
	Spritesheet(void);
	~Spritesheet(void);

	// Crea un Spritesheet a partir del nombre
	// de la imagen correspondiente. Se deben
	// especificar adem�s la cantidad de rows
	// y columnas de la imagen respecto de sus
	// subim�gens. Adem�s, se agrega un x_offset
	// y un y_offset que representa el "origen" de
	// las subimagenes dentro de estas.
	// NOTA: Para no llenar la memoria, la imagen
	// se obtiene de la BibliotecaDeImagenes.
	Spritesheet(string imagen, int rows, int cols, int x_offset, int y_offset);

	// Devuelve la coordenada x respeto de la
	// vista de la pantalla donde debe alinearse
	// el origen de la subimagen asociada.
	// Pre: la clase tiene una imagen asociada.
	int verCoordXPantalla(void);

	// Devuelve la coordenada y respeto de la
	// vista de la pantalla donde debe alinearse
	// el origen de la subimagen asociada.
	// Pre: la clase tiene una imagen asociada.
	int verCoordYPantalla(void);

	// Cambia las coordenadas (x,y) respecto de
	// la vista de la pantalla por los valores
	// nuevos recibidos.
	void cambirCoordPantalla(int coordXPant, int coordYPant);

	// Cambia la imagen asignada al Spritesheet
	// por una imagen nueva, de forma similar
	// que en el constructor.
	void cambiarImagen(string nuevaImagen, int rows, int cols, int x_offset, int y_offset);
	
	// Devuelve el valor de offset en x de
	// cada una de las subim�genes.
	int verOffsetX(void);

	// Devuelve el valor de offset en y de
	// cada una de las subim�genes.
	int verOffsetY(void);

	// Devuelve la imagen asociada al Spritesheet
	// que se cargo desde el constructor o con el
	// m�todo de cambiarImagen.
	SDL_Surface* devolverImagenAsociada(void);
	int getRows(void);
	int getCols(void);
};

