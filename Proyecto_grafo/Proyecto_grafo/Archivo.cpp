#include "Archivo.h"

Archivo::Archivo(const string &path) {
	this->path = path;
	ofstream myfile;
    this->archivo.open (path.c_str(), fstream::app);
}

Archivo::~Archivo() {
}

void Archivo::escribir(const string& contenido) {
	this->archivo << contenido << endl;
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

string Archivo::getPath() {
	return this->path;
}

void Archivo::cerrar() {
	if (this->archivo.is_open()) {
		this->archivo.close();
	}
}

