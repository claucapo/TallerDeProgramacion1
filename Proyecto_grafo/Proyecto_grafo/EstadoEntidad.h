#include "Posicion.h"

class Entidad;
#pragma once


/* La clase de EstadoEntidad modela
las diversas acciones o consecuencias
que pueden acontecer sobre una Entidad.
Actualmente s�lo tenemos para hacer dos
estados ("quieto" y "moviendose"), pero
despu�s ayudar� a contemplar m�s cosas
(v.g. unidad "atacando" o "recolectando").
Esta clase es una base abstracta desde donde
derivar�n los Estados propiamente dichos.*/
class EstadoEntidad
{
public:
	// Constructor y destructor por defecto
	EstadoEntidad(void);
	~EstadoEntidad(void);

	// M�todo que recibe una Posici�n y una
	// Entidad y efectua una acci�n de avance
	// de frames (v.g. si la Entidad se est�
	// moviendo hacia un determinado destino,
	// puede modificar Posicion). Devuelve un
	// nuevo EstadoEntidad que corresponde a
	// la "pr�xima acci�n" a desarrollar (v.g.
	// si la Entidad recibi� un ataque, el pr�x.
	// estado puede ser "muriendo").
	virtual EstadoEntidad avanzarEstado(Entidad entidad, Posicion posicion);
};

