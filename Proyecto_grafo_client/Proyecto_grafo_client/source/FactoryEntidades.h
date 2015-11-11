#ifndef FACTORY_ENTIDADES_H
#define FACTORY_ENTIDADES_H
#include "Entidad.h"
#include "ConfigParser.h"
#include "Enumerados.h"
#include "Unidad.h"
#include "Protocolo.h"

#include <list>
#include <map>
#include <string>

using namespace std;
#pragma once



// Implementada como singleton
class FactoryEntidades {
private:
	static FactoryEntidades* singleton;
	static bool hay_instancia;
	map<string, tipoEntidad_t*> prototipos;
	list<string> edificiosConstruibles;
	FactoryEntidades();
	
public:
	// Destructor por defecto
	~FactoryEntidades(void);
	static FactoryEntidades* obtenerInstancia(void);
	void limpar();

	void agregarEntidad(msg_tipo_entidad eInfo);

	Entidad* obtenerEntidad(string name, unsigned int id);
	// Unidad* obtenerUnidad(string name);
	list<string> verListaEdificios(void);
};

#endif FACTORY_ENTIDADES_H