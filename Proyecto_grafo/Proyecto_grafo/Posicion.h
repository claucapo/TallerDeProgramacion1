#pragma once


/* La clase Posicion representa la casilla (tile)
con coordenadas (x,y) dentro de la estructura del
mapa.
NOTA: Tal como está programa la clase por ahora,
si no se la inicializa con su constructor su
valor por defecto de (x,y) es (0,0).*/
class Posicion
{
private:
	// Atributos
	float coord_x, coord_y;
public:
	// Constructor y destructor por defecto
	Posicion();
	~Posicion(void);

	// Crea una Posicion con las coordenadas
	// xy recibidas.
	Posicion(float x, float y);
	
	// Devuelve las coordeadas en punto flotante
	float getX() {return this->coord_x;}
	float getY() {return this->coord_y;}

	// Redefinición de operadores:
	// "Dos Posicion son iguales si poseen las
	// mismas coordenadas (x,y)"
	// El mayor y el menor sólo fueron inventados
	// para que existiera un orden en el grafo.
	int generarCodigo();
	bool operator <(const Posicion other);
	bool operator >(const Posicion other);
	bool operator ==(const Posicion other);
};

