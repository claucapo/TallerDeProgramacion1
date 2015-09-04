#include "Posicion.h"

class Entidad;
#pragma once


/* La clase de EstadoEntidad modela
las diversas acciones o consecuencias
que pueden acontecer sobre una Entidad.
Actualmente sólo tenemos para hacer dos
estados ("quieto" y "moviendose"), pero
después ayudará a contemplar más cosas
(v.g. unidad "atacando" o "recolectando").
Esta clase es una base abstracta desde donde
derivarán los Estados propiamente dichos.*/
class EstadoEntidad
{
public:
	// Constructor y destructor por defecto
	EstadoEntidad(void);
	~EstadoEntidad(void);

	// Método que recibe una Posición y una
	// Entidad y efectua una acción de avance
	// de frames (v.g. si la Entidad se está
	// moviendo hacia un determinado destino,
	// puede modificar Posicion). Devuelve un
	// nuevo EstadoEntidad que corresponde a
	// la "próxima acción" a desarrollar (v.g.
	// si la Entidad recibió un ataque, el próx.
	// estado puede ser "muriendo").
	virtual EstadoEntidad avanzarEstado(Entidad entidad, Posicion posicion);
};

