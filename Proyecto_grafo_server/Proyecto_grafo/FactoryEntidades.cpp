#include "FactoryEntidades.h"
#include "ErrorLog.h"
#include "Enumerados.h"
#include "Recurso.h"

#include "Protocolo.h"

bool FactoryEntidades::hay_instancia = false;
FactoryEntidades* FactoryEntidades::singleton = nullptr;
unsigned int FactoryEntidades::nextID = 0;

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

unsigned int FactoryEntidades::obtenerIDValida() {
	return nextID++;
}

// Funciones de factory

void FactoryEntidades::agregarEntidad(entidadInfo_t eInfo) {
	// El nombre de la entidad no contiene subcadenas de estados como "_move"
	if (eInfo.nombre.find(estados_extensiones[EST_CAMINANDO]) != string::npos) {
		return;
	}

	// El nombre de la entidad no es el valor por defecto
	if (eInfo.nombre == nombre_entidad_def) {
		ErrorLog::getInstance()->escribirLog("Tipo de instancia [" + nombre_entidad_def + "] reservada. No se reemplazará el tipo por defecto.", LOG_WARNING);
		return;
	}

	// Si el nombre ya existía no lo agrego e imprimo un error
	if (prototipos.count(eInfo.nombre) > 0) {
		ErrorLog::getInstance()->escribirLog("Definición de entidad [" + eInfo.nombre + "] duplicada. No se sobreescribirán los datos", LOG_WARNING);
	} else {
		tipoEntidad_t* pType = new tipoEntidad_t();

		// Estos chequeos se pueden sacar... ya que el yaml siempre tira datos válidos
		if (eInfo.tamX <= 0) {
			ErrorLog::getInstance()->escribirLog("Tamaño inválido en la entidad [" + eInfo.nombre + "]. Se utiliza valor por defecto.", LOG_WARNING);
			eInfo.tamX = 1;
		}
		if (eInfo.tamY <= 0) {
			ErrorLog::getInstance()->escribirLog("Tamaño inválido en la entidad [" + eInfo.nombre + "]. Se utiliza valor por defecto.", LOG_WARNING);
			eInfo.tamY = 1;
		}
		if (eInfo.vision <= 1) {
			ErrorLog::getInstance()->escribirLog("Rango visión inválido en la entidad [" + eInfo.nombre + "]. Se utiliza valor por defecto.", LOG_WARNING);
			eInfo.vision = 1;
		}

		pType->tamX = eInfo.tamX;
		pType->tamY = eInfo.tamY;
		pType->vision = eInfo.vision;
		pType->score = eInfo.score;
		pType->velocidad = eInfo.velocidad;

		// Harcodeado temporalmente hasta definir como se crearán tipos
		// de unidades distintos desde yaml.
		if (eInfo.tipo == "resource")
			pType->tipo = ENT_T_RESOURCE;
		else if (eInfo.tipo == "unit")
			pType->tipo = ENT_T_UNIT;
		else if (eInfo.tipo == "building")
			pType->tipo = ENT_T_BUILDING;

		prototipos[eInfo.nombre] = pType;
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
		switch (pType->tipo) {
		case ENT_T_RESOURCE:
			ent = new Recurso(obtenerIDValida(), name, pType->tamX, pType->tamX, pType->vision, pType->score); break;
		case ENT_T_UNIT: {
			ent = new Unidad(obtenerIDValida(), name, pType->tamX, pType->tamX, pType->vision, pType->velocidad);
			break;
			 }
		case ENT_T_NONE:
		default:
			ent = new Entidad(obtenerIDValida(), name, pType->tamX, pType->tamX, pType->vision); break;
		}
		ent->tipo = pType->tipo;
	} else {
		ErrorLog::getInstance()->escribirLog("Entidad [" + name + "] no existe en sistema. Se reemplazará por entidad por defecto.", LOG_WARNING);
		pType = prototipos[nombre_entidad_def];
		ent = new Entidad(obtenerIDValida(), nombre_entidad_def, pType->tamX, pType->tamX, pType->vision);
	} 
	return ent;
}


// Método para poder enviar la lista de clases al cliente

list<msg_tipo_entidad*> FactoryEntidades::obtenerListaTipos(void) {
	list<msg_tipo_entidad*> lista = list<msg_tipo_entidad*>();
	for (map<string, tipoEntidad_t*>::const_iterator iter = this->prototipos.begin(); iter != this->prototipos.end(); ++iter) {
		msg_tipo_entidad* msg = new msg_tipo_entidad();
		
		int last = iter->first.copy(msg->name, 49, 0);
		msg->name[last] = '\0';
				
		msg->tamX = iter->second->tamX;
		msg->tamY= iter->second->tamY;
		msg->tipo = iter->second->tipo;
		msg->velocidad = iter->second->velocidad;
		msg->vision = iter->second->vision;
		msg->score = iter->second->score;
		lista.push_back(msg);	
	}
	return lista;
}