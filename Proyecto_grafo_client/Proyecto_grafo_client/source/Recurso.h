#ifndef RECURSO_H
#define RECURSO_H

#include "Entidad.h"

class Recurso : public Entidad {
private:
	int recursoMax;
	int recursoAct;
	
public:
	resource_type_t tipoR;
	
	Recurso(unsigned int id, string name, tipoEntidad_t pType);
	~Recurso(void) {};

	virtual af_result_t avanzarFrame(Escenario* scene);
};


#endif // RECURSO_H