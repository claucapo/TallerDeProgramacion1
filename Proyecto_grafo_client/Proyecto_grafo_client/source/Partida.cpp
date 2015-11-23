#include "Partida.h"
#include "Escenario.h"
#include "Jugador.h"
#include "Enumerados.h"
#include "ErrorLog.h"
#include "Recurso.h"
#include "Unidad.h"
#include "FactoryEntidades.h"
#include "BibliotecaDeImagenes.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

#include <sstream> // Para convertir int en string

Partida::Partida(void) {
	this->jugadores = list<Jugador*>();
	this->seleccionados = list<Posicion*>();
	this->escenario = nullptr;
	this->ent_seleccionadas = list<Entidad*>();
	Jugador* gaia = new Jugador("gaia", 0, "green");
	gaia->settearConexion(true);
	this->jugadores.push_front(gaia);
	sx = 0; sy = 0; sx2 = 0; sy2 = 0;
	algoSeleccionado = false;
	this->seleccionSecundaria = nullptr;
	this->modoUbicarEdificio = false;
}

Partida::~Partida(void) {
	list<Jugador*>::const_iterator iter = this->jugadores.begin();
	while(iter != this->jugadores.end()) {
		delete (*iter);
		iter++;
	}
	
	if (this->escenario);
		delete this->escenario;
}

bool Partida::agregarJugador(Jugador* jugador) {
	for( list<Jugador*>::const_iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		if ((*iter)->verNombre() == jugador->verNombre()) {
			ErrorLog::getInstance()->escribirLog("El jugador [" + jugador->verNombre() + "] ya existe en la partida.", LOG_ERROR);
			return false;
		} else if ((*iter)->verID() == jugador->verID()) {
			std::stringstream s;
			s << jugador->verID();
			ErrorLog::getInstance()->escribirLog("El con ID [" + s.str() + "] ya existe en la partida.", LOG_ERROR);
			return false;
		}
	}
	this->jugadores.push_front(jugador);
	return true;
}


void Partida::asignarEscenario(Escenario* escenario) {
	if (this->escenario)
		delete this->escenario;
	this->escenario = escenario;
}

Jugador* Partida::obtenerJugador(int id) {
	for( list<Jugador*>::const_iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		if ((*iter)->verID() == id) {
			return *iter;
		}
	}
	return nullptr;
}

void Partida::avanzarFrame(unsigned int actPlayer){
	for (list<Jugador*>::iterator iter = this->jugadores.begin(); iter != this->jugadores.end(); ++iter) {
		Jugador* act = (*iter);
		if (act->verID() == (*iter)->verID())
			act->reiniciarVision();
	}
	// Acá adentro se asignan las casillas vistas de cada jugador
	this->hayViolencia = this->escenario->avanzarFrame(actPlayer);
}


void Partida::procesarUpdate(msg_update msj, unsigned int actPlayer) {
	Escenario* scene = this->escenario;
	CodigoMensaje accion = msj.accion;
	// Según la acción que sea, hago lo que corresponda
	Posicion destino;
	Jugador* owner;
	Jugador* newOwner = nullptr;
	Entidad* ent = nullptr;
	Estados_t nState;
	Mix_Chunk* snd = NULL;
	switch (accion) {
		case MSJ_ELIMINAR:
			ent = escenario->obtenerEntidad(msj.idEntidad);
			ent->settearEstado(EST_MUERTO);
 
			snd = BibliotecaDeImagenes::obtenerInstancia()->devolverSonido(ent->verNombre() + "_die");
			Mix_PlayChannel( -1, snd, 0 );
			this->escenario->quitarEntidad(msj.idEntidad);
			break;

		case MSJ_STATE_CHANGE:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			nState = ent->verEstado();
			switch ((int)msj.extra1) {
			case 0: 
				nState = EST_QUIETO; 
				if(ent->verEstado() != EST_QUIETO){
					Spritesheet* spEnt = (ent)->verSpritesheet();
					string nombreEnt = (ent)->verNombre();
					if((ent)->verJugador()->verID() == 2)
						nombreEnt = nombreEnt + '2';
					if((ent)->verJugador()->verID() == 3)
						nombreEnt = nombreEnt + '3';
					spEnt->cambiarImagen(nombreEnt);
				}	
				break;

			case 1: nState = EST_CAMINANDO; break;
			case 2: 
				nState = EST_ATACANDO; 
				if(ent->verEstado() != EST_ATACANDO){
					Spritesheet* spEnt = (ent)->verSpritesheet();
					string nombreEnt = (ent)->verNombre() + "_atk";
					if((ent)->verJugador()->verID() == 2)
						nombreEnt = nombreEnt + '2';
					if((ent)->verJugador()->verID() == 3)
						nombreEnt = nombreEnt + '3';
					spEnt->cambiarImagen(nombreEnt);
					ent->targetID = (int) msj.extra2;

					Posicion* act = ent->verPosicion();
					Posicion* tPos = scene->obtenerEntidad(ent->targetID)->verPosicion();
					float distX = tPos->getX() - act->getX() ;
					float distY = tPos->getY() - act->getY() ;
					Direcciones_t dir = ((Unidad*)ent)->calcularDirecion(distX, distY);
					((Unidad*)ent)->setDireccion(dir);
			
					ent->verSpritesheet()->cambiarSubImagen(0, dir);
				}
				break;
			case 3: 
				nState = EST_RECOLECTANDO; 
				if(ent->verEstado() != EST_RECOLECTANDO){
					string nombreEnt;
					Spritesheet* spEnt = (ent)->verSpritesheet();
					Recurso* rec =(Recurso*) this->escenario->obtenerEntidad((int) msj.extra2);
					if(rec->tipoR == RES_T_FOOD)
						nombreEnt = (ent)->verNombre() + "_collect";
					else if(rec->tipoR == RES_T_WOOD)
						nombreEnt = (ent)->verNombre() + "_chop";
					else
						nombreEnt = (ent)->verNombre() + "_mine";
				
					if((ent)->verJugador()->verID() == 2)
						nombreEnt = nombreEnt + '2';
					if((ent)->verJugador()->verID() == 3)
						nombreEnt = nombreEnt + '3';
					spEnt->cambiarImagen(nombreEnt);
					ent->targetID = (int) msj.extra2;

					Posicion* act = ent->verPosicion();
					Posicion* tPos = scene->obtenerEntidad(ent->targetID)->verPosicion();
					float distX = tPos->getX() - act->getX() ;
					float distY = tPos->getY() - act->getY() ;
					Direcciones_t dir = ((Unidad*)ent)->calcularDirecion(distX, distY);
					((Unidad*)ent)->setDireccion(dir);
		
					ent->verSpritesheet()->cambiarSubImagen(0, dir);
				}
				break;
			case 4: 
				nState = EST_CONSTRUYENDO; 
				if(ent->verEstado() != EST_CONSTRUYENDO){
					string nombreEnt;
					Spritesheet* spEnt = (ent)->verSpritesheet();
					nombreEnt = (ent)->verNombre() + "_build";
				
					if((ent)->verJugador()->verID() == 2)
						nombreEnt = nombreEnt + '2';
					if((ent)->verJugador()->verID() == 3)
						nombreEnt = nombreEnt + '3';
					spEnt->cambiarImagen(nombreEnt);
					ent->targetID = (int) msj.extra2;

					Posicion* act = ent->verPosicion();
					Posicion* tPos = scene->obtenerEntidad(ent->targetID)->verPosicion();
					float distX = tPos->getX() - act->getX() ;
					float distY = tPos->getY() - act->getY() ;
					Direcciones_t dir = ((Unidad*)ent)->calcularDirecion(distX, distY);
					((Unidad*)ent)->setDireccion(dir);
		
					ent->verSpritesheet()->cambiarSubImagen(0, dir);
				}				
				break;
			}
			ent->settearEstado(nState);



		
		//	if(!this->ent_seleccionadas.empty())
		//		this->seleccionarEntidad(this->ent_seleccionadas.front(), false);
			break;

		case MSJ_MOVER:
			destino = Posicion(msj.extra1, msj.extra2);
			scene->moverEntidad(msj.idEntidad, &destino, true);
			if(this->ent_seleccionadas.size() == 1)
				this->seleccionarEntidad(this->ent_seleccionadas.front(), true);
			break;

		case MSJ_RES_CHANGE:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			if (ent && ent->tipo == ENT_T_RESOURCE) {
				Recurso* res = (Recurso*)ent;
				res->recursoAct += msj.extra1;
			}
			break;

		case MSJ_VIDA_CHANGE:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			if (ent) {
				ent->vidaAct += msj.extra1;
				if (ent->vidaAct <= 0){
					ent->vidaAct = 0;			
					}
				if (ent->vidaAct >= ent->vidaMax)
					ent->vidaAct = ent->vidaMax;
			if(msj.extra1 < 0){
				if(ent->verJugador()->verID() == actPlayer){ // Si era un ataque...
					if(!this->hayViolencia){
						cout << "Me atacaron!" << endl;
						snd = BibliotecaDeImagenes::obtenerInstancia()->devolverSonido("underAttack");
						Mix_PlayChannel( -1, snd, 0 );
						}
					}
				this->hayViolencia = true;
				}
			}
			break;
			
		case MSJ_ASIGNAR_JUGADOR:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			owner = this->obtenerJugador((int)msj.extra1);
			if (owner) {
				ent->asignarJugador(owner);
			}
			break;

		case MSJ_AVANZAR_PRODUCCION:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			if (ent->tipo == ENT_T_BUILDING) {
				((Edificio*)ent)->ticks_restantes += msj.extra1;
				if (((Edificio*)ent)->ticks_restantes <= 0)
					((Edificio*)ent)->ticks_restantes = 0;
			}
			break;

		case MSJ_PRODUCIR_UNIDAD:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			if (ent->tipo == ENT_T_BUILDING) {
				Edificio* ed = (Edificio*)ent;
				ed->entrenarUnidad(msj.extra1, msj.extra2);
			}
			break;

		case MSJ_FINALIZAR_PRODUCCION:	
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			if (ent->tipo == ENT_T_BUILDING) {
				Edificio* ed = (Edificio*)ent;
				ed->finalizarEntrenamiento();
			}

			break;

		case MSJ_FINALIZAR_EDIFICIO:
			ent = this->escenario->obtenerEntidad(msj.idEntidad);
			if (ent->tipo == ENT_T_CONSTRUCTION){
				((Edificio*)ent)->setEnConstruccion(false);
				string newImg = ent->verNombre();
				if(ent->verJugador()->verID() == 2)
					newImg = newImg + '2';
				if(ent->verJugador()->verID() == 3)
					newImg = newImg + '3';
				ent->verSpritesheet()->cambiarImagen(newImg);
				ent->vidaAct = ent->vidaMax;
				}
			break;


		case MSJ_JUGADOR_DERROTADO:
			for (list<Jugador*>::const_iterator iter = jugadores.begin(); iter != jugadores.end(); iter++) {
				if ((*iter)->verID() == (unsigned int)msj.extra2) {
					newOwner = (*iter);
				}
			}

			this->escenario->derrotarJugador(msj.idEntidad, tipo_derrota_t((int)msj.extra1), newOwner);
			break;

		default:
			if (accion >= MSJ_SPAWN) {
				ent = FactoryEntidades::obtenerInstancia()->obtenerEntidad(accion - MSJ_SPAWN, msj.idEntidad);
				if (ent->tipo == ENT_T_BUILDING) {
					((Edificio*)ent)->setEnConstruccion(true);
				}
				destino = Posicion((float)msj.extra1, (float)msj.extra2);
				if (!scene->ubicarEntidad(ent, &destino)) {
					delete ent;
				} else {
					owner = this->obtenerJugador(0);
					ent->asignarJugador(owner);
					string nombreEnt = ent->verNombre();

					if(ent->verTipo() == ENT_T_CONSTRUCTION){
					nombreEnt = "building";
					switch(ent->verTamX()){
						case 2: nombreEnt += "2x2"; break;
						case 3: nombreEnt += "3x3"; break;
						case 4: nombreEnt += "4x4"; break;
						case 5: nombreEnt += "5x5"; break;
						case 6: nombreEnt += "6x6"; break;
						}
					}

					if(owner->verID() == 2)
						nombreEnt = nombreEnt + '2';
					if(owner->verID() == 3)
						nombreEnt = nombreEnt + '3';

					Spritesheet* cas = new Spritesheet(nombreEnt);
					ent->asignarSprite(cas);
					
					if(ent->verTipo() == ENT_T_UNIT){
						if(ent->verNombre() == "villager")
							snd = BibliotecaDeImagenes::obtenerInstancia()->devolverSonido("villager_create");
						else
							snd = BibliotecaDeImagenes::obtenerInstancia()->devolverSonido("ent_create");
						Mix_PlayChannel( -1, snd, 0 );
						}
			
				}
			}

	}
}

void Partida::seleccionarEntidad(Entidad* ent, bool emptyFirst){
	if(emptyFirst)
		this->deseleccionarEntidades();
	int coordX = ent->verPosicion()->getRoundX();
	int coordY = ent->verPosicion()->getRoundY();
	for(int i = 0; i < ent->verTamX(); i++) {
		for(int j = 0; j < ent->verTamY(); j++){
			Posicion* pos= new Posicion(coordX + i, coordY + j);
			
			this->seleccionados.push_front(pos);
		}
	}
	// cout<< "seleccionado: " << ent->verNombre() << endl;
	this->ent_seleccionadas.push_back(ent);
}

void Partida::deseleccionarEntidades(void){
	while(!this->seleccionados.empty()){
		Posicion* pAct = this->seleccionados.front();
		this->seleccionados.pop_front();
		delete pAct;
		}
	while(!this->ent_seleccionadas.empty())
		this->ent_seleccionadas.pop_front();
}


list<Posicion*> Partida::verSeleccionados(void){
	return this->seleccionados;
}

Entidad* Partida::verEntidadSeleccionada(void) {
	return ent_seleccionadas.front();
}

list<Entidad*> Partida::verListaEntidadesSeleccionadas(void){
	return this->ent_seleccionadas;
}

bool Partida::edificioUbicablePuedeConstruirse(Posicion pos){
	if(!this->modoUbicarEdificio)
		return false;

	if(!this->escenario->verMapa()->posicionPertenece(&pos))
		return false;

	Entidad* edif = FactoryEntidades::obtenerInstancia()->obtenerEntidad(this->edificioAubicar, 65500);

	for(int i = 0; i < edif->verTamX(); i++){
		for(int j = 0; j < edif->verTamY(); j++){
				Posicion pAct(pos.getRoundX() + i, pos.getRoundY() + j);
				if(!this->escenario->verMapa()->posicionPertenece(&pAct))
					return false;
				if(!this->escenario->casillaEstaVacia(&pAct)){
	//				delete edif;
					return false;
					}
			}
		}

//	delete edif;
	return true;

}