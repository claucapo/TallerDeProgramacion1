#include "Partida.h"
#include "Escenario.h"
#include "Protocolo.h"
#include "ErrorLog.h"

#pragma once
class TranslatorServer
{
private:
	Partida* partida;

public:
	TranslatorServer(void);
	~TranslatorServer(void);
	TranslatorServer(Partida *partida);
	msg_update* generarUpdate(CodigoMensaje accion, int idEntidad, float extra1, float extra2);
	msg_update* traducirMensajeDeCliente(msg_event* msj);
};

