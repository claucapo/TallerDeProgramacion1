#include "ConfigParser.h"
#include "ErrorLog.h"
#include <fstream>
#include <yaml-cpp\yaml.h>

// Definición de las claves para los hashes del YAML

#define RED_KEY "red"
	#define PUERTO_KEY "puerto"

#define LOG_KEY "log"
	#define WARNINGS_KEY "warnings"
	#define INFO_KEY "info"
	#define ERRORS_KEY "errors"

#define JUGADORES_KEY "jugadores"
	#define CANTIDAD_KEY "cantidad"
	#define ID_KEY "id"
	#define COLOR_KEY "color"

#define ENTIDADES_KEY "entidades"
	#define TIPO_KEY "tipo"
	#define NOMBRE_KEY "nombre"
	#define TAM_X_KEY "sizeX"
	#define TAM_Y_KEY "sizeY"

	#define RANGE_VIEW_KEY "rangoV"
	#define RANGE_ATK_KEY "rangoA"

	#define RECURSO_TIPO_KEY "tipoR"
	#define RECURSO_MAX_KEY "maxR"

	#define VELOCIDAD_KEY "velocidad"
	#define ATTACK_KEY "atk"
	#define DEFENSE_KEY "def"
	#define HP_MAX_KEY "vidaMax"

	#define COOLDOWN_KEY "cooldown"
	#define RATE_COLLECT_KEY "collectR"
	#define RATE_BUILD_KEY "buildR"
	#define RATE_TRAIN_KEY "trainR"

	#define COST_GOLD_KEY "costGold"
	#define COST_FOOD_KEY "costFood"
	#define COST_STONE_KEY "costStone"
	#define COST_WOOD_KEY "costWood"

	#define TRAINING_KEY "entrenables"
	#define HABLIDADES_KEY "habilidades"

#define ESCENARIO_KEY "escenario"
	#define COORD_X_KEY "x"
	#define COORD_Y_KEY "y"
	#define INSTANCE_PLAYER_KEY "p"


/* Descripción de los errores
NO_SUCH_FILE_ERR: No se puede encontrar la ruta especificada, se utilizará la ruta por defecto.
NO_DEFAULT_FILE_ERR: No existe archivo de configuración por defecto, se utilizarán valores iniciales (dentro del código).
						La ruta por defecto es "default.yaml"
MISSING_SECTION_ERR: No se puede encontrar una sección de configuración dentro del yaml. (i.e. "pantalla" o "gameplay"), se usan valores por defecto.
MISSING_VALUE_ERR: No se puede encontrar referencia a un valor específico dentro de una categoría.
FORMAT_ERR: Error en el formato del yaml

UNKNOWN_ERR: Error desconocido
*/
enum yaml_error_code_t {NO_ERROR = 0,
						NO_SUCH_FILE_ERR, 
						NO_DEFAULT_FILE_ERR, 
						MISSING_SECTION_ERR, 
						MISSING_VALUE_ERR, 
						FORMAT_ERR, 
						MEMORY_ERR,
						UNKNOWN_ERR  
						};

// Levanta un error generado a partir del código y un mensaje específico.
// El booleano "required" determinará si se trata de un ERROR o un WARNING
void raiseError(yaml_error_code_t code, std::string msg, log_lvl_t log_lvl = LOG_ERROR) {
	ErrorLog::getInstance()->escribirLog(msg, log_lvl);
}


// Constructor por defecto, prepara el parseo del archivo en la ruta por defecto
ConfigParser::ConfigParser() {
	this->jInfoL = std::list<jugadorInfo_t*>();
	this->lInfo = struct logInfo_t();
	this->eInfoL = std::list<entidadInfo_t*>();
	this->path = PATH_DEFAULT;
	this->sInfo = struct escenarioInfo_t();
}

// Constructor y métodos específicos para cargar directorio arbitrario
ConfigParser::ConfigParser(std::string path) {
	this->jInfoL = std::list<jugadorInfo_t*>();
	this->lInfo = struct logInfo_t();
	this->eInfoL = std::list<entidadInfo_t*>();
	this->sInfo = struct escenarioInfo_t();
	
	if (!path.empty())
		this->path = path;
	else
		this->path = PATH_DEFAULT;
}
void ConfigParser::setPath(std::string newpath) {
	this->path = PATH_DEFAULT;

	if (!path.empty()) {
		std::ifstream fstream(newpath.c_str());
		if (fstream.good())
			this->path = newpath;
		else
			raiseError(NO_SUCH_FILE_ERR, "Error al cargar el archivo [" + newpath +"]. Se utilizará la rutapor defecto", LOG_ERROR);
		fstream.close();
	}
}

// Destrutor por si se lo aloca en memoria, y función para liberar recursos
ConfigParser::~ConfigParser() {
	this->limpiar();
}

// Función para liberar recursos.
// IMPORTANTE: debe de llamarse al terminar de operar con los structs para liberar
// las listas de entidades e instancias, que están pedidas dinámicamente.
void ConfigParser::limpiar() {
	while (!this->jInfoL.empty()) {
		delete this->jInfoL.front();
		this->jInfoL.pop_front();
	}
	while (!this->eInfoL.empty()) {
		delete this->eInfoL.front();
		this->eInfoL.pop_front();
	}
	while (!this->sInfo.instancias.empty()) {
		delete this->sInfo.instancias.front();
		this->sInfo.instancias.pop_front();
	}
}



//**********************************************************//
//******************* PARSER DE LITERALES ******************//
//**********************************************************//

// Realizan la operación node[clave] >> target,
// y eleva una excepción dejando a target en default si no se encuentra una clave.
// Pre: target ya está inicializado en su valor por defecto
// Post: se sobrescribió target con el valor correspondiente a clave, si es que existe
void parsearEntero(const YAML::Node& node, std::string clave, int* target, log_lvl_t log_lvl = LOG_ERROR, bool allow_zero = true) {
	try {
		int tmp = -1;
		node[clave] >> tmp;
		if (tmp < 0) {
			raiseError(FORMAT_ERR, "Se leyó un número negativo al parsear [" + clave + "]. Se tomará el vlaor por defecto", LOG_WARNING);
		} else if (tmp == 0 && !allow_zero) {
			raiseError(FORMAT_ERR, "Valor de [" + clave + "] debe ser distinto de cero. Se tomará el valor por defecto", LOG_WARNING);
		} else {
			*target = tmp;
		}
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_VALUE_ERR, e.what(), log_lvl);
	} catch (YAML::Exception e) {
		raiseError(FORMAT_ERR, e.what(), log_lvl);
	}
}

void parsearString(const YAML::Node& node, std::string clave, std::string* target, log_lvl_t log_lvl = LOG_ERROR) {
	try {
		node[clave] >> *target;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_VALUE_ERR, e.what(), log_lvl);
	} catch (YAML::Exception e) {
		raiseError(FORMAT_ERR, e.what(), log_lvl);
	}
}

void parsearFloat(const YAML::Node& node, std::string clave, float* target, log_lvl_t log_lvl = LOG_ERROR, bool allow_zero = true) {
	try {
		float tmp = -1;
		node[clave] >> tmp;
		if (tmp < 0) {
			raiseError(FORMAT_ERR, "Se leyó un número negativo al parsear [" + clave + "]. Se tomará el vaolr por defecto", LOG_WARNING);
		} else if (tmp == 0 && !allow_zero) {
			raiseError(FORMAT_ERR, "Valor de [" + clave + "] debe ser distinto de cero. Se tomará el valor por defecto", LOG_WARNING);
		} else {
			*target = tmp;
		}
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_VALUE_ERR, e.what(), log_lvl);
	} catch (YAML::Exception e) {
		raiseError(FORMAT_ERR, e.what(), log_lvl);
	}
}

void parsearBoolean(const YAML::Node& node, std::string clave, bool* target, log_lvl_t log_lvl = LOG_ERROR) {
	try {
		float number = 0;
		node[clave] >> number;
		if (number != 0)
			*target = true;
		else
			*target = false;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_VALUE_ERR, e.what(), log_lvl);
	} catch (YAML::Exception e) {
		raiseError(FORMAT_ERR, e.what(), log_lvl);
	}
}




//******************************************************//
//****************** PARSER DE STRUCTS *****************//
//******************************************************//

// Estos parser sobreescriben los operadores, y realizan la tarea de cargar nodos de yaml a los
// structs definidos en ConfigParser.h

// Información de un jugador en particular
void operator >> (const YAML::Node& node, jugadorInfo_t& jInfo) {
	parsearString(node, NOMBRE_KEY, &jInfo.nombre, LOG_ERROR);
	parsearEntero(node, ID_KEY, &jInfo.id, LOG_ERROR, false);
	parsearString(node, COLOR_KEY, &jInfo.color, LOG_INFO);
}


// Informacion de la lista de jugadores (std::list<jugadorInfo_t*>)
void operator >>(const YAML::Node& node, std::list<jugadorInfo_t*>& jInfoL) {
	try {	
		for (unsigned i = 0; i < node.size(); i++) {
			// Memoria pedida dinamicamente
			jugadorInfo_t* jInfo = new jugadorInfo_t();
			node[i] >> *(jInfo);
			jInfoL.push_back(jInfo);
		}
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what(), LOG_INFO);
	}
}

// Parsea una lista de strings, usada para obtener la lista de nombres
// de unidades entrenables
void operator >>(const YAML::Node& node, std::list<string>& aList) {
	try {
		for (unsigned i = 0; i < node.size(); i++) {
			std::string nElem;
			node[i] >> nElem;
			aList.push_back(nElem);
		}
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what(), LOG_INFO);
	}
}


// Informacion de una clase (entidad) dentro del juego (entidadInfo_t)
// Dentro de esta categoria se incluye la informacion comun a todas las instancias
// de una misma entidad, como lo es el directorio de la ruta o el tamanio
void operator >> (const YAML::Node& node, entidadInfo_t& eInfo) {
	parsearString(node, NOMBRE_KEY, &eInfo.nombre);
	parsearString(node, TIPO_KEY, &eInfo.tipo, LOG_INFO);

	parsearEntero(node, TAM_X_KEY, &eInfo.tamX, LOG_INFO, false);
	parsearEntero(node, TAM_Y_KEY, &eInfo.tamY, LOG_INFO, false);

	parsearEntero(node, RANGE_VIEW_KEY, &eInfo.rangoV, LOG_INFO, false);
	parsearEntero(node, RANGE_ATK_KEY, &eInfo.rangoA, LOG_INFO, false);

	parsearEntero(node, RECURSO_TIPO_KEY, &eInfo.tipoR, LOG_INFO, true);
	parsearEntero(node, RECURSO_MAX_KEY, &eInfo.recursoMax, LOG_INFO, true);

	parsearEntero(node, VELOCIDAD_KEY, &eInfo.velocidad, LOG_INFO, true);
	parsearEntero(node, HP_MAX_KEY, &eInfo.vidaMax, LOG_INFO, false);
	parsearEntero(node, ATTACK_KEY, &eInfo.ataque, LOG_INFO, true);
	parsearEntero(node, DEFENSE_KEY, &eInfo.defensa, LOG_INFO, true);

	parsearEntero(node, COOLDOWN_KEY, &eInfo.cooldown, LOG_INFO, true);
	parsearEntero(node, RATE_BUILD_KEY, &eInfo.buildRate, LOG_INFO, true);
	parsearEntero(node, RATE_COLLECT_KEY, &eInfo.collectRate, LOG_INFO, true);
	parsearEntero(node, RATE_TRAIN_KEY, &eInfo.trainRate, LOG_INFO, true);

	parsearEntero(node, COST_FOOD_KEY, &eInfo.costoComida, LOG_INFO, true);	
	parsearEntero(node, COST_GOLD_KEY, &eInfo.costoOro, LOG_INFO, true);
	parsearEntero(node, COST_WOOD_KEY, &eInfo.costoMadera, LOG_INFO, true);
	parsearEntero(node, COST_STONE_KEY, &eInfo.costoPiedra, LOG_INFO, true);


	try {	
		node[TRAINING_KEY] >> eInfo.entrenables;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what(), LOG_INFO);
	}
	
	try {	
		node[HABLIDADES_KEY] >> eInfo.habilidades;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what(), LOG_INFO);
	}
}

// Carga una lista de struct entidadInfo_t para poder guardar en simultaneo
// la informacion de todas las entidades.
// IMPORTANTE: la memoria para cada struct se pide dinamicamente!!
//				Ha de liberarse con parser.limpiar() o llamando al destructor.
void operator >> (const YAML::Node& node, std::list<entidadInfo_t*>& eInfoL) {
	try {
		for (unsigned i = 0; i < node.size(); i++) {
			// Memoria pedidad dinamicamente
			entidadInfo_t* eInfo = new entidadInfo_t();
			node[i] >> *eInfo;
			eInfoL.push_back(eInfo);
		}
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what(), LOG_ERROR);
	}
}

// Informacion de una instancia particualr (instanciaInfo_t)
void operator >> (const YAML::Node& node, logInfo_t& lInfo) {
	parsearBoolean(node, WARNINGS_KEY, &lInfo.warnings, LOG_INFO);
	parsearBoolean(node, INFO_KEY, &lInfo.info, LOG_INFO);
	parsearBoolean(node, ERRORS_KEY, &lInfo.errors, LOG_INFO);
}

// Informacion de una instancia particualr (instanciaInfo_t)
void operator >> (const YAML::Node& node, instanciaInfo_t& iInfo) {
	parsearString(node, TIPO_KEY, &iInfo.tipo);

	parsearEntero(node, COORD_X_KEY, &iInfo.x, LOG_INFO);
	parsearEntero(node, COORD_Y_KEY, &iInfo.y, LOG_INFO);

	parsearEntero(node, INSTANCE_PLAYER_KEY, &iInfo.player, LOG_INFO, true);
}

// Coleccion de instancias bajo un TDA de lista
// IMPORTANTE: la memoria para cada instancia se pide dinamicamente!!!
//				Liberar con parser.limpiar() o llamando al destructor.
void operator >> (const YAML::Node& node, std::list<instanciaInfo_t*>& iInfoL) {
	try {	
		for (unsigned i = 0; i < node.size(); i++) {
			// Memoria pedida dinamicamente
			instanciaInfo_t* iInfo = new instanciaInfo_t();
			node[i] >> *(iInfo);
			iInfoL.push_back(iInfo);
		}
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what(), LOG_INFO);
	}
}

// Informacion del escenario (escenarioInfo_t)
// Contiene el tamanio del mapa y la lista de entidades que hay en el mismo.
void operator >> (const YAML::Node& node, escenarioInfo_t& sInfo) {
	parsearString(node, NOMBRE_KEY, &sInfo.name, LOG_INFO);
	parsearEntero(node, TAM_X_KEY, &sInfo.size_X, LOG_WARNING, false);
	parsearEntero(node, TAM_Y_KEY, &sInfo.size_Y, LOG_WARNING, false);

	// Obtengo lista de entidades.
	try {
		node[ENTIDADES_KEY] >> sInfo.instancias;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what(), LOG_INFO);
	}
}

// Informacion de red
void operator >> (const YAML::Node& node, redInfo_t& rInfo) {
	parsearString(node, PUERTO_KEY, &rInfo.port, LOG_WARNING);
}


//******************************************************//
//**************** PARSER DE CATEGORÍAS ****************//
//******************************************************//


// Carga la info del log.
// Pre: el struct se inicializó a los valores por defecto.
// Post: si se pueden leer los datos del YAML, se sobreescriben en el struct.
void parsearInfoLog(const YAML::Node& node, logInfo_t* lInfo) {
	try {
		node[LOG_KEY] >> *lInfo;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, "Log se inicializa con valores por defecto: E:T, W:T, I:F", LOG_ALLWAYS);
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}

// Parsea información de los jugadores
void parsearInfoJugadores(const YAML::Node& node, std::list<jugadorInfo_t*>* jInfoL){
	try {
		node[JUGADORES_KEY] >> *jInfoL;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what());
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}


// Carga la informacion de todas las entidades que encuentre en la lista atributo de ConfigParser.
// Pre: la lista esta vacia previamente.
// Post: la lista contiene una coleccion de entidadInfo_t con todos los datos leidos, poniendo en default los faltantes.
void parsearInfoEntidades(const YAML::Node& node, std::list<entidadInfo_t*>* eInfoL){
	try {
		node[ENTIDADES_KEY] >> *eInfoL;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what());
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}

// Carga la informacion del escenario en el struct sInfo.
// Pre: los valores estan cargados por defecto, y la lista de instancias esta vacia.
// Post: se cargo la informacion del escenario y se pusieron en default los valores faltantes o erroneos.
void parsearInfoEscenario(const YAML::Node& node, escenarioInfo_t* sInfo) {
	try {
		node[ESCENARIO_KEY] >> *sInfo;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what());
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}

void parsearInfoRed(const YAML::Node& node, redInfo_t* rInfo) {
	try {
		node[RED_KEY] >> *rInfo;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what());
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}

// Parsea el archivo y almacena los resultados parciales en las variables del parser.
void ConfigParser::parsearTodo() {
	try {
		std::ifstream file(this->path);
		if (!file.good()) {
			if (this->path == PATH_DEFAULT)
				raiseError(NO_DEFAULT_FILE_ERR, "Error crítico!!! No se pudo encontrar ruta de configuración por defecto!!! Revise el directorio [" + this->path + "]", LOG_ERROR);
			else
				raiseError(NO_DEFAULT_FILE_ERR, "Error crítico!!! No se pudo encontrar ruta de configuración!!! Revise el directorio [" + this->path + "]", LOG_ERROR);
		}
		YAML::Parser parser(file);
		YAML::Node doc;
		parser.GetNextDocument(doc);

		this->limpiar();
		parsearInfoLog(doc, &this->lInfo);
		ErrorLog::getInstance()->habilitarFlags( this->lInfo.warnings, this->lInfo.info, this->lInfo.errors );
		
		parsearInfoJugadores(doc, &this->jInfoL);
		parsearInfoEntidades(doc, &this->eInfoL);
		parsearInfoEscenario(doc, &this->sInfo);
		parsearInfoRed(doc, &this->rInfo);

		file.close();
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	} catch (std::exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}