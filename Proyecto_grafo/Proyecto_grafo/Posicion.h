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
	int coord_x, coord_y;
public:
	// Constructor y destructor por defecto
	Posicion();
	~Posicion(void);

	// Crea una Posicion con las coordenadas
	// xy recibidas.
	Posicion(int x, int y);

	// Devuelve la coordenada x de la Posicion
	int devolverCoordX();

	// Devuelve la coordenada y de la Posicion
	int devolverCoordY();

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

