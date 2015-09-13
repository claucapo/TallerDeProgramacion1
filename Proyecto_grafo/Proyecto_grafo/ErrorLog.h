#include<string>
#include "Archivo.h"

#pragma once
using namespace std;

// La clase ErrorLog es la encargada de abrir,
// cerrar y escribir en el log de errores.
// ¿Implementar usando singleton?
class ErrorLog {
public:
	// Constructor y destructor por defecto
	ErrorLog(void);
	~ErrorLog(void);
	Archivo* arch;
	bool _warnings;
	bool _info; 
	bool _errores;

	// Crea un ErrorLog asociado al archivo 
	// de log de errores en el directorio 
	// especificado por path.
	ErrorLog(string path);

	// Escribe el mensaje msj en el log de
	// errores.
	// Pre: Existe un archivo de log asociado.
	// Post: Se escribió el mensaje en el log
	// y se lo dejó listo para otra escritura.
	void escribirLog(string msj);

	// Escribe el mensaje msj en el log de
	// errores como tipo WARNING.
	// Pre: Existe un archivo de log asociado.
	// Post: Se escribió el WARNING en el log
	// y se lo dejó listo para otra escritura.
	// NOTA: Sólo funciona si el flag de warnings
	// estaba previamente activado.
	void escribirLogW(string msj);

	// Escribe el mensaje msj en el log de
	// errores como tipo INFO.
	// Pre: Existe un archivo de log asociado.
	// Post: Se escribió la INFO en el log
	// y se lo dejó listo para otra escritura.
	// NOTA: Sólo funciona si el flag de info
	// estaba previamente activado.
	void escribirLogI(string msj);
	
	// Escribe el mensaje msj en el log de
	// errores como tipo ERROR.
	// Pre: Existe un archivo de log asociado.
	// Post: Se escribió el ERROR en el log
	// y se lo dejó listo para otra escritura.
	// NOTA: Sólo funciona si el flag de errores
	// estaba previamente activado.
	void escribirLogE(string msj);

	// Inicializa y abre un archivo de log de
	// errores asociado a la ruta recibida por
	// path desde el constructor.
	// Pre: Existe un path asociado.
	// Post: Deja el archivo de log de errores
	// abierto y listo para la escritura
	void abrirLog(void);

	// Cierra el archivo de log de errores
	// asociado a la ruta recibida por path
	// desde el constructor.
	// Pre: Existe un archivo abierto asociado.
	// Post: Cierra el archivo de log de errores.
	void cerrarLog(void);

	// Habilita o inhabilita los flags de
	// escritura de WARNINGS, INFO y ERROR.
	void habilitarFlags(bool warnings, bool info, bool errores);
};

