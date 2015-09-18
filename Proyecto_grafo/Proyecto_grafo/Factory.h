#ifndef FACTORY_H
#define FACTORY_H
#include "Entidad.h"
#include "Edificios.h"
#include "ConfigParser.h"
#pragma once
class Factory
{

public:
	// Destructor por defecto
	~Factory(void);
	static Entidad* obtenerEntidad(instanciaInfo_t* instanciaInfo);

};

#endif FACTORY_H