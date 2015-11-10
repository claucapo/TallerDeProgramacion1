#ifndef RECURSO_H
#define RECURSO_H

#include "Entidad.h"

class Recurso : public Entidad {
private:
	int recursoMax;
	
public:
	int recursoAct;
	resource_type_t tipoR;
	
	Recurso(unsigned int id, string name, tipoEntidad_t pType);
	~Recurso(void) {};

	virtual af_result_t avanzarFrame(Escenario* scene);
};


#endif // RECURSO_H