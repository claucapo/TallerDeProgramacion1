#include "ConfigParser.h"
#include "ErrorLog.h"
#include <fstream>
#include <yaml-cpp\yaml.h>

// Definici�n de las claves para los hashes del YAML
#define RED_KEY "red"
	#define PUERTO_KEY "port"
	#define IP_KEY "ip"
	#define ID_KEY "ID"
	#define ALIAS_KEY "alias"

#define PANTALLA_KEY "pantalla"
	#define ANCHO_KEY "ancho"
	#define ALTO_KEY "alto"
	#define FULLSCREEN_KEY "fullscreen"

#define GAMEPLAY_KEY "gameplay"
	#define MARGEN_KEY "scroll_margen"
	#define SCROLL_KEY "scroll_vel"

#define LOG_KEY "log"
	#define WARNINGS_KEY "warnings"
	#define INFO_KEY "info"
	#define ERRORS_KEY "errors"

#define TIPO_KEY "tipo"
	#define NOMBRE_KEY "nombre"
	#define PATH_KEY "ruta"
	#define SUB_X_KEY "subX"
	#define SUB_Y_KEY "subY"
	#define ALIGN_X_KEY "pixelX"
	#define ALIGN_Y_KEY "pixelY"
	#define FPS_KEY "fps"
	#define DELAY_KEY "delay"


/* Descripci�n de los errores
NO_SUCH_FILE_ERR: No se puede encontrar la ruta especificada, se utilizar� la ruta por defecto.
NO_DEFAULT_FILE_ERR: No existe archivo de configuraci�n por defecto, se utilizar�n valores iniciales (dentro del c�digo).
						La ruta por defecto es "default.yaml"
MISSING_SECTION_ERR: No se puede encontrar una secci�n de configuraci�n dentro del yaml. (i.e. "pantalla" o "gameplay"), se usan valores por defecto.
MISSING_VALUE_ERR: No se puede encontrar referencia a un valor espec�fico dentro de una categor�a.
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

// Levanta un error generado a partir del c�digo y un mensaje espec�fico.
// El booleano "required" determinar� si se trata de un ERROR o un WARNING
void raiseError(yaml_error_code_t code, std::string msg, log_lvl_t log_lvl = LOG_ERROR) {
	ErrorLog::getInstance()->escribirLog(msg, log_lvl);
}


// Constructor por defecto, prepara el parseo del archivo en la ruta por defecto
ConfigParser::ConfigParser() {
	this->pInfo = struct pantallaInfo_t();
	this->gInfo = struct gameplayInfo_t();
	this->lInfo = struct logInfo_t();
	this->eInfoL = std::list<entidadInfo_t*>();
	this->path = PATH_DEFAULT;
	this->rInfo = struct redInfo_t();
}

// Constructor y m�todos espec�ficos para cargar directorio arbitrario
ConfigParser::ConfigParser(std::string path) {
	this->pInfo = struct pantallaInfo_t();
	this->gInfo = struct gameplayInfo_t();
	this->lInfo = struct logInfo_t();
	this->eInfoL = std::list<entidadInfo_t*>();
	this->rInfo = struct redInfo_t();
	
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
			raiseError(NO_SUCH_FILE_ERR, "Error al cargar el archivo [" + newpath +"]. Se utilizar� la rutapor defecto", LOG_ERROR);
		fstream.close();
	}
}

// Destrutor por si se lo aloca en memoria, y funci�n para liberar recursos
ConfigParser::~ConfigParser() {
	this->limpiar();
}

// Funci�n para liberar recursos.
// IMPORTANTE: debe de llamarse al terminar de operar con los structs para liberar
// las listas de entidades e instancias, que est�n pedidas din�micamente.
void ConfigParser::limpiar() {
	while (!this->eInfoL.empty()) {
		delete this->eInfoL.front();
		this->eInfoL.pop_front();
	}
}



//**********************************************************//
//******************* PARSER DE LITERALES ******************//
//**********************************************************//

// Realizan la operaci�n node[clave] >> target,
// y eleva una excepci�n dejando a target en default si no se encuentra una clave.
// Pre: target ya est� inicializado en su valor por defecto
// Post: se sobrescribi� target con el valor correspondiente a clave, si es que existe
void parsearEntero(const YAML::Node& node, std::string clave, int* target, log_lvl_t log_lvl = LOG_ERROR, bool allow_zero = true) {
	try {
		int tmp = -1;
		node[clave] >> tmp;
		if (tmp < 0) {
			raiseError(FORMAT_ERR, "Se ley� un n�mero negativo al parsear [" + clave + "]. Se tomar� el vlaor por defecto", LOG_WARNING);
		} else if (tmp == 0 && !allow_zero) {
			raiseError(FORMAT_ERR, "Valor de [" + clave + "] debe ser distinto de cero. Se tomar� el valor por defecto", LOG_WARNING);
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
			raiseError(FORMAT_ERR, "Se ley� un n�mero negativo al parsear [" + clave + "]. Se tomar� el vaolr por defecto", LOG_WARNING);
		} else if (tmp == 0 && !allow_zero) {
			raiseError(FORMAT_ERR, "Valor de [" + clave + "] debe ser distinto de cero. Se tomar� el valor por defecto", LOG_WARNING);
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

// Informacion de la pantalla (struct pantallaInfo_t)
void operator >>(const YAML::Node& node, pantallaInfo_t& pInfo) {
	// Estos dos valores los chequeo juntos para que no exista la posibilidad
	// de que la pantalla quede deformada
	parsearBoolean(node, FULLSCREEN_KEY, &pInfo.fullscreen, LOG_INFO);

	try {
		int h = 640;
		int w = 480;
		node[ANCHO_KEY] >> w;

		node[ALTO_KEY] >>  h;
		if (w <= 0 || h <= 0)
			raiseError(FORMAT_ERR, "Tama�o de pantalla inv�ido. Usando tama�o por defecto 640x480", LOG_WARNING);
		else {
			pInfo.screenW = w;
			pInfo.screenH = h;
		}
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_VALUE_ERR, e.what());
		pInfo = pantallaInfo_t();
	} catch (YAML::Exception e) {
		raiseError(FORMAT_ERR, e.what());
		pInfo = pantallaInfo_t();
	}
}

// Informacion de jugabilidad (gameplayInfo_t)
void operator >>(const YAML::Node& node, gameplayInfo_t& gInfo) {
	parsearEntero(node, MARGEN_KEY, &gInfo.scroll_margen, LOG_INFO, false);
	parsearEntero(node, SCROLL_KEY, &gInfo.scroll_vel, LOG_INFO, false);
	
}

// Informacion de una clase (entidad) dentro del juego (entidadInfo_t)
// Dentro de esta categoria se incluye la informacion comun a todas las instancias
// de una misma entidad, como lo es el directorio de la ruta o el tamanio
void operator >> (const YAML::Node& node, entidadInfo_t& eInfo) {
	parsearString(node, NOMBRE_KEY, &eInfo.nombre);
	parsearString(node, PATH_KEY, &eInfo.spritePath);

	parsearEntero(node, FPS_KEY, &eInfo.fps, LOG_INFO, false);
	parsearEntero(node, DELAY_KEY, &eInfo.delay, LOG_INFO);

	parsearEntero(node, SUB_X_KEY, &eInfo.subX, LOG_INFO, false);
	parsearEntero(node, SUB_Y_KEY, &eInfo.subY, LOG_INFO, false);
	parsearEntero(node, ALIGN_X_KEY, &eInfo.pixel_align_X, LOG_WARNING);
	parsearEntero(node, ALIGN_Y_KEY, &eInfo.pixel_align_Y, LOG_WARNING);

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



// Informacion del escenario (escenarioInfo_t)
// Contiene el tamanio del mapa y la lista de entidades que hay en el mismo.
void operator >> (const YAML::Node& node, redInfo_t& rInfo) {
	parsearString(node, ALIAS_KEY, &rInfo.name, LOG_WARNING);
	parsearString(node, IP_KEY, &rInfo.ip, LOG_WARNING);

	parsearEntero(node, PUERTO_KEY, &rInfo.port, LOG_WARNING, false);
	parsearEntero(node, ID_KEY, &rInfo.ID, LOG_WARNING, false);
}



//******************************************************//
//**************** PARSER DE CATEGOR�AS ****************//
//******************************************************//

// Trata de obtener la configuraci�n de pantalla y almacenarla en pInfo
// Pre: pInfo ya se inicializ� previamente a los valores por defecto
// Post: si se puede leer la configuraci�n, se sobrescriben los valores, en caso
// contrario se levanta una excepci�n.
void parsearInfoPantalla(const YAML::Node& node, pantallaInfo_t* pInfo) {
	try {
		node[PANTALLA_KEY] >> *pInfo;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what());
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}


// Trata de obtener la configuraci�n de jugabilidad y almacenarla en gInfo
// Pre: gInfo ya se inicializ� previamente a los valores por defecto
// Post: si se puede leer la configuraci�n, se sobrescriben los valores, en caso
// contrario se levanta una excepci�n.
void parsearInfoGameplay(const YAML::Node& node, gameplayInfo_t* gInfo) {
	try {
		node[GAMEPLAY_KEY] >> *gInfo;
	} catch (YAML::KeyNotFound e) {
		raiseError(MISSING_SECTION_ERR, e.what());
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}

// Carga la info del log.
// Pre: el struct se inicializ� a los valores por defecto.
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

// Carga la informacion de todas las entidades que encuentre en la lista atributo de ConfigParser.
// Pre: la lista esta vacia previamente.
// Post: la lista contiene una coleccion de entidadInfo_t con todos los datos leidos, poniendo en default los faltantes.
void parsearInfoEntidades(const YAML::Node& node, std::list<entidadInfo_t*>* eInfoL){
	try {
		node[TIPO_KEY] >> *eInfoL;
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
				raiseError(NO_DEFAULT_FILE_ERR, "Error cr�tico!!! No se pudo encontrar ruta de configuraci�n por defecto!!! Revise el directorio [" + this->path + "]", LOG_ERROR);
			else
				raiseError(NO_DEFAULT_FILE_ERR, "Error cr�tico!!! No se pudo encontrar ruta de configuraci�n!!! Revise el directorio [" + this->path + "]", LOG_ERROR);
		}
		YAML::Parser parser(file);
		YAML::Node doc;
		parser.GetNextDocument(doc);

		this->limpiar();
		parsearInfoLog(doc, &this->lInfo);
		ErrorLog::getInstance()->habilitarFlags( this->lInfo.warnings, this->lInfo.info, this->lInfo.errors );
		
		parsearInfoPantalla(doc, &this->pInfo);
		parsearInfoGameplay(doc, &this->gInfo);
		parsearInfoEntidades(doc, &this->eInfoL);
		parsearInfoRed(doc, &this->rInfo);

		file.close();
	} catch (YAML::Exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	} catch (std::exception e) {
		raiseError(UNKNOWN_ERR, e.what());
	}
}