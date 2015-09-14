#ifndef ENUM_H
#define ENUM_H

using namespace std;

// Enumerado de posible direcciones de movimiento, escritos en un orden arbitrario para
// que concidan con las filas de los sprites de movimiento
enum Direcciones_t {DIR_RIGHT, DIR_TOP_RIGHT, DIR_TOP, DIR_TOP_LEFT, DIR_LEFT, DIR_DOWN_LEFT, DIR_DOWN, DIR_DOWN_RIGHT};

// Estados
enum Estados_t {EST_QUIETO, EST_CAMINANDO};
const string estados_extensiones[] = {"", "_move"};

// Otras constantes
const string IMG_EXT = ".png";
const float LARGO_TILE = 30.2696;

// Duracion de 1 frame en ms
const float FRAME_DURATION = 50;

// Constante de velocidad
const float VEL_CONST = 0.2;

#endif