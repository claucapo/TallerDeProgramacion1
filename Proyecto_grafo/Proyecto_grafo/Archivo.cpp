#include "Archivo.h"

Archivo::Archivo(const string &path, bool binario) {

	this->path = path;

	/*Primero intento crear los directorios por si no existen*/
	int posicionUltimoSlash = path.find_last_of("/");


	this->archivo.open(path.c_str(), fstream::binary | fstream::out
			| fstream::in);

	if (!this->archivo.is_open()) {
		ofstream aux(path.c_str());
		if (aux.is_open()) {
			aux.close();
		}
		this->archivo.open(path.c_str(), fstream::binary | fstream::out
				| fstream::in);
	}
	this->valido = this->archivo.good();
}

Archivo::~Archivo() {

	
}

void Archivo::escribir(const string& contenido, unsigned long int offset) {

	if (offset > getTamanio()) {
		cerr << "Intentando escribir en un offset no valido";
		return;
	}

	string buffer = contenido;

	this->archivo.seekp(offset, ios_base::beg);
	this->archivo.write(buffer.c_str(), buffer.size());
	this->archivo.flush();
}

string Archivo::leer(unsigned long int offset, unsigned long int cantidadBytes) {

	ifstream archivoEntrada;

	archivoEntrada.open(this->path.c_str());

	/* Valido que el offset y la cantidad de bytes sean validos */
	if (this->getTamanio() >= (offset + cantidadBytes)) {
		if (archivoEntrada.is_open()) {

			archivoEntrada.seekg(offset, ios_base::beg);
			char* buffer;
			buffer = new char[cantidadBytes];
			string stream;

			archivoEntrada.read(buffer, cantidadBytes);
			stream.append(buffer, cantidadBytes);
			delete[] buffer;

			return stream;
		}
	}

	string vacio = "";
	return vacio;
}

long Archivo::getTamanio() {

	if (archivo.is_open() && archivo.good()) {

		long size = 0;
		long posActual = this->archivo.tellp();

		this->archivo.seekp(0, std::ios::end);
		size = this->archivo.tellp();
		this->archivo.seekp(posActual);

		return size;
	}

	return 0;
}

bool Archivo::expandirArchivo(unsigned long int longitudArchivo) {

	if (this->getTamanio() >= longitudArchivo) {
		return false;
	}

	this->archivo.seekp(longitudArchivo - 1, ios_base::beg);
	this->archivo.write(" ", 1);
	this->archivo.flush();

	if (this->getTamanio() == 0) {
		this->valido = false;
	}
	return true;
}

bool Archivo::esValido() const {

	return this->valido;
}

string Archivo::getPath() {
	return this->path;
}

void Archivo::cerrar() {
	if (this->archivo.is_open()) {
		this->archivo.close();
	}
}

