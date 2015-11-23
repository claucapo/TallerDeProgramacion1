#ifndef FACTORY_ENTIDADES_H
#define FACTORY_ENTIDADES_H
#include "Entidad.h"
#include "ConfigParser.h"
#include "Enumerados.h"
#include "Unidad.h"
#include "Protocolo.h"

#include <map>

#pragma once

// Implementada como singleton
class FactoryEntidades {
private:
	static FactoryEntidades* singleton;
	static bool hay_instancia;
	map<string, tipoEntidad_t*> prototipos;

	FactoryEntidades();

	static unsigned int obtenerIDValida();
	static unsigned int nextID;

	static unsigned int nextTypeID;
	static unsigned int obtenerTypeIDValida();

public:
	// Destructor por defecto
	~FactoryEntidades(void);
	static FactoryEntidades* obtenerInstancia(void);
	void limpar();

	void agregarEntidad(entidadInfo_t eInfo);

	list<msg_tipo_entidad*> obtenerListaTipos(void);

	tipoEntidad_t* obtenerPrototipo(unsigned int id);
	tipoEntidad_t* obtenerPrototipo(string name);
	Entidad* obtenerEntidad(string name);
	Entidad* obtenerEntidad(unsigned int id);
	unsigned int obtenerTypeID(string name);
	string obtenerName(unsigned int typeID);

	// Unidad* obtenerUnidad(string name);
};

#endif FACTORY_ENTIDADES_H