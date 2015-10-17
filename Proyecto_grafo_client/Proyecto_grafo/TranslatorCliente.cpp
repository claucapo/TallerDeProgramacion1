#include "TranslatorCliente.h"
#include "Partida.h"
#include "Escenario.h"
#include "Protocolo.h"
#include "ErrorLog.h"


TranslatorCliente::TranslatorCliente(void){
	this->partida = nullptr;
}

TranslatorCliente::TranslatorCliente(Partida *partida){
	this->partida = partida;
}

TranslatorCliente::~TranslatorCliente(void)
{
}


msg_event* TranslatorCliente::generarEvento(CodigoMensaje accion, int idEntidad, float extra1, float extra2){
	msg_event* mensaje = new msg_event;
	if(!mensaje){
		ErrorLog::getInstance()->escribirLog("Error fatal: no hay memoria.");
		return NULL;
	}
	mensaje->accion = accion;
	mensaje->idEntidad = idEntidad;
	mensaje->extra1 = extra1;
	mensaje->extra2 = extra2;
	return mensaje;
}

// Este es el método encargado de tratar un update recibido
// del servidor y ejecutar lo que corresponda
void TranslatorCliente::traducirMensajeDeServidor(msg_update* msj){
		Escenario* scene = this->partida->escenario;
	CodigoMensaje accion = msj->accion;
	// Según la acción que sea, hago lo que corresponda
	switch(accion){
	case MSJ_MOVER:
		// Cambiar por algo que detecte la id!
		Unidad* uni = scene->verProtagonista();
		Posicion nuevaPos = Posicion(msj->extra1, msj->extra2);
		uni->nuevoDestino(&nuevaPos);
		return;
	}
}