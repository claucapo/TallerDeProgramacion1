#ifndef RECURSO_H
#define RECURSO_H

#include "Entidad.h"

class Recurso : public Entidad {
private:
	int amount;
	
	Recurso(void);
	Recurso(int amount);

public:
	resource_type_t tipoR;

	Recurso(unsigned int id, string name, int tamX = 1, int tamY = 1, int vision = 1, int amount = 1, resource_type_t tipoR = RES_T_NONE);
	~Recurso(void) {};

	virtual af_result_t avanzarFrame(Escenario* scene);
};


#endif // RECURSO_H