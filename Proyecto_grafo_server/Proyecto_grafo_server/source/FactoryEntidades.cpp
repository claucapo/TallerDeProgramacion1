#include "FactoryEntidades.h"
#include "ErrorLog.h"
#include "Enumerados.h"
#include "Recurso.h"
#include "Edificio.h"
#include "Protocolo.h"

bool FactoryEntidades::hay_instancia = false;
FactoryEntidades* FactoryEntidades::singleton = nullptr;
unsigned int FactoryEntidades::nextID = 0;
unsigned int FactoryEntidades::nextTypeID = 0;

// Constructores y destructores para singleton!!!

FactoryEntidades::FactoryEntidades(void) {
	this->prototipos = map<string, tipoEntidad_t*>();
	tipoEntidad_t* def = new tipoEntidad_t();
	def->typeID = this->obtenerTypeIDValida();
	this->prototipos[nombre_entidad_def] = def;
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
		tipoEntidad_t* def = new tipoEntidad_t();
		nextTypeID = 0;
		def->typeID = this->obtenerTypeIDValida();
		this->prototipos[nombre_entidad_def] = def;
	}
}

unsigned int FactoryEntidades::obtenerIDValida() {
	return nextID++;
}

unsigned int FactoryEntidades::obtenerTypeIDValida() {
	return nextTypeID++;
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
		if (eInfo.rangoV <= 1) {
			ErrorLog::getInstance()->escribirLog("Rango visión inválido en la entidad [" + eInfo.nombre + "]. Se utiliza valor por defecto.", LOG_WARNING);
			eInfo.rangoV = 1;
		}


		// Harcodeado temporalmente hasta definir como se crearán tipos
		// de unidades distintos desde yaml.
		if (eInfo.tipo == "resource")
			pType->tipo = ENT_T_RESOURCE;
		else if (eInfo.tipo == "unit") {
			// Solo las unidades pueden tener interacciones
			pType->tipo = ENT_T_UNIT;
			for (list<string>::iterator it = eInfo.habilidades.begin(); it != eInfo.habilidades.end(); it++) {
				std::string act = (*it);
				if (act == "attack") {
					pType->habilidades[ACT_ATACK] = true;
				} else if (act == "build") {
					pType->habilidades[ACT_BUILD] = true;
				} else if (act == "collect") {
					pType->habilidades[ACT_COLLECT] = true;
				} else {
					cout << "Habilidad: [" << act.c_str() << "] no reconocida." << endl;
				}
			}
		} else if (eInfo.tipo == "building") {
			// Solo los edificios (TBI) puede tener listas de entrenables
			pType->tipo = ENT_T_BUILDING;
			pType->cantidad_entrenables = eInfo.entrenables.size();
			if (pType->cantidad_entrenables > MAX_ENTRENABLES)
				ErrorLog::getInstance()->escribirLog("Edificio [" + eInfo.nombre +"] supera cantidad maxima de entrenables. Solo se toman los primeros valores.", LOG_WARNING);
			int i = 0;
			for (list<string>::iterator it = eInfo.entrenables.begin(); it != eInfo.entrenables.end(); it++) {
				std::string act = (*it);
				pType->entrenables[i] = act;
				i++;
			}
		}

		pType->typeID = this->obtenerTypeIDValida();
		pType->tamX = eInfo.tamX;
		pType->tamY = eInfo.tamY;
		pType->rangoV = eInfo.rangoV;
		pType->recursoMax = eInfo.recursoMax;
		pType->velocidad = eInfo.velocidad;

		pType->costo.comida = eInfo.costoComida;
		pType->costo.oro = eInfo.costoOro;
		pType->costo.madera = eInfo.costoMadera;
		pType->costo.piedra = eInfo.costoPiedra;


		pType->ataque = eInfo.ataque;
		pType->defensa = eInfo.defensa;
		pType->vidaMax = eInfo.vidaMax;

		pType->collectRate = eInfo.collectRate;
		pType->buildRate = eInfo.buildRate;
		pType->trainRate = eInfo.trainRate;
		pType->cooldown = eInfo.cooldown;
		
		
		// Mapeo provisorio, hasta redefinir yaml para tercer entrega?
		switch (eInfo.tipoR) {
		case 0: pType->tipoR = RES_T_GOLD; break;
		case 1: pType->tipoR = RES_T_WOOD; break;
		case 2: pType->tipoR = RES_T_FOOD; break;
		case 3: pType->tipoR = RES_T_STONE; break;
		default:
			pType->tipoR = RES_T_NONE;
		}
		
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
			ent = new Recurso(obtenerIDValida(), name, *pType); break;
		case ENT_T_UNIT:
			ent = new Unidad(obtenerIDValida(), name, *pType);
			break;
		case ENT_T_BUILDING:
			ent = new Edificio(obtenerIDValida(), name, *pType);
			break;
		case ENT_T_NONE:
		default:
			ent = new Entidad(obtenerIDValida(), name, *pType); break;
		}
		ent->tipo = pType->tipo;
	} else {
		ErrorLog::getInstance()->escribirLog("Entidad [" + name + "] no existe en sistema. Se reemplazará por entidad por defecto.", LOG_WARNING);
		pType = prototipos[nombre_entidad_def];
		ent = new Entidad(obtenerIDValida(), nombre_entidad_def, *pType);
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
		msg->typeID = iter->second->typeID;

		msg->velocidad = iter->second->velocidad;
		msg->rangoV = iter->second->rangoV;
		msg->rangoA = iter->second->recursoMax;
		msg->ataque = iter->second->ataque;
		msg->defensa = iter->second->defensa;
		msg->vidaMax = iter->second->vidaMax;

		msg->tipoR = iter->second->tipoR;
		msg->recursoMax = iter->second->recursoMax;

		msg->costo = iter->second->costo;

		msg->collectRate = iter->second->collectRate;
		msg->buildRate = iter->second->buildRate;
		msg->trainRate = iter->second->trainRate;
		msg->cooldown = iter->second->cooldown;

		for (int i = 0; i < CANT_ACCIONES; i++) {
			msg->habilidades[i] = iter->second->habilidades[i];
		}		

		// Serialización de las entidades entrenables
		msg->cant_entrenables = iter->second->cantidad_entrenables;
		if (msg->cant_entrenables > 0) {
			char* tmp = new char[msg->cant_entrenables * 50];

			// Para cada entrenable, inserto la cadena en el arreglo dinámico
			unsigned int i = 0;
			for (i = 0; i < msg->cant_entrenables; i++) {
				// Esto copia hasta 49 caracteres en el arreglo, a partir de un offset determinado
				// por la posición actual de la entidad en la lista de entrenables
				int last = (iter->second->entrenables[i]).copy(&(tmp[i * 50]), 49, 0);
				tmp[i * 50 + last] = '\0';
			}
			msg->entrenables = tmp;
		} else {
			msg->entrenables = NULL;
		}

		lista.push_back(msg);
	}
	return lista;
}



Entidad* FactoryEntidades::obtenerEntidad(unsigned int id) {
	Entidad* ent = nullptr;
	tipoEntidad_t* pType = nullptr;
	std::string name = nombre_entidad_def;

	for (map<string, tipoEntidad_t*>::const_iterator iter = this->prototipos.begin(); iter != this->prototipos.end(); ++iter) {
		if ((*iter).second->typeID == id) {
			pType = (*iter).second;
			name = (*iter).first;
		}
	}

	if (pType != nullptr) {
		switch (pType->tipo) {
		case ENT_T_RESOURCE:
			ent = new Recurso(obtenerIDValida(), name, *pType); break;
		case ENT_T_UNIT:
			ent = new Unidad(obtenerIDValida(), name, *pType);
			break;
		case ENT_T_BUILDING:
			ent = new Edificio(obtenerIDValida(), name, *pType);
			break;
		case ENT_T_NONE:
		default:
			ent = new Entidad(obtenerIDValida(), name, *pType); break;
		}
		ent->tipo = pType->tipo;
	} else {
		ErrorLog::getInstance()->escribirLog("Entidad no existe en sistema. Se reemplazará por entidad por defecto.", LOG_WARNING);
		pType = prototipos[nombre_entidad_def];
		ent = new Entidad(obtenerIDValida(), nombre_entidad_def, *pType);
	} 
	return ent;
}



unsigned int FactoryEntidades::obtenerTypeID(string name) {
	if (prototipos.count(name) > 0) {
		return prototipos[name]->typeID;
	}
	return 0;
}