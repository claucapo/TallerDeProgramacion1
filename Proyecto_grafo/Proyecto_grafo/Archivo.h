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
	Archivo(const string &path, bool binario = true);

	/*
	 * Pre: -
	 * Pos: Libera los recursos instanciados.
	 */
	virtual ~Archivo();

	/*
	 * Pre: offset valido
	 * Pos: escribe en el archivo los Bytes pasados por argumento.
	 */
	virtual void escribir(const string&, unsigned long int offset);

	/*
	 * Pre: argumentos validos
	 * Pos: devuelve una tira de bytes con la informacion obtenida.
	 */
	virtual string leer(unsigned long int offset, unsigned long int cantBytes);

	/*
	 * Pre: -
	 * Pos: devuelve el tamanio en bytes del archivo.
	 */
	long getTamanio();

	/*
	 * Pre: Memoria disponible en disco rigido.
	 * Pos: Expande el archivo.
	 */
	bool expandirArchivo(unsigned long int longitudArchivo);

	/*
	 * Pre: -
	 * Pos: devuelve true si el archivo es valido.
	 */
	bool esValido() const;

	/*
	 * Pre: -
	 * Pos: devuelve el path del archivo.
	 */
	string getPath();
	
	virtual void cerrar();

};

#endif /* ARCHIVO_H_ */
