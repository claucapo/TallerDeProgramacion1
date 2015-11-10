#include "FactoryEntidades.h"
#include "ErrorLog.h"
#include "Enumerados.h"
#include "Recurso.h"
#include "Protocolo.h"

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

void FactoryEntidades::agregarEntidad(msg_tipo_entidad eInfo) {
	string nombre(eInfo.name);

	// El nombre de la entidad no contiene subcadenas de estados como "_move"
	if (nombre.find(estados_extensiones[EST_CAMINANDO]) != string::npos) {
		return;
	}

	// El nombre de la entidad no es el valor por defecto
	if (nombre == nombre_entidad_def) {
		ErrorLog::getInstance()->escribirLog("Tipo de instancia [" + nombre_entidad_def + "] reservada. No se reemplazará el tipo por defecto.", LOG_WARNING);
		return;
	}

	// Si el nombre ya existía no lo agrego e imprimo un error
	if (prototipos.count(nombre) > 0) {
		ErrorLog::getInstance()->escribirLog("Definición de entidad [" + nombre + "] duplicada. No se sobreescribirán los datos", LOG_WARNING);
	} else {
		tipoEntidad_t* pType = new tipoEntidad_t();

		// Estos chequeos se pueden sacar... ya que el yaml siempre tira datos válidos
		if (eInfo.tamX <= 0) {
			ErrorLog::getInstance()->escribirLog("Tamaño inválido en la entidad [" + nombre + "]. Se utiliza valor por defecto.", LOG_WARNING);
			eInfo.tamX = 1;
		}
		if (eInfo.tamY <= 0) {
			ErrorLog::getInstance()->escribirLog("Tamaño inválido en la entidad [" + nombre + "]. Se utiliza valor por defecto.", LOG_WARNING);
			eInfo.tamY = 1;
		}
		if (eInfo.rangoV <= 1) {
			ErrorLog::getInstance()->escribirLog("Rango visión inválido en la entidad [" + nombre + "]. Se utiliza valor por defecto.", LOG_WARNING);
			eInfo.rangoV = 1;
		}


		pType->tipo = eInfo.tipo;
		pType->tipoR = eInfo.tipoR;

		pType->tamX = eInfo.tamX;
		pType->tamY = eInfo.tamY;
		pType->rangoV = eInfo.rangoV;
		pType->recursoMax = eInfo.recursoMax;
		pType->velocidad = eInfo.velocidad;

		pType->ataque = eInfo.ataque;
		pType->defensa = eInfo.defensa;
		pType->vidaMax = eInfo.vidaMax;

		pType->collectRate = eInfo.collectRate;
		pType->buildRate = eInfo.buildRate;
		pType->cooldown = eInfo.cooldown;

		if (pType->tipo == ENT_T_BUILDING) {
			for (unsigned int i = 0; i < eInfo.cant_entrenables; i++) {
				std::string nuevoEntrenable(&eInfo.entrenables[i * 50], 50);
				pType->entrenables.push_front(nuevoEntrenable);
			}
		}

		for (unsigned int i = 0; i < CANT_ACCIONES; i++)
			pType->habilidades[i] = eInfo.habilidades[i];

		prototipos[nombre] = pType;
	}
}

Entidad* FactoryEntidades::obtenerEntidad(string name, unsigned int id){
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
			ent = new Recurso(id, name, *pType); break;
		case ENT_T_UNIT:
			ent = new Unidad(id, name, *pType); break;
		case ENT_T_NONE:
		default:
			ent = new Entidad(id, name, *pType);
			
		}
	//	ent->tipo = pType->tipo;
	} else {
		ErrorLog::getInstance()->escribirLog("Entidad [" + name + "] no existe en sistema. Se reemplazará por entidad por defecto.", LOG_WARNING);
		pType = prototipos[nombre_entidad_def];
		ent = new Entidad(id, nombre_entidad_def, *pType);
	} 
	return ent;
}


