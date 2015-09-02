#pragma once
class Posicion
{
private:
	// Atributos
	int coord_x, coord_y;
public:
	// Métodos
	Posicion(int x, int y);
	~Posicion(void);
	int devolverCoordX();
	int devolverCoordY();
	int generarCodigo();
	bool operator <(const Posicion other);
	bool operator >(const Posicion other);
	bool operator ==(const Posicion other);
};

