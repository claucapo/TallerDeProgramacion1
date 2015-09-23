#ifndef FACTORY_ENTIDADES_H
#define FACTORY_ENTIDADES_H
#include "Entidad.h"
#include "ConfigParser.h"
#include "Unidad.h"

#include <map>

#pragma once

// Definición del struct que contendrá información particular de cada clase.
struct tipoEntidad_t {
	int tamX;
	int tamY;

	tipoEntidad_t() {
		tamX = 1;
		tamY = 1;
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

	void agregarEntidad(string name, int tamX, int tamY);

	Entidad* obtenerEntidad(string name);
	Unidad* obtenerUnidad(string name);
};

#endif FACTORY_ENTIDADES_H