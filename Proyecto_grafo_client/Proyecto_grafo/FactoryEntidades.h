#ifndef FACTORY_ENTIDADES_H
#define FACTORY_ENTIDADES_H
#include "Entidad.h"
#include "ConfigParser.h"
#include "Unidad.h"

#include <map>

#pragma once

enum entity_type_t {ENT_T_NONE, ENT_T_UNIT, ENT_T_RESOURCE};

// Definición del struct que contendrá información particular de cada clase.
struct tipoEntidad_t {
	int tamX;
	int tamY;
	int vision;
	entity_type_t tipo;
	int score;

	// Valores por defecto
	tipoEntidad_t() {
		tamX = 1;
		tamY = 1;
		vision = 1;
		tipo = ENT_T_NONE;
		score = 1;
	}
};


// Implementada como singleton
class FactoryEntidades {
private:
	static FactoryEntidades* singleton;
	static bool hay_instancia;
	map<string, tipoEntidad_t*> prototipos;

	FactoryEntidades();


public:
	// Destructor por defecto
	~FactoryEntidades(void);
	static FactoryEntidades* obtenerInstancia(void);
	void limpar();

	void agregarEntidad(entidadInfo_t eInfo);

	Entidad* obtenerEntidad(string name);
	Unidad* obtenerUnidad(string name);
};

#endif FACTORY_ENTIDADES_H