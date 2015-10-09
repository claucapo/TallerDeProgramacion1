#ifndef RECURSO_H
#define RECURSO_H

#include "Entidad.h"

class Recurso : public Entidad {
private:
	int amount;
	
public:
	Recurso(void);
	Recurso(int amount);
	Recurso(string name, int tamX = 1, int tamY = 1, int vision = 1, int amount = 1);
	~Recurso(void) {};

	virtual bool avanzarFrame(Escenario* scene);
};


#endif // RECURSO_H