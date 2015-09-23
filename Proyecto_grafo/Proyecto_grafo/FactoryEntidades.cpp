#include "FactoryEntidades.h"
#include "ErrorLog.h"
#include "Enumerados.h"

bool FactoryEntidades::hay_instancia = false;
FactoryEntidades* FactoryEntidades::singleton = nullptr;

// Constructores y destructores para singleton!!!

FactoryEntidades::FactoryEntidades(void) {
	this->prototipos = map<string, tipoEntidad_t*>();
	this->prototipos[nombre_entidad_def] = new tipoEntidad_t();
}

FactoryEntidades::~FactoryEntidades(void) {
	if (hay_instancia) {
		hay_instancia = false;
		map<string, tipoEntidad_t*>::const_iterator iter = singleton->prototipos.begin();
		while (iter != singleton->prototipos.end()) {
			delete iter->second;
			iter++;
		}
		singleton->prototipos.clear();
	}
}

FactoryEntidades* FactoryEntidades::obtenerInstancia(void) {
	if (!hay_instancia || !singleton){
		singleton = new FactoryEntidades();		
		hay_instancia = true;
	}
	return singleton;
}

void FactoryEntidades::limpar(void) {
	if (hay_instancia) {
		map<string, tipoEntidad_t*>::const_iterator iter = singleton->prototipos.begin();
		while (iter != singleton->prototipos.end()) {
			delete iter->second;
			iter++;
		}
		singleton->prototipos.clear();

		this->prototipos = map<string, tipoEntidad_t*>();
		this->prototipos[nombre_entidad_def] = new tipoEntidad_t();
	}
}



// Funciones de factory

void FactoryEntidades::agregarEntidad(string name, int tamX, int tamY) {
	if (name.find(estados_extensiones[EST_CAMINANDO]) != string::npos) {
		return;
	}
	if (name == nombre_entidad_def) {
		ErrorLog::getInstance()->escribirLog("Tipo de instancia [" + nombre_entidad_def + "] reservada. No se reemplazará el tipo por defecto.", LOG_WARNING);
		return;
	}

	if (prototipos.count(name) > 0) {
		ErrorLog::getInstance()->escribirLog("Definición de entidad [" + name + "] duplicada. No se sobreescribirán los datos", LOG_WARNING);
	} else {
		tipoEntidad_t* pType = new tipoEntidad_t();
		if (tamX <= 0) {
			ErrorLog::getInstance()->escribirLog("Tamaño inválido en la entidad [" + name + "]. Se utiliza valor por defecto.", LOG_WARNING);
			tamX = 1;
		}
		if (tamY <= 0) {
			ErrorLog::getInstance()->escribirLog("Tamaño inválido en la entidad [" + name + "]. Se utiliza valor por defecto.", LOG_WARNING);
			tamY = 1;
		}
		pType->tamX = tamX;
		pType->tamY = tamY;
		prototipos[name] = pType;
	}
}

Entidad* FactoryEntidades::obtenerEntidad(string name){
	if (name.find(estados_extensiones[EST_CAMINANDO]) != string::npos) {
		ErrorLog::getInstance()->escribirLog("Extensión [" + estados_extensiones[EST_CAMINANDO] + "] reservada. La instancia se reemplazará por entidad por defecto.", LOG_WARNING);
		name = nombre_entidad_def;
	}

	Entidad* ent = nullptr;
	tipoEntidad_t* pType = nullptr;
	if (prototipos.count(name) > 0) {
		pType = prototipos[name];
		ent = new Entidad(name, pType->tamX, pType->tamX);
	} else {
		ErrorLog::getInstance()->escribirLog("Entidad [" + name + "] no existe en sistema. Se reemplazará por entidad por defecto.", LOG_WARNING);
		pType = prototipos[nombre_entidad_def];
		ent = new Entidad(nombre_entidad_def, pType->tamX, pType->tamX);
	} 
	return ent;
}

Unidad* FactoryEntidades::obtenerUnidad(string name){	
	if (name.find(estados_extensiones[EST_CAMINANDO]) != string::npos) {
		ErrorLog::getInstance()->escribirLog("Extensión [" + estados_extensiones[EST_CAMINANDO] + "] reservada. La instancia se reemplazará por entidad por defecto.", LOG_WARNING);
		name = nombre_entidad_def;
	}
	Unidad* unit = nullptr;
	tipoEntidad_t* pType = nullptr;
	if (prototipos.count(name) > 0) {
		pType = prototipos[name];
		unit = new Unidad(name, pType->tamX, pType->tamX);
	} else {
		ErrorLog::getInstance()->escribirLog("Entidad [" + name + "] no existe en sistema. Se reemplazará por entidad por defecto.", LOG_WARNING);
		pType = prototipos[nombre_entidad_def];
		unit = new Unidad(nombre_entidad_def, pType->tamX, pType->tamX);
	} 
	return unit;
}






/*

Entidad* Factory::obtenerEntidad(instanciaInfo_t* instanciaInfo){	
	if(instanciaInfo->tipo == "house")
		return new Casa();
	else if(instanciaInfo->tipo == "town_center")
		return new CentroUrbano();
	else if(instanciaInfo->tipo == "castle")
		return new Castillo();
	else if(instanciaInfo->tipo == "windmill")
		return new Molino();
	else if(instanciaInfo->tipo == "stable")
		return new Establo();
	else if(instanciaInfo->tipo == "barracks")
		return new Cuarteles();
	else if(instanciaInfo->tipo == "archery")
		return new Arqueria();

	
	ErrorLog::getInstance()->escribirLog("Se trató de crear una entidad de tipo: " + instanciaInfo->tipo + " inexistente en sistema", LOG_ERROR);
	return nullptr;
}

Unidad* Factory::obtenerUnidad(instanciaInfo_t* instanciaInfo){	
	if(instanciaInfo->tipo == "settler")
		return new Aldeano();
	if(instanciaInfo->tipo == "champion")
		return new Campeon();
	if (instanciaInfo->tipo == "knight")
		return new Jinete();
	
	ErrorLog::getInstance()->escribirLog("Se trató de crear una entidad de tipo: " + instanciaInfo->tipo + " inexistente en sistema", LOG_ERROR);
	return nullptr;
};

*/