#ifndef ENUM_H
#define ENUM_H

#include <list>
using namespace std;

enum tipo_derrota_t {LOSE_ALL, LOSE_UNITS, TRANSFER_ALL};
enum tipo_partida_t {PARTIDA_SUPREMACIA, PARTIDA_CAPTURE_FLAG, PARTIDA_REGICIDA};

const int cant_tipos_terreno = 2;
enum terrain_type_t {TERRAIN_GRASS, TERRAIN_WATER};

// Enumerado de posible direcciones de movimiento, escritos en un orden arbitrario para
// que concidan con las filas de los sprites de movimiento
enum Direcciones_t {DIR_RIGHT, DIR_TOP_RIGHT, DIR_TOP, DIR_TOP_LEFT, DIR_LEFT, DIR_DOWN_LEFT, DIR_DOWN, DIR_DOWN_RIGHT};

// Tipos de entidades y recursos
enum entity_type_t {ENT_T_NONE, ENT_T_UNIT, ENT_T_RESOURCE, ENT_T_BUILDING, ENT_T_CONSTRUCTION};
enum resource_type_t {RES_T_NONE = -1, RES_T_GOLD = 0, RES_T_WOOD = 1, RES_T_FOOD = 2, RES_T_STONE = 3};

// Resultados de un avanzarFrame
enum af_result_t {AF_NONE, AF_STATE_CHANGE, AF_MOVE, AF_KILL, AF_SPAWN};

// Estados
enum Estados_t {EST_QUIETO, EST_CAMINANDO, EST_ATACANDO, EST_RECOLECTANDO, EST_CONSTRUYENDO, EST_MUERTO = 100};
const string estados_extensiones[] = {"", "_move"};

const char MAX_ENTRENABLES = 4;	// Cantidad m�xima de tipos de entidades distintas que un dado edificio puede entrenar
const char MAX_PRODUCCION = 5;	// Cantidad m�xima de unidades que pueden encolarse para la producci�n en un edificio

// Definici�n de acciones, y los estados que corresponde a cada uno (en caso de haberlo)
const char CANT_ACCIONES = 14;
enum Accion_t {ACT_NONE, ACT_ATACK, ACT_COLLECT, ACT_BUILD, ACT_INFANTRY, ACT_CAVALRY, ACT_ARCHERY, ACT_SIEGE, ACT_BONUS_INFANTRY, ACT_BONUS_CAVALRY, ACT_BONUS_ARCHERY, ACT_BONUS_SIEGE, ACT_BONUS_BUILDING, ACT_INVISIBLE};
const Estados_t accionAEstado[] = {EST_QUIETO, EST_ATACANDO, EST_RECOLECTANDO, EST_CONSTRUYENDO};

const unsigned int NAME_LENGH_DEF = 50;
const string nombre_entidad_def = "unknown";

// Otras constantes
const string IMG_EXT = ".png";
const float LARGO_TILE = 30.2696;

// Duracion de 1 frame en ms
const float FRAME_DURATION = 59;

// Constante de velocidad
const float VEL_CONST = 0.2;


// Definicion del struct que contiene informacion del costo
struct costo_t {
	int oro;
	int madera;
	int comida;
	int piedra;

	costo_t() {
		oro = 0;
		madera = 0;
		comida = 0;
		piedra = 0;
	}
};


// Definici�n del struct que contendr� informaci�n particular de cada clase.
struct tipoEntidad_t {
	unsigned int typeID;
	int tamX;
	int tamY;
	entity_type_t tipo;

	costo_t costo;

	int rangoV;	// Rango vision
	int rangoA;	// Rango ataque
	// Asumo recoleccion y construccion limitadas a casillas adyacentes

	bool habilidades[CANT_ACCIONES];

	// Para recursos
	int recursoMax;
	resource_type_t tipoR;

	// Para unidades
	int luck;
	int velocidad;
	int ataque, defensa, vidaMax;
	int cooldown;	// Cooldown en ticks entre una interaccion y la siguiente

	// Para recolectores
	int collectRate;

	// Para constructores
	int buildRate;

	// Para edificios
	int trainRate;
	int cantidad_entrenables;
	terrain_type_t validTerrain;
	std::string entrenables[MAX_ENTRENABLES];
	
//	terrain_type_t validTerrain;

	// Valores por defecto
	tipoEntidad_t() {
		tamX = 1;
		tamY = 1;
		rangoV = 1;
		rangoA = 1;
		velocidad = 1;
		luck = 50;
		tipo = ENT_T_NONE;
		recursoMax = 0;
		tipoR = RES_T_NONE;

		costo = costo_t();

		ataque = 0;
		defensa = 0;
		vidaMax = 1;
		cooldown = 20;

		trainRate = 0;
		validTerrain = TERRAIN_GRASS;

		// Inicializo el arreglo de entrenables en "unknown"
		cantidad_entrenables = 0;
		for (int i = 0; i < MAX_ENTRENABLES; i++) {
			entrenables[i] = nombre_entidad_def;
		}

		collectRate = 0;
		buildRate = 0;
		
		for (int i = 0; i < CANT_ACCIONES; i++) {
			habilidades[i] = false;		
		}
		habilidades[ACT_NONE] = true;
	}
};


struct datosPantInic {
	string puerto;
	int cantPlayers;
	int tipoPartida;
};


#endif