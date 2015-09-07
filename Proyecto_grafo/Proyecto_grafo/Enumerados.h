#ifndef ENUM_H
#define ENUM_H

// Enumerado de posible direcciones de movimiento, escritos en un orden arbitrario para
// que concidan con las filas de los sprites de movimiento
enum Direcciones_t {DIR_RIGHT, DIR_TOP_RIGHT, DIR_TOP, DIR_TOP_LEFT, DIR_LEFT, DIR_DOWN_LEFT, DIR_DOWN, DIR_DOWN_RIGHT};

// Estados
enum Estados_t {EST_QUIETO, EST_CAMINANDO};

const string estados_extensiones[] = {"", "_move"};

// Otras constantes
const string IMG_EXT = ".png";

#endif