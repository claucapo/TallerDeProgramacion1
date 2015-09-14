#include "Tests.h"
#include "ErrorLog.h"
#include <iostream>
using namespace std;


int main(int argc, char** argv) {
	testEscenarioBasico();

	// Nuevo uso del log como singleton
	ErrorLog::getInstance()->escribirLogW("Probando el log");
	ErrorLog::cerrarLog();
	char c;
	cin >> c;
	return 0;
}