#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

/* Ejemplo de uso:

int main (int argc, char** argv) {
	// Primero se crea el parser
	ConfigParser parser = ConfigParser();

	// [Opcional] Se le asigna una ruta distinta ("default.yaml" por defecto)
	parser.setPath("otroPath.yaml");

	// Se parsea el archivo
	parser.parsearTodo();

	// IMPORTANTE: si no se llama a parsearTodo(), toda la informacion que se obtenga
	// del parser seran los valores establecidos por defecto en los #define


	// Se accede a cada "categoria" de informacion por separado mediante su propio 
	// tipo de dato struct, o a traves de una lista de los mismos.
	// OJO!! hay valores pedidos dinamicamente, ver la documentacion de cada categoria.

	// Pedir informacion de la pantalla
	pantallaInfo_t pInfo = parser.verInfoPantalla();

	// Accediendo a los miembros se obtienen los datos especificos
	int ancho = pInfo.screenW;

	
	// Pedir informacion del escenario
	escenarioInfo_t sInfo = parser.verInfoEscenario();
	// Lista de instancias
	std::list<instanciaInfo_t*> iInfoL = sInfo.instancias;
	for (std::list<instanciaInfo_t*>::const_iterator iter = iInfoL.begin(); iter != iInfoL.end(); ++iter) {
		// iter es un puntero al tipo generico de la lista, que a su vez es un puntero a una instanciaInfo_t,
		// por eso hay que desreferenciar dos veces.
		instanciaInfo_t iInfo = *(*iter);
		// Metodo ficticio
		unMapa.insertarEntidad(iInfo.tipo, iInfo.x, iInfo.y);
	}

	// IMPORTANTE: si alguno de los structs tiene una lista, los elementos en ella son pedidos dinamicamente
	// en memoria, y deben manipularse con cuidados. Estos datos seran borrados si: 1) se elimina el parser (la
	// variable parser estatica muere en la llamada), o se borra el parser manualente. 2) Se llama manualmente
	// al metodo parser.limpiar() o 3) se llama al metodo parser.parsearTodo() nuevamente, lo cual resetea a todo
	// a valores por defecto para iniciar el parseo y limpia cualquier lista.

	
	// Para terminar de operar no es obligatorio nada mas, pero para estar seguros de que se libere la memoria.
	parser.limpiar();

	// NOTA: una vez hecho esto, la referencia a iInfoL apunta a una lista con punteros a memoria liberada, por lo
	// que habria que asegurarse de no volver a utilizarlo o reasignarlo a otra variable.
	// Otra posible solucion es copiar la lista del parser en lugar de referenciarla

	// Esto soluciona el problema de la referencia, en caso de que se utilice de esta manera.
	iInfoL = parser.verInfoEscenario().instancias;
}

*/

#include <string>
#include <list>

// Definici�n de valores por defecto (otros valores por defecto mas evidentes dentro de las definiciones de
// los constructores)
#define PATH_DEFAULT "default.yaml"

#define PANTALLA_W_DEFAULT 800
#define PANTALLA_H_DEFAULT 600

#define VELOCIDAD_DEFAULT 3
#define MARGEN_DEFAULT 30

#define TYPE_NAME_DEFAULT "unknown"
#define IMG_PATH_DEFAULT "no_image.png"

#define SCENARY_NAME_DEAFAULT "orleans"



// STRUCTS (categorias) //

// Struct que define la categor�a pantalla
struct pantallaInfo_t {
	// Tama�o de la pantalla
	int screenW;
	int screenH;

	// Inicializa los valores a cero por defecto
	pantallaInfo_t(): screenW(PANTALLA_W_DEFAULT), screenH(PANTALLA_H_DEFAULT){};
};

// Struct que define la categor�a gameplay (jugabilidad)
struct gameplayInfo_t {
	// Velocidad del personaje
	int velocidad;
	int margenScroll;

	// Inicializa los valores a valores por defecto
	gameplayInfo_t(): velocidad(VELOCIDAD_DEFAULT), margenScroll(MARGEN_DEFAULT){};
};

// Struct que define la informaci�n acerca de una unidad (general)
struct entidadInfo_t {
	std::string nombre;
	std::string spritePath;
	int tamX;
	int tamY;
	int pixel_align_X;
	int pixel_align_Y;

	// Inicializa los valores a cero por defecto
	entidadInfo_t(): nombre(TYPE_NAME_DEFAULT), spritePath(IMG_PATH_DEFAULT), tamX(1), tamY(1), pixel_align_X(0), pixel_align_Y(0) {};
};


// Struct que representa una instancia de alguna entidad dentro del escenario
struct instanciaInfo_t {
	int x;
	int y;
	std::string tipo;

	// Inicializa los valores a cero por defecto
	instanciaInfo_t(): x(0), y(0), tipo(TYPE_NAME_DEFAULT) {};
};

// Struct que define la informaci�n acerca del escenario
struct escenarioInfo_t {
	std::string name;
	int size_X;
	int size_Y;
	std::list<instanciaInfo_t*> instancias;
	instanciaInfo_t protagonista;

	escenarioInfo_t(): name(SCENARY_NAME_DEAFAULT), size_X(100), size_Y(100), instancias(), protagonista() {};
};

// PARSER //

class ConfigParser {
private:
	std::string path;

	pantallaInfo_t pInfo;
	gameplayInfo_t gInfo;
	std::list<entidadInfo_t*> eInfoL;
	escenarioInfo_t sInfo;

public:
	ConfigParser();
	ConfigParser(std::string path);
	~ConfigParser();

	// Permite especificar una ruta nueva
	void setPath(std::string newPath);
	void parsearTodo();
	void limpiar();

	// Devuelve distintas estructuras con informaci�n
	pantallaInfo_t verInfoPantalla() {return this->pInfo;}
	gameplayInfo_t verInfoGameplay() {return this->gInfo;}
	std::list<entidadInfo_t*> verInfoEntidades() {return this->eInfoL;}	// CUIDADO: Memoria dinamica en los elementos de eInfoL
	escenarioInfo_t verInfoEscenario() {return this->sInfo;}				// CUIDADO: Memoria dinamica en los elementos de sInfo.instancias
};

#endif