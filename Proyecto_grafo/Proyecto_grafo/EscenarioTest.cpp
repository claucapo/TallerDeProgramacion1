#include <stdio.h>
#include <list>
#include <conio.h>
#include "Posicion.h"
#include <map>
#include "Grafo.h"
#include "Escenario.h"

using namespace std;

/* Función auxiliar para imprimir si estuvo OK o no. */
void print_test(char* name, bool result) {
	printf("%s: %s\n", name, result? "OK" : "ERROR");
}

void testEscenarioChico(){
	Escenario ej(5, 4);

	Grafo mapa = ej.verMapa();

	print_test("Tam. grafo correcto", mapa.size()==20);
	print_test("(0,0) pertenece", mapa.elementoPertenece(Posicion(0,0)));
	print_test("(2,3) pertenece", mapa.elementoPertenece(Posicion(2,3)));
	print_test("(4,3) pertenece", mapa.elementoPertenece(Posicion(4,3)));
	print_test("(5,5) no pertenece", !mapa.elementoPertenece(Posicion(5,5)));
	print_test("(0,0) y (0,1) conectados", mapa.elementosEstanConectados(Posicion(0,0), Posicion(0,1)));
	print_test("(2,3) y (1,3) conectados", mapa.elementosEstanConectados(Posicion(2,3), Posicion(1,3)));
	print_test("(3,3) y (3,2) conectados", mapa.elementosEstanConectados(Posicion(3,3), Posicion(3,2)));
	print_test("(2,3) y (1,2) conectados", mapa.elementosEstanConectados(Posicion(2,3), Posicion(1,2)));
	print_test("(1,1) y (3,3) no conectados", !mapa.elementosEstanConectados(Posicion(1,1), Posicion(3,3)));

}

void testEscenarioGrande(){
	Escenario ej(100, 100);

	Grafo mapa = ej.verMapa();

	print_test("Tam. grafo correcto", mapa.size()== 10000);
	print_test("(0,0) pertenece", mapa.elementoPertenece(Posicion(0,0)));
	print_test("(99,99) pertenece", mapa.elementoPertenece(Posicion(99,99)));
	print_test("(56,78) pertenece", mapa.elementoPertenece(Posicion(56,78)));
	print_test("(500,500) no pertenece", !mapa.elementoPertenece(Posicion(500,500)));
	print_test("(99,99) y (99,98) conectados", mapa.elementosEstanConectados(Posicion(99,99), Posicion(99,98)));
	print_test("(54,31) y (53,30) conectados", mapa.elementosEstanConectados(Posicion(54,31), Posicion(53,30)));
	print_test("(37,13) y (38,13) conectados", mapa.elementosEstanConectados(Posicion(37,13), Posicion(38,13)));
	print_test("(21,39) y (22,40) conectados", mapa.elementosEstanConectados(Posicion(21,39), Posicion(22,40)));
	print_test("(23,18) y (34,35) no conectados", !mapa.elementosEstanConectados(Posicion(23,18), Posicion(34,35)));

}

int main(){

	testEscenarioChico();

	testEscenarioGrande();

	getch();
	return 0;
}
	