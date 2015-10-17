#include "TranslatorServer.h"
#include "Partida.h"
#include "Escenario.h"
#include "Protocolo.h"
#include "ErrorLog.h"

TranslatorServer::TranslatorServer(void){
	this->partida = nullptr;
}

TranslatorServer::TranslatorServer(Partida *partida){
	this->partida = partida;
}


TranslatorServer::~TranslatorServer(void)
{
}



msg_update* TranslatorServer::generarUpdate(CodigoMensaje accion, int idEntidad, float extra1, float extra2){
	msg_update* mensaje = new msg_update;
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

// Este es el método encargado de tratar un mensaje recibido
// del cliente y devolver la respuesta asociada
msg_update* TranslatorServer::traducirMensajeDeCliente(msg_event* msj){
	Escenario* scene = this->partida->escenario;
	CodigoMensaje accion = msj->accion;
	// Según la acción que sea, hago lo que corresponda
	switch(accion){
	case MSJ_MOVER:
		// Cambiar por algo que detecte la id!
		Unidad* uni = scene->verProtagonista();
		Posicion destino = Posicion(msj->extra1, msj->extra2);
		scene->asignarDestinoProtagonista(&destino);
		uni->avanzarFrame(scene);
		// Genero respuesta:
		return this->generarUpdate(MSJ_MOVER, msj->idEntidad, uni->verPosicion()->getX(), uni->verPosicion()->getX());
	}
	return NULL;
}
