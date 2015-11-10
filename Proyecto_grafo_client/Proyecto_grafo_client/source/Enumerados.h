#ifndef ENUM_H
#define ENUM_H


#include <list>
using namespace std;

// Enumerado de posible direcciones de movimiento, escritos en un orden arbitrario para
// que concidan con las filas de los sprites de movimiento
enum Direcciones_t {DIR_RIGHT, DIR_TOP_RIGHT, DIR_TOP, DIR_TOP_LEFT, DIR_LEFT, DIR_DOWN_LEFT, DIR_DOWN, DIR_DOWN_RIGHT};

enum entity_type_t {ENT_T_NONE, ENT_T_UNIT, ENT_T_RESOURCE, ENT_T_BUILDING};
enum af_result_t {AF_NONE, AF_STATE_CHANGE, AF_MOVE, AF_KILL};
enum resource_type_t {RES_T_NONE = -1, RES_T_GOLD = 0, RES_T_WOOD = 1, RES_T_FOOD = 2, RES_T_STONE = 3};

// Estados
enum Estados_t {EST_QUIETO, EST_CAMINANDO, EST_ATACANDO, EST_RECOLECTANDO, EST_CONSTRUYENDO};
const string estados_extensiones[] = {"", "_move"};

const char CANT_ACCIONES = 4;
enum Accion_t {ACT_NONE, ACT_ATACK, ACT_COLLECT, ACT_BUILD};

const Estados_t accionAEstado[] = {EST_QUIETO, EST_ATACANDO, EST_RECOLECTANDO, EST_CONSTRUYENDO};

const string nombre_entidad_def = "unknown";

// Otras constantes
const string IMG_EXT = ".png";
const float LARGO_TILE = 30.2696;

// Duracion de 1 frame en ms
const float FRAME_DURATION = 59;

// Constante de velocidad
const float VEL_CONST = 0.2;


// Definición del struct que contendrá información particular de cada clase.
struct tipoEntidad_t {
	int tamX;
	int tamY;
	entity_type_t tipo;

	int rangoV;	// Rango vision
	int rangoA;	// Rango ataque
	// Asumo recoleccion y construccion limitadas a casillas adyacentes

	bool habilidades[CANT_ACCIONES];

	// Para recursos
	int recursoMax;
	resource_type_t tipoR;

	// Para unidades
	int velocidad;
	int ataque, defensa, vidaMax;
	int cooldown;	// Cooldown en ticks entre una interaccion y la siguiente

	// Para recolectores
	int collectRate;

	// Para constructores
	int buildRate;

	// Para edificios
	std::list<std::string> entrenables;

	// Valores por defecto
	tipoEntidad_t() {
		tamX = 1;
		tamY = 1;
		rangoV = 1;
		rangoA = 1;
		velocidad = 1;
		tipo = ENT_T_NONE;
		recursoMax = 0;
		tipoR = RES_T_NONE;

		ataque = 0;
		defensa = 0;
		vidaMax = 1;
		cooldown = 20;
		entrenables = std::list<std::string>();
		
		collectRate = 0;
		buildRate = 0;
		
		for (int i = 0; i < CANT_ACCIONES; i++) {
			habilidades[i] = false;		
		}
		habilidades[ACT_NONE] = true;
	}
};

#endif