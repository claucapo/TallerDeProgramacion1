#ifndef FACTORY_H
#define FACTORY_H
#include "Entidad.h"
#include "Edificios.h"
#include "ConfigParser.h"
#include "Unidad.h"
#include "Protagonistas.h"
#pragma once
class Factory {

public:
	// Destructor por defecto
	~Factory(void);
	static Entidad* obtenerEntidad(instanciaInfo_t* instanciaInfo);
	static Unidad* obtenerUnidad(instanciaInfo_t* iInfo);

};

#endif FACTORY_H