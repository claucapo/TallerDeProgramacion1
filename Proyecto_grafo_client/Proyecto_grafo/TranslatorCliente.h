#include "Partida.h"
#include "Escenario.h"
#include "Protocolo.h"
#include "ErrorLog.h"

#pragma once
class TranslatorCliente
{
private:
	Partida* partida;
public:
	TranslatorCliente(void);
	~TranslatorCliente(void);
	TranslatorCliente(Partida *partida);
	msg_event* generarEvento(CodigoMensaje accion, int idEntidad, float extra1, float extra2);
	void traducirMensajeDeServidor(msg_update* msj);
};

