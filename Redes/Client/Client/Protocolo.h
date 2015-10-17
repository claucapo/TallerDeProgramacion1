#ifndef MENSAJES_H
#define MENSAJES_H

enum CodigoMensaje {
	// Mensajes especiales de conectividad
	MSJ_LOGIN,
	MSJ_LOGOUT,
	// Mensajes de acciones de juego
	MSJ_QUIETO,
	MSJ_MOVER,
	MSJ_RECOLECTAR
};

struct msg_login {
	int playerCode;
};

struct msg_login_response {
	bool ok;
};

struct msg_map {
	int coordX;
	int coordY;
};

struct msg_event {
	int something;
};

struct msg_update {
	int something;
};


#endif // MESNAJES_H