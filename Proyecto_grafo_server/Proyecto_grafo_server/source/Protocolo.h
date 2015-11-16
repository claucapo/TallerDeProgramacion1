#ifndef MENSAJES_H
#define MENSAJES_H

#include "Enumerados.h"
enum entity_type_t;
enum resource_type_t;

// Separar para protocolo de updates y events??? Nah
enum CodigoMensaje {
	MSJ_KEEP_ALIVE,
	// Mensajes de acciones de juego
	MSJ_STATE_CHANGE,
	MSJ_MOVER,
	MSJ_RECOLECTAR,
	MSJ_ATACAR,
	MSJ_CONSTRUIR,
	MSJ_RECURSO_JUGADOR,
	MSJ_RES_CHANGE,
	MSJ_VIDA_CHANGE,
	MSJ_ASIGNAR_JUGADOR,
	MSJ_PRODUCIR_UNIDAD,
	MSJ_AVANZAR_PRODUCCION,
	MSJ_NUEVO_EDIFICIO,
	MSJ_FINALIZAR_EDIFICIO,
	MSJ_ELIMINAR,
	MSJ_JUGADOR_LOGIN,
	MSJ_JUGADOR_LOGOUT,


	MSJ_SPAWN = 100
};
enum Estados_t;

struct msg_login {
	int playerCode;
	char nombre[50];
};

struct msg_login_response {
	bool ok;
};

struct msg_map {
	int coordX;
	int coordY;
	int cantTipos;
	int cantInstancias;
	int cantJugadores;
};


// struct que define los recursos
struct msg_recursos {
	unsigned int oro;
	unsigned int madera;
	unsigned int comida;
	unsigned int piedra;
};

// La vision del jugador se ha de enviar por separado
// Los recursos se pasan por un struc estático dentro de este propio struct
struct msg_jugador {
	char name[50];
	char color[50];
	unsigned int id;
	msg_recursos recursos;
	bool conectado;
};


struct msg_tipo_entidad {
	unsigned int typeID;
	char name[50];
	int tamX, tamY;
	entity_type_t tipo;

	costo_t costo;

	int rangoV, rangoA;
	bool habilidades[CANT_ACCIONES];

	int recursoMax;
	resource_type_t tipoR;

	int velocidad, ataque, defensa, vidaMax;
	int cooldown, collectRate, buildRate, trainRate;

	unsigned int cant_entrenables;
	char* entrenables;
};

struct msg_instancia {
	unsigned int idEntidad;
	unsigned int playerCode;

	char name[50];

	Estados_t estadoEntidad;

	float coordX;
	float coordY;
};

struct msg_event {
	CodigoMensaje accion;
	unsigned int idEntidad;
	float extra1;
	float extra2;
};

struct msg_update {
	CodigoMensaje accion;
	unsigned int idEntidad;
	float extra1;
	float extra2;
};


#endif // MESNAJES_H



/*	
	PROTOCOLO DE COMUNICACION CLIENTE-SERVIDOR
---------------------------------------------------------------------

	Mensaje del cliente:						Mensaje del server:
	
	# Mensaje enviado del servidor al cliente para indicar un cambio en el estado de la unidad
	-										MSJ_STATE_CHANGE
	-												idEntidad: la Unidad a mover
	-												extra1: CoordX de la Unidad en el sgte frame
	-												extra2: CoordY de la Unidad en el sgte frame

	# Mensaje enviado del cliente al servidor para mover una Unidad a una Posicion
	# Mensaje enviado del servidor al cliente para indicar un cambio en la ubicacion de una unidad
	MSJ_MOVER									MSJ_MOVER
		idEntidad: la Unidad a mover				idEntidad: la Unidad a mover
		extra1: CoordX del destino					extra1: CoordX de la Unidad en el sgte frame
		extra2: CoordY del destino					extra2: CoordY de la Unidad en el sgte frame
	
	# Mensaje enviado del cliente al servidor para ordenar la recoleccion de un recurso
	MSJ_RECOLECTAR
		idEntidad: la Unidad que va a recolectar
		extra1: ID del recurso
		extra2: XXX
	
	# Mensaje enviado del cliente al servidor para ordenar la recoleccion de un recurso
	MSJ_ATACAR
		idEntidad: la Unidad que va a atacar
		extra1: ID del objetivo
		extra2: XXX
			
	# Mensaje enviado del cliente al servidor para ordenar la construccion de un edificio (ya colocado en el mapa)
	MSJ_CONSTRUIR
		idEntidad: la Unidad que va a construir
		extra1: CoordX del objetivo
		extra2: CoordY del objetivo

	# Mensaje enviado del servidor al cliente cuando se modifican los recursos de un jugador
	-											MSJ_RECURSO_JUGADOR
	-												idEntidad: la ID del jugador
	-												extra1: valor actual del recurso (cliente tiene que sobreescribir)
	-												extra2: resource_type_t del recurso
	
	# Mensaje enviado del servidor al cliente cuando se modifican los recursos de una entidad
	-											MSJ_RES_CHANGE
	-												idEntidad: la ID del recurso
	-												extra1: variacion en el campo recursoAct
	-												extra2: XXX

	# Mensaje enviado del servidor al cliente cuando se modifican la vida de una entidad
	-											MSJ_VIDA_CHANGE
	-												idEntidad: la ID del recurso
	-												extra1: variacion en el campo vidaAct
	-												extra2: XXX

	# Mensaje enviado del servidor al cliente cuando se modifican el dueño de una unidad
	-											MSJ_ASIGNAR_JUGADOR
	-												idEntidad: la ID de la entidad
	-												extra1:  ID del nuevo dueño
	-												extra2: XXX

	# Mensaje enviado del cliente al servidor para ordenar la produccion de una unidad
	# Mensaje enviado del servidor al cliente para indicar que un edificio comienza la produccion
	MSJ_PRODUCIR_UNIDAD							MSJ_PRODUCIR_UNIDAD
		idEntidad: ID del edificio					idEntidad: la ID del edificio
		extra1: typeID de la unidad a producir		extra1: typeID de la unidad a producir
		extra2: XXX									extra2: cantidad de ticks para producir
		
	# Mensaje enviado del servidor al cliente para indicar un avance en la produccion
	-											MSJ_AVANZAR_PRODUCCION
	-												idEntidad: la ID del edificio
	-												extra1: variacion en los ticks restantes
	-												extra2: XXX
	

	# Mensaje enviado del cliente al servidor para ordenar la construccion de un nuevo edificio
	# Se responderá con un MSJ_SPAWN
	MSJ_NUEVO_EDIFICIO							
		idEntidad: typeID del edificio				
		extra1: CoordX de la ubicacion	
		extra2: CoordY de la ubicacion			
						
	# Mensaje enviado del servidor al cliente cuando se finaliza la construccion de un edificio
	-											MSJ_FINALIZAR_EDIFICIO
	-												idEntidad: la ID del edificio
	-												extra1: XXX
	-												extra2: XXX
						
	# Mensaje enviado del servidor al cliente cuando debe borrar una entidad
	-											MSJ_ELIMINAR
	-												idEntidad: la ID de la entidad
	-												extra1: XXX
	-												extra2: XXX

	# Mensaje enviado del servidor al cliente cuando se conecta un jugador
	-											MSJ_JUGADOR_LOGIN
	-												idEntidad: la ID del jugador
	-												extra1: XXX
	-												extra2: XXX

	# Mensaje enviado del servidor al cliente cuando se desconecta un jugador
	-											MSJ_JUGADOR_LOGOUT
	-												idEntidad: la ID del jugador
	-												extra1: XXX
	-												extra2: XXX

	# Mensaje enviado del servidor al cliente cuando hay que generar una nueva entidad
	-											MSJ_SPAWN
	-												idEntidad: la ID de la entidad nueva
	-												extra1: CoordX del punto de spawn
	-												extra2: CoordY del punto de spawn

	##### NOTA:  MSJ_SPAWN = 100, y el typeID de la nueva entidad esta codificada dentro del mismo código
	#####		 del mensaje mediante un offset: así un CodigoMensaje = 101 implica crear una unidad con
	#####		 typeID = 1; un CodigoMensaje = 112 implica crear una con typeID = 12, etc...
	##### NOTA2: Si el typeID se corresponde a una entidad de tipo ENT_T_BUILDING; deberá setearle el modo
	#####		 en construccion (llamando a setEnConstruccion(true);.
	##### NOTA3: Se reservan para esta serie de mensajes un espacio de 100 CodigoMensaje, es decir, que un
	#####		 mensaje con codigo >= 200 (o < 100) no entra en esta categoria.
	
*/