/*#include <stdio.h>
#include <list>
#include <conio.h>
#include "Posicion.h"
#include <map>
#include "Grafo.h"



int main(){
	using namespace std;
	// Ejemplo de uso de lista con elementos estáticos
	list <int> mi_lista;

	mi_lista.push_front(1);
	printf("%d\n", mi_lista.size());
	mi_lista.push_back(2);
	printf("%d\n", mi_lista.size());
	printf("%d\n", mi_lista.front());

	mi_lista.pop_front();
	printf("%d\n", mi_lista.size());
	printf("%d\n", mi_lista.front());

	
	// Ejemplo de uso de lista con elem. dinámicos
	list<Posicion*> p_list;
	int i;
	for(i = 0; i < 10; i++){
		Posicion* pp = new Posicion(0, i);
		p_list.push_front(pp);
	}
	
	printf("%d\n", p_list.size());
	Posicion* pos = p_list.front();
	printf("Frente: %d-%d\n", pos->devolverCoordX(), pos->devolverCoordY());
	pos = p_list.back();
	printf("Back: %d-%d\n", pos->devolverCoordX(), pos->devolverCoordY());

	for(i = 0; i < 10; i++){
		Posicion* pp = p_list.front();
		delete pp;
		p_list.pop_front();
	}
		
	printf("%d\n", p_list.size());

	// Ejemplo de uso de mapa (dictionary)
	map<char, int> dic;
	dic['A'] = 65;
	dic['C'] = 67;
	dic['K'] = 75;

	printf("%d\n", dic.size());
	printf("%d\n", dic.at('A'));
	printf("%d\n", dic.at('K'));
	printf("%d\n", dic.at('C'));
	dic.erase('C');
	printf("%d\n", dic.size());

	// Grafo!
	printf("GRAFO\n");
	Grafo grafito;
	printf("%d\n", grafito.size());
	if(grafito.estaVacio())
		printf("EMPTY\n");
	Posicion p(1,1);
	grafito.crearVertice(p);
	Posicion p44(2,1);
	grafito.crearVertice(p44);
	Posicion p4(3,1);
	grafito.crearVertice(p4);
	printf("%d\n", grafito.size());

	if(!grafito.crearCamino(p, p44, false, 2))
		printf("MISSED!\n");
	if(grafito.elementosEstanConectados(p, p44))
		printf("Arista creada con exito!\n");
	if(!grafito.crearCamino(p, p4, true, 8))
		printf("MISSED!\n");
	if(grafito.elementosEstanConectados(p, p4))
		printf("Arista creada con exito!\n");
	if(grafito.elementosEstanConectados(p4, p))
		printf("Arista bidirigida creada con exito!\n");

	Posicion p11(7,7);
	grafito.crearVertice(p11);
	if(!grafito.crearCamino(p11, p44, false, 2))
		printf("MISSED!\n");
	if(grafito.elementosEstanConectados(p11, p44))
		printf("Arista creada con exito!\n");
	printf("%d\n", grafito.size());

	grafito.borrarCamino(p11, p44);
	if(!grafito.elementosEstanConectados(p11, p44))
		printf("Borrado exitoso!\n");
	if(!grafito.crearCamino(p11, p44, false, 2))
		printf("MISSED!\n");
	if(grafito.elementosEstanConectados(p11, p44))
		printf("Arista creada con exito!\n");
	grafito.borrarVertice(p11);
	if(!grafito.elementosEstanConectados(p11, p44))
		printf("Borrado exitoso!\n");
	printf("%d\n", grafito.size());
	grafito.borrarVertice(p);
	grafito.borrarVertice(p44);
	grafito.borrarVertice(p4);
	if(grafito.estaVacio())
		printf("EMPTY\n");




	getch();
	return 0;
}*/