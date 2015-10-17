#ifndef EDIFICIOS_H
#define EDIFICIOS_H

#include "Entidad.h"


class Casa : public Entidad {
public:
	Casa();
	~Casa() {};
};

class CentroUrbano : public Entidad {
public:
	CentroUrbano();
	~CentroUrbano() {};
};

class Castillo : public Entidad {
public:
	Castillo();
	~Castillo() {};
};

class Molino : public Entidad {
public:
	Molino();
	~Molino() {};
};

class Cuarteles : public Entidad {
public:
	Cuarteles();
	~Cuarteles() {};
};

class Arqueria : public Entidad {
public:
	Arqueria();
	~Arqueria() {};
};

class Establo : public Entidad {
public:
	Establo();
	~Establo() {};
};


#endif EDIFICIOS_H