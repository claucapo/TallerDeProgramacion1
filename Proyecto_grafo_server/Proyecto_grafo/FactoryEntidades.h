#ifndef FACTORY_ENTIDADES_H
#define FACTORY_ENTIDADES_H
#include "Entidad.h"
#include "ConfigParser.h"
#include "Enumerados.h"
#include "Unidad.h"
#include "Protocolo.h"

#include <map>

#pragma once

// Definición del struct que contendrá información particular de cada clase.
struct tipoEntidad_t {
	int tamX;
	int tamY;
	int vision;
	int velocidad;
	entity_type_t tipo;
	int score;
	resource_type_t tipoR;

	// Valores por defecto
	tipoEntidad_t() {
		tamX = 1;
		tamY = 1;
		vision = 1;
		velocidad = 1;
		tipo = ENT_T_NONE;
		score = 1;
		tipoR = RES_T_NONE;
	}
};


// Implementada como singleton
class FactoryEntidades {
private:
	static FactoryEntidades* singleton;
	static bool hay_instancia;
	map<string, tipoEntidad_t*> prototipos;

	FactoryEntidades();

	static unsigned int obtenerIDValida();
	static unsigned int nextID;

public:
	// Destructor por defecto
	~FactoryEntidades(void);
	static FactoryEntidades* obtenerInstancia(void);
	void limpar();

	void agregarEntidad(entidadInfo_t eInfo);

	list<msg_tipo_entidad*> obtenerListaTipos(void);

	Entidad* obtenerEntidad(string name);
	// Unidad* obtenerUnidad(string name);
};

#endif FACTORY_ENTIDADES_H