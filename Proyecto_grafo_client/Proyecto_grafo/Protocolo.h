#ifndef MENSAJES_H
#define MENSAJES_H

// Separar para protocolo de updates y events??? Nah
enum CodigoMensaje {
	// Mensajes de acciones de juego
	MSJ_QUIETO,
	MSJ_MOVER,
	MSJ_RECOLECTAR,
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
};

struct msg_entidad{
	unsigned int playerCode;
	unsigned int idEntidad;

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