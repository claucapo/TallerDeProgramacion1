#ifndef PROTAGONISTA_H
#define PROTAGONISTA_H

#include "Unidad.h"

class Aldeano : public Unidad {
public:
	Aldeano();
	~Aldeano() {};
};

class Campeon : public Unidad {
public:
	Campeon();
	~Campeon() {};
};

class Jinete : public Unidad {
public:
	Jinete();
	~Jinete() {};
};

#endif