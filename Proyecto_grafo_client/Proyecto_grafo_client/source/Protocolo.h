#ifndef MENSAJES_H
#define MENSAJES_H

enum entity_type_t;
enum resource_type_t;

// Separar para protocolo de updates y events??? Nah
enum CodigoMensaje {
	MSJ_KEEP_ALIVE,
	// Mensajes de acciones de juego
	MSJ_QUIETO,
	MSJ_MOVER,
	MSJ_RECOLECTAR,
	MSJ_RECURSO_JUGADOR,
	MSJ_ELIMINAR,
	MSJ_JUGADOR_LOGIN,
	MSJ_JUGADOR_LOGOUT
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
	char name[50];
	int tamX;
	int tamY;
	int vision;
	int velocidad;
	entity_type_t tipo;
	int score;
	resource_type_t tipoR;
};


struct msg_instancia {
	unsigned int idEntidad;
	unsigned int playerCode;

	char name[50];

	Estados_t estadoEntidad;

	float coordX;
	float coordY;
};

/*	
	PROTOCOLO DE COMUNICACION CLIENTE-SERVIDOR
---------------------------------------------------------------------

	Mensaje del cliente:						Respuesta del server:
	
	# Mensaje enviado del cliente al servidor para mover una Unidad a una Posicion
	MSJ_MOVER									MSJ_MOVER
		idEntidad: la Unidad a mover				idEntidad: la Unidad a mover
		extra1: CoordX del destino					extra1: CoordX de la Unidad en el sgte frame
		extra2: CoordY del destino					extra2: CoordY de la Unidad en el sgte frame
	
	# Mensaje enviado del servidor al cliente cuando una Unidad queda quieta
	-											MSJ_QUIETO
	-												idEntidad: la Unidad que se queda quieta
	-												extra1: XXX
	-												extra2: XXX
	
	# Mensaje enviado del servidor al cliente cuando una Unidad recolecta un recurso
	-											MSJ_RECOLECTAR
	-												idEntidad: la Unidad que recolecta
	-												extra1: idEntidad del recurso
	-												extra2: XXX
	
		
*/
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