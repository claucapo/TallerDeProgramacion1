#ifndef ARCHIVO_H_
#define ARCHIVO_H_



#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>


using namespace std;

class Archivo {

private:
	fstream archivo;
	string path;
	bool valido;

public:
	/*
	 * Pre: path valido.
	 * Pos: Crea una nueva instancia.
	 */
	Archivo(const string &path);

	/*
	 * Pre: -
	 * Pos: Libera los recursos instanciados.
	 */
	virtual ~Archivo();

	/*
	 * Pre: archivo abierto
	 * Pos: escribe en el archivo el string pasado por argumento.
	 */
	virtual void escribir(const string&);


	/*
	 * Pre: -
	 * Pos: devuelve el tamanio en bytes del archivo.
	 */
	long getTamanio();

	/*
	 * Pre: -
	 * Pos: devuelve el path del archivo.
	 */
	string getPath();
	
	virtual void cerrar();

	bool is_open() {return archivo.is_open();}

};

#endif /* ARCHIVO_H_ */
