#include<string>
#pragma once

using namespace std;

/* La clase Spritesheet representa la imagen
con todas las posibles subimagenes de una Entidad
y un determinado estado. A su vez, esta clase 
sabe en qué coordenadas respecto de la vista de
la pantalla dibujar dichas subimágenes.*/

/* NOTA IMPORTANTE: Debemos ponernos de acuerdo
y establecer una clase de convensión respecto de
la distribución de las subimágenes en la spritesheet.
Mi propuesta: que cada fila de la spritesheet contenga
en orden todas las subimagenes del movimiento de la unidad
en una misma dirección. Luego, la primer fila tendría
las subimagenes de la unidad moviéndose a 0º (hacia la dcha);
la segunda fila, cuando se mueve a 45º, la tercera
fila a 90º (arriba), y así hasta llegar a la octava
fila que tendría las subimagenes de cuando la unidad
se mueve hacia abajo y la derecha.*/
class Spritesheet
{
public:
	// Constructor y destructor por defecto
	Spritesheet(void);
	~Spritesheet(void);

	// Crea un Spritesheet a partir del nombre
	// de la imagen correspondiente. Se deben
	// especificar además la cantidad de rows
	// y columnas de la imagen respecto de sus
	// subimágens.
	// NOTA: Para no llenar la memoria, la imagen
	// se obtiene de la BibliotecaDeImagenes.
	Spritesheet(string imagen, int rows, int cols);

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
	void cambiarImagen(string nuevaImagen, int rows, int cols);
};

