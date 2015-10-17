#ifndef ERRORLOG_H
#define ERRORLOG_H

#include<string>
#include "Archivo.h"
#pragma once
using namespace std;

// Constantes y enumerados que apareceran en el flag.
enum log_lvl_t {LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_ALLWAYS};
const string log_lvl_str[] = {"[INFO]", "[WARNING]", "[ERROR]", "[ALLWAYS]"};
const string log_separator = " - ";

// La clase ErrorLog es la escribir mensajes en el archivo del log
// Esta implementado con un modelo singleton.
// TODO: - Cambiar metodos singulares por un método general que tome como parametro un valor del enumerado log_lvl_t???
//		 - Agregar un metodo que permita cambiar la ruta del log.
//		 - Reemplazar los flags separados por un único nivel, o incluirlos en un arreglo??
//		 - Permitir disintos tipos de mensajes sean logueados en distintos archivos (quizás es demasiado)

// Modo de uso:
// Escritura:
//		ErrorLog::getInstance()->escribirLog("Probando el log");

// Cerrar el log:
//		ErrorLog::cerrarLog();
// NO es necesario cerrarlo tras cada escritura, solo al finalizar
// la ejecución del programa para liberar la memoria pedida dinamicamente


class ErrorLog {
private:
	ErrorLog(void);
	// Archivo asociado
	static Archivo* arch;

	// Variables de clase del modelo singleton
	static ErrorLog* singleton;
	static bool hay_instancia;



public:
	// Metodo de clase para obtener la instancia
	static ErrorLog* getInstance();
		
	bool _warnings;
	bool _info; 
	bool _errores;

	~ErrorLog(void);
	
	// Cierra el archivo de log de errores
	// asociado a la ruta recibida por path
	// desde el constructor.
	// Post: Cierra el archivo de log de errores.
	// IMPORTANTE: debe llamarse cuando se deje de usar el log,
	// antes de cerrar la aplicación.
	static void cerrarLog(void);
		
	// Habilita o inhabilita los flags de
	// escritura de WARNINGS, INFO y ERROR.
	void habilitarFlags(bool warnings, bool info, bool errores);

	// Escribe el mensaje msj en el log de
	// errores.
	// Post: Se escribió el mensaje en el log
	// Este metodo garantiza la escritura sin importar
	// los flags.
	void escribirLog(string msj, log_lvl_t lvl = LOG_ALLWAYS);


	// Escribe el mensaje msj en el log de
	// errores como tipo WARNING.
	// Post: Se escribió el WARNING en el log
	// NOTA: Sólo escribe si el flag de warnings
	// estaba previamente activado.
	void escribirLogW(string msj);

	// Escribe el mensaje msj en el log de
	// errores como tipo INFO.
	// Post: Se escribió la INFO en el log
	// NOTA: Sólo funciona si el flag de info
	// estaba previamente activado.
	void escribirLogI(string msj);
	
	// Escribe el mensaje msj en el log de
	// errores como tipo ERROR.
	// Post: Se escribió el ERROR en el log
	// NOTA: Sólo funciona si el flag de errores
	// estaba previamente activado.
	void escribirLogE(string msj);
};


#endif