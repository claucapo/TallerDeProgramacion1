#include "Partida.h"
#include "Entidad.h"
#include "Unidad.h"
#include "Cliente.h"
#include "GraficadorPantalla.h"
#include "Escenario.h"
#include "Posicion.h"
#include "ProcesadorEventos.h"
#include "FactoryEntidades.h"
#include "Enumerados.h"

#include<iostream>
#include <SDL.h>
#include <set>

#pragma once

using namespace std;

#define TOL_SEL_MULT 9 // Si solo dibujo para la seleccion un
// cuadradito de 9x9 no es seleccion multiple sino de una sola unidad




ProcesadorEventos::ProcesadorEventos(void){
}

ProcesadorEventos::~ProcesadorEventos(void){
}

ProcesadorEventos::ProcesadorEventos(Partida* partida, GraficadorPantalla* grafP){
	this->game = partida;
	this->gp = grafP;
}


Posicion ProcesadorEventos::adyacenteSiguiente(Posicion pos, int i, Escenario* scene){
	Posicion nueva;
	switch(i){
		case 0:
			nueva = Posicion(pos.getX() -1, pos.getY() +1);
			break;
		case 1:
			nueva = Posicion(pos.getX() -1, pos.getY());
			break;
		case 2:
			nueva = Posicion(pos.getX() -1, pos.getY() -1);
			break;
		case 3:
			nueva = Posicion(pos.getX(), pos.getY() -1);
			break;
		case 4:
			nueva = Posicion(pos.getX() +1, pos.getY() -1);
			break;
		case 5:
			nueva = Posicion(pos.getX() +1, pos.getY());
			break;
		case 6:
			nueva = Posicion(pos.getX() +1, pos.getY() +1);
			break;
		case 7:
			nueva = Posicion(pos.getX() , pos.getY() +1);
			break;
		}

	if(!scene->verMapa()->posicionPertenece(&nueva))
		return pos;
	else
		return nueva;
}

void ProcesadorEventos::procesarBoton(Jugador* player){
	if(game->verListaEntidadesSeleccionadas().empty())
		return;

	Entidad* entPri = game->verEntidadSeleccionada();

	if(!entPri->habilidades[ACT_BUILD])
		return;

	list<string> construibles = FactoryEntidades::obtenerInstancia()->verListaEdificios();
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	my -= 372;
	
	list<string>::iterator it = construibles.begin();
	int i = 0;

	while(mx >= BUTTON_SIZE){
		mx -= BUTTON_SIZE;
		i++;
		}
	while(my >= BUTTON_SIZE){
		my -= BUTTON_SIZE;
		i += 4;
		}
	if(i >= construibles.size())
		return;

	while(i){
		it++;
		i--;
	}

	cout << "Hay que construir: " << (*it) << endl;
}

void ProcesadorEventos::procesarSeleccionMultiple(Jugador* player){
	int dif1 = game->sx2-game->sx;
	int dif2 = game->sy2-game->sy;
	if((dif1>(-TOL_SEL_MULT))&&(dif1<TOL_SEL_MULT)&&(dif2<TOL_SEL_MULT)&&(dif2>(-TOL_SEL_MULT)))
		return;
	
	// Reacomodo las coordenadas del rectangulo
	int ax1 = game->sx, ax2 = game->sx2;
	int ay1 = game->sy, ay2 = game->sy2;

	if((game->sx2 < game->sx) &&(game->sy2 < game->sy)){
		ax2 = game->sx;
		ay2 = game->sy;
		ax1 = game->sx2;
		ay1 = game->sy2;
		}
	else if(game->sx2 < game->sx){
		ax2 = game->sx;
		ax1 = game->sx2;
		}
	else if(game->sy2 < game->sy){
		ay2 = game->sy;
		ay1 = game->sy2;
		}

	int i, j;
	game->deseleccionarEntidades();
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	set<Posicion, lex_compare> usados;

	// Bucle m�gico que itera dentro de cada casilla del rectangulo
	for(i = ax1; i < ax2; i+= 26)
		for(j = ay1; j < ay2; j+= 16){
			float pX = cu->obtenerCoordLogicaX(i, j, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
			float pY = cu->obtenerCoordLogicaY(i, j, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
			
			Posicion pAct = Posicion(pX, pY);
		//	cout << "PxPY = " << pX << "," << pY << endl;
			Posicion pAux = Posicion(pAct.getRoundX(), pAct.getRoundY());

			if(game->escenario->verMapa()->posicionPertenece(&pAct))
				if(player->visionCasilla(pAct) != VIS_NO_EXPLORADA)
					if(!game->escenario->verMapa()->posicionEstaVacia(&pAct))
						if(game->escenario->verMapa()->verContenido(&pAct)->verTipo() == ENT_T_UNIT)
							if(player->poseeEntidad(game->escenario->verMapa()->verContenido(&pAct)))
								if(usados.count(pAux) == 0){
								game->seleccionarEntidad(game->escenario->verMapa()->verContenido(&pAct), false);
								usados.insert(pAux);
								}
			}

}


void ProcesadorEventos::procesarMover(Cliente* client) {
	list<Entidad*> lasEnt = game->verListaEntidadesSeleccionadas();
	// Obtengo la posici�n del click (quiz�s convertir en una funci�n ya que se repite en varios lados...
	Posicion dest = *game->seleccionSecundaria;
	float pX = dest.getX();
	float pY = dest.getY();
	
	set<Posicion, lex_compare> destinos;
	set<Posicion, lex_compare> ocupadas;

	for (list<Entidad*>::iterator it=lasEnt.begin(); it != lasEnt.end(); ++it){
		msg_event newEvent;
		newEvent.idEntidad = (*it)->verID();
		newEvent.accion = MSJ_MOVER;
		int i = 0;
		dest = Posicion(pX, pY);
		// Carga en dest una posicion cercana y disponible
		// Revisar? Posible causa del lag misterioso?
		while(destinos.count(dest)){
			Posicion p = adyacenteSiguiente(dest, i, game->escenario);
			if(ocupadas.count(p))
				i++;
			else if(!game->escenario->verMapa()->posicionEstaVacia(&p))
				ocupadas.insert(p);
			else
				dest = p;
			}

		destinos.insert(dest);
		ocupadas.insert(dest);

		newEvent.extra1 = dest.getRoundX() +0.5;
		newEvent.extra2 = dest.getRoundY() +0.5;

		client->agregarEvento(newEvent);

		cout << "Se gener� un mover!!" << endl;
		
	}
}


void ProcesadorEventos::procesarSeleccionSecundaria(Cliente* client, Jugador* player) {
	list<Entidad*> lasEnt = game->verListaEntidadesSeleccionadas();
	if (lasEnt.empty())
		return;
	
	if (lasEnt.front()->verJugador()->verID() != client->playerID)
		return;

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	float pX = cu->obtenerCoordLogicaX(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
	float pY = cu->obtenerCoordLogicaY(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
	Posicion* dest = new Posicion(pX, pY);
	
	if(game->seleccionSecundaria != nullptr){
			delete game->seleccionSecundaria;
			game->seleccionSecundaria = nullptr;
			}
	game->seleccionSecundaria = dest;


	if (game->escenario->verMapa()->posicionPertenece(dest)){ 
		if(game->escenario->verMapa()->posicionEstaVacia(dest)){
			if(game->verEntidadSeleccionada()->verTipo() == ENT_T_UNIT)
				// Accion de mover seleccionado(s)
				this->procesarMover(client);
			else{
				delete game->seleccionSecundaria;
				game->seleccionSecundaria = nullptr;				
				}
			}
		else{ // Si la selec. secundaria es una Entidad...
			Entidad* target = game->escenario->verMapa()->verContenido(dest);
			Posicion* posTarg = target->verPosicion();
			entity_type_t tipo = target->verTipo();
			if(tipo == ENT_T_RESOURCE){
				// Procesar Recolectar
				this->procesarRecolectar(client);
				}
			else if(!player->poseeEntidad(target)){
				// Procesar Atacar
				this->procesarAtacar(client);
				}
			else{
				delete game->seleccionSecundaria;
				game->seleccionSecundaria = nullptr;				
				}
			}
		}

}


void ProcesadorEventos::procesarRecolectar(Cliente* client) {
	list<Entidad*> lasEnt = game->verListaEntidadesSeleccionadas();
	
	// Obtengo la posici�n del click (quiz�s convertir en una funci�n ya que se repite en varios lados...
	Posicion dest = *game->seleccionSecundaria;
	float pX = dest.getX();
	float pY = dest.getY();

	for (list<Entidad*>::iterator it=lasEnt.begin(); it != lasEnt.end(); ++it){
		if((*it)->habilidades[ACT_COLLECT]){
			msg_event newEvent;
			newEvent.idEntidad = (*it)->verID();
			newEvent.accion = MSJ_RECOLECTAR;
			int i = 0;
			dest = Posicion(pX, pY);

			// La posta
			Entidad* aux = game->escenario->verMapa()->verContenido(&dest);
			newEvent.extra1 = (float)aux->verID();

			cout<<(*it)->name << " debe recolectar (" << dest.getRoundX() << "," << dest.getRoundY() << ")" << endl;
				
			client->agregarEvento(newEvent);
		}
		
	}
}


void ProcesadorEventos::procesarAtacar(Cliente* client) {
	list<Entidad*> lasEnt = game->verListaEntidadesSeleccionadas();
	
	// Obtengo la posici�n del click (quiz�s convertir en una funci�n ya que se repite en varios lados...
	Posicion dest = *game->seleccionSecundaria;
	float pX = dest.getX();
	float pY = dest.getY();
	
	for (list<Entidad*>::iterator it=lasEnt.begin(); it != lasEnt.end(); ++it){
		if((*it)->habilidades[ACT_ATACK]){
			msg_event newEvent;
			newEvent.idEntidad = (*it)->verID();
			newEvent.accion = MSJ_ATACAR;
			int i = 0;
			dest = Posicion(pX, pY);

			// La posta
			Entidad* aux = game->escenario->verMapa()->verContenido(&dest);
			newEvent.extra1 = (float)aux->verID();
//				cout<<(*it)->name << " debe atacar (" << dest.getRoundX() << "," << dest.getRoundY() << ")" << endl;
				
			client->agregarEvento(newEvent);
	//	}
		}
		
	}
}

void ProcesadorEventos::procesarSeleccion(Jugador* player){
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	ConversorUnidades* cu = ConversorUnidades::obtenerInstancia();
	

	float pX = cu->obtenerCoordLogicaX(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
	float pY = cu->obtenerCoordLogicaY(mx, my, gp->getViewX(), gp->getViewY(), gp->getAnchoBorde());
	
	if((game->sx == 0)&&(game->sy == 0)){
		game->sx = mx;
		game->sy = my;
		game->sx2 = mx + TOL_SEL_MULT;
		game->sy2 = my + TOL_SEL_MULT;
	}
	
	// Si me paso del borde del screen_frame, me voy...
	if(my > (gp->screen_height*360/480)){
		if(mx < (gp->screen_width*165/640)){ 
			// ...A menos que haya clickeado un boton
			try{
				this->procesarBoton(player);
			} catch (const std::out_of_range& oor){
				cout << "Baboso!"<< endl;
				return;
				}
			}
		return;
		}

	Posicion slct = Posicion(pX, pY);
	game->deseleccionarEntidades();
	if(game->escenario->verMapa()->posicionPertenece(&slct))
		if(player->visionCasilla(slct) != VIS_NO_EXPLORADA)
			if(!game->escenario->verMapa()->posicionEstaVacia(&slct))
				game->seleccionarEntidad(game->escenario->verMapa()->verContenido(&slct), true);
}