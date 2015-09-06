#include "ErrorLog.h"
#include <ctime>


ErrorLog::ErrorLog(void)
{
}


ErrorLog::~ErrorLog(void)
{
}

string obtenerFechaHora(){
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,80,"%d/%m/%Y %I:%M:%S",timeinfo);
  std::string str(buffer);

  return str + " ";
}

void ErrorLog::escribirLog(string msj)
{
	arch->escribir(obtenerFechaHora() + msj);
}


void ErrorLog::escribirLogW(string msj)
{
	if(_warnings){
	    arch->escribir(obtenerFechaHora() + "WARNING " + msj);
	}
}


void ErrorLog::escribirLogI(string msj)
{
	if(_info){
	   arch->escribir(obtenerFechaHora() + "INFO"  + msj);
	}
}


void ErrorLog::escribirLogE(string msj)
{
	if(_errores){
	   arch->escribir(obtenerFechaHora() + "ERROR " +  msj);
	}
}


void ErrorLog::abrirLog(void)
{
	arch = new Archivo("c:\\Proyecto_grafo.log");
}


void ErrorLog::cerrarLog(void)
{
	arch->cerrar();
}


void ErrorLog::habilitarFlags(bool warnings, bool info, bool errores)
{
	_warnings = warnings;
	_info = info;
	_errores = errores;
}
