#include <list>
#include "Matriz.h"
#include "ErrorLog.h"
#include "CalculadorCaminoMinimo.h"

using namespace std;

//****************************//
//*********CONSTANTES*********//
//****************************//

#define TAM_DEFAULT 100

// Se genera una matriz pidiendo una array de arrays dinamico
Matriz::Matriz(void) {
	this->filas = TAM_DEFAULT;
	this->columnas = TAM_DEFAULT;
	this->cantidad_entidades = 0;

	casillas = new Entidad**[TAM_DEFAULT];
	for(int i = 0; i < TAM_DEFAULT; i++)
		casillas[i] = new Entidad*[TAM_DEFAULT];


	//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
	this->calculadorCamino = new CalculadorCaminoMinimo(TAM_DEFAULT,TAM_DEFAULT);
	mapDeOcupaciones = new int*[TAM_DEFAULT];
	for(int j = 0; j < TAM_DEFAULT; j++)
		mapDeOcupaciones[j] = new int[TAM_DEFAULT];

	generarMatrizVacia();

}


// Mismo constructor, pero especificando la cantidad de filas y columnas
//
Matriz::Matriz(int casillas_x, int casillas_y) {

	this->filas = casillas_x;
	this->columnas = casillas_y;
	this->cantidad_entidades = 0;

	casillas = new Entidad**[casillas_x];
	for(int i = 0; i < casillas_x; i++)
		casillas[i] = new Entidad*[casillas_y];

	//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
	this->calculadorCamino = new CalculadorCaminoMinimo(casillas_x,casillas_y);
	mapDeOcupaciones = new int*[casillas_x];
	for(int j = 0; j < casillas_x; j++)
		mapDeOcupaciones[j] = new int[casillas_y];

	generarMatrizVacia();
}

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
// Destructor que libera los arrays pedidos dinamicamente
Matriz::~Matriz(void) {
	for(int i = 0; i < this->filas; i++){
		delete[] casillas[i];
		delete[] mapDeOcupaciones[i];
	}
	delete[] casillas;
	delete[] mapDeOcupaciones;
} 


//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
void Matriz::generarMatrizVacia(){
	for(int i = 0; i < this->filas; i++){
		for(int j = 0; j < this->columnas; j++){
		 casillas[i][j] = nullptr;
		 mapDeOcupaciones[i][j] = 0;
		}
	}
} 

bool Matriz::posicionPertenece(Posicion* pos){
	float pos_x = pos->getX();
	float pos_y = pos->getY();
	if((pos_x < 0) || (pos_y < 0))
		return false;
	if ((this->filas <= pos_x) || (this->columnas <= pos_y))
		return false;
	return true;
}

bool Matriz::posicionEstaVacia(Posicion* pos){
	if (!posicionPertenece(pos)) {
		// ErrorLog::getInstance()->escribirLog("Se pregunta por una posicion invalida: " + pos->toStrRound() + "." , LOG_WARNING);
		return false;
	}
	if(casillas[pos->getRoundX()][pos->getRoundY()] != nullptr)
		return false;
	return true;
}


bool Matriz::quitarEntidad(Entidad* elemento){

	if (!elemento || !elemento->verPosicion()) {
		// ErrorLog::getInstance()->escribirLog("Error al querer remover [" + elemento->name + "]: No se encuentra en el mapa.", LOG_ERROR);
		return false;
	}

	Posicion* pos = elemento->verPosicion();
	if (! posicionPertenece(pos))
		return false;

	// Obtengo el tamanio de la entidad a quitar
	int altura_x = elemento->verTamX();
	int ancho_y = elemento->verTamY();

	// Por ahora solo desreferencio a la entidad en las posiciones en que estaba ubicada
	for(int i = 0; i < altura_x; i++){
		for(int j = 0; j < ancho_y; j++) {

			casillas[pos->getRoundX() + i][pos->getRoundY() + j] = nullptr;
			mapDeOcupaciones[pos->getRoundX() + i][pos->getRoundY() + j] = 0;
			// cout << "Se vació la posicion: " << pos->toStrRound() << endl;
		}
	}
	this->cantidad_entidades--;
	return true;
}

bool Matriz::ubicarEntidad(Entidad* elemento, Posicion* pos){
	// TODO: levantar error o warning cuando pase esto
	if (!elemento) {
		return false;
		// ErrorLog::getInstance()->escribirLog("Se quiso agregar un elemento vacio al mapa.", LOG_WARNING);
		}
	if (!posicionPertenece(pos)) {
		// ErrorLog::getInstance()->escribirLog("Error al querer asignar posicion a [" + elemento->verNombre() + "]. Posicion " + pos->toStrRound() + " inexistente.", LOG_WARNING);
		return false;
	}
		
	// Obtengo el tamanio de la entidad
	int altura_x = elemento->verTamX();
	int ancho_y = elemento->verTamY();

	// Primero chequeo que las posiciones esten libres, y que caigan dentro del mapa
	for(int i = 0; i < altura_x; i++)
		for(int j = 0; j < ancho_y; j++) {
			Posicion act = Posicion(pos->getX() + i, pos->getRoundY() + j);
			if ((!posicionPertenece(&act)) || (this->filas == act.getX()) || (this->columnas == act.getY())) {
				// ErrorLog::getInstance()->escribirLog("No se puede agregar entidad [" + elemento->verNombre() +"] en " + pos->toStrRound() + ". Posición no pertenece al mapa.", LOG_WARNING);
				return false;
			}
			if(casillas[pos->getRoundX() + i][pos->getRoundY() + j] != nullptr) {
				Posicion tmp = Posicion(pos->getRoundX() + i,pos->getRoundY() + j);
				// ErrorLog::getInstance()->escribirLog("No se puede agregar entidad [" + elemento->verNombre() +"] en " + pos->toStrRound() + ". Colisiona con otra entidad en" + tmp.toStrRound() +".", LOG_WARNING);
				return false;
			}
		}


	// Si llego acá es porque todas las posiciones están libres
	for(int i = 0; i < altura_x; i++)
		for(int j = 0; j < ancho_y; j++){
			casillas[pos->getRoundX() + i][pos->getRoundY() + j] = elemento;
			mapDeOcupaciones[pos->getRoundX() + i][pos->getRoundY() + j]= 1;
			}
		
	this->cantidad_entidades++;
	return true;
}

Entidad* Matriz::verContenido(Posicion* pos) {
	if (!this->posicionPertenece(pos)) {
		// ErrorLog::getInstance()->escribirLog("Error al querer ver contenido de posicion " + pos->toStrRound() + ", posición inexistente.", LOG_WARNING);
		return nullptr;
	} else {
		return this->casillas[pos->getRoundX()][pos->getRoundY()];
	}
}

// Calcula la distancia hamiltoniana entre una posicion y una entidad.
int Matriz::distanciaEntre(Posicion pos, Entidad* ent) {
	int distX = 0;
	int distY = 0;

	int minX = ent->verPosicion()->getRoundX();
	int minY = ent->verPosicion()->getRoundY();
	int maxX = minX + ent->verTamX();
	int maxY = minY + ent->verTamY();

	int actX = pos.getRoundX();
	int actY = pos.getRoundY();

	if (actX < maxX && actX >= minX)
		distX = 0;
	else if (actX >= maxX)
		distX = actX - maxX + 1;
	else
		distX = minX - actX;

	if (actY < maxY && actY >= minY)
		distY = 0;
	else if (actY >= maxY)
		distY = actY - maxY + 1;
	else
		distY = minY - actY;
	return distX + distY;
}

// Devuelve la lista de posiciones que estan dentro del rango de vision de la entidad
list<Posicion> Matriz::posicionesVistas(Entidad* elemento, int rangoV) {
	int rango;
	if (rangoV == 0)
		rango = elemento->verRango();
	else
		rango = rangoV;

	list<Posicion> posEnRango;
	int origenX = elemento->verPosicion()->getRoundX();
	int origenY = elemento->verPosicion()->getRoundY();
	for (int i = -rango; i <= rango + elemento->verTamX(); i++) {
		for (int j = -rango; j <= rango + elemento->verTamY(); j++) {
			Posicion act = Posicion(i + origenX, j + origenY);
			if (this->posicionPertenece(&act) && distanciaEntre(act, elemento) <= rango) {
				posEnRango.push_back(act);
				/*if (!this->posicionEstaVacia(&act)) {
					list<Posicion> posOcupadas = this->posicionesOcupadas(this->verContenido(&act));
					while (!posOcupadas.empty()) {
						Posicion pos = posOcupadas.front();
						posOcupadas.pop_front();
						posEnRango.push_back(pos);
					}
				}*/
			}
		}
	}
	return posEnRango;
}

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
void Matriz::actualizarMapDeOcupaciones(){
	for(int i = 0; i < this->filas; i++){
		for(int j = 0; j < this->columnas; j++){
		 if (casillas[i][j] != nullptr)
			 mapDeOcupaciones[i][j]= 1;
		}
	}
}
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
list<Posicion*> Matriz::caminoMinimo(Posicion posAct, Posicion posDest){

	int actX = posAct.getRoundX();
	int actY = posAct.getRoundY();
	int destX = posDest.getRoundX();
	int destY = posDest.getRoundY();
	this->actualizarMapDeOcupaciones();
	return this->calculadorCamino->calcularCaminoMinimo(actX,actY,destX,destY,this->mapDeOcupaciones);

}


// Devuelve la lista de posiciones que estan dentro del rango de vision de la entidad
list<Posicion> Matriz::posicionesOcupadas(Entidad* elemento) {
	list<Posicion> posEnRango;
	int origenX = elemento->verPosicion()->getRoundX();
	int origenY = elemento->verPosicion()->getRoundY();
	for (int i = 0; i < elemento->verTamX(); i++) {
		for (int j = 0; j < elemento->verTamY(); j++) {
			Posicion act = Posicion(i + origenX, j + origenY);
			if (this->posicionPertenece(&act)) {
				posEnRango.push_back(act);
			}
		}
	}
	return posEnRango;
}


// Devuelve la lista de posiciones adyacentes
list<Posicion> Matriz::adyacentesVacias(Entidad* elemento) {
	list<Posicion> posAdyacentes;
	Posicion pos;
	int origenX = elemento->verPosicion()->getRoundX();
	int origenY = elemento->verPosicion()->getRoundY();
	for (int i = -1; i <= elemento->verTamX(); i++) {
		pos = Posicion(origenX + i, origenY-1);
		if(this->posicionPertenece(&pos)){
		if (this->posicionEstaVacia(&pos))
			posAdyacentes.push_back(pos);

		pos = Posicion(origenX + i, origenY + elemento->verTamY());
		if (this->posicionEstaVacia(&pos))
			posAdyacentes.push_back(pos);
		}
	}

	for (int i = 0; i < elemento->verTamY(); i++) {
		pos = Posicion(origenX - 1, origenY + i);
		if (this->posicionEstaVacia(&pos))
			posAdyacentes.push_back(pos);

		pos = Posicion(origenX + elemento->verTamX(), origenY + i);
		if (this->posicionEstaVacia(&pos))
			posAdyacentes.push_back(pos);
	}
	return posAdyacentes;
}


Posicion* Matriz::adyacenteCercana(Entidad* destino, Entidad* origen) {
	list<Posicion> adyacentes = this->adyacentesVacias(destino);
	Posicion pos;
	if (adyacentes.empty())
		return nullptr;
	pos = adyacentes.front();
	adyacentes.pop_front();
	int dist = this->distanciaEntre(pos, origen);
	while (!adyacentes.empty()) {
		Posicion aux = adyacentes.front();
		adyacentes.pop_front();
		int distTmp = this->distanciaEntre(aux, origen);
		if (distTmp < dist) {
			pos = aux;
			dist = distTmp;
		}

	}
	return new Posicion(pos.getX(), pos.getY());
}