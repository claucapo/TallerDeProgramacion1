#include "ErrorLog.h"


ErrorLog::ErrorLog(void)
{
}


ErrorLog::~ErrorLog(void)
{
}


void ErrorLog::escribirLog(string msj)
{
	arch->escribir(msj + "\n",arch->getTamanio());
}


void ErrorLog::escribirLogW(string msj)
{
	if(_warnings){
	    arch->escribir("WARNING " + msj + "\n" ,arch->getTamanio());
	}
}


void ErrorLog::escribirLogI(string msj)
{
	if(_info){
	   arch->escribir("INFO"  + msj + "\n",arch->getTamanio());
	}
}


void ErrorLog::escribirLogE(string msj)
{
	if(_errores){
	   arch->escribir("ERROR " +  msj + "\n",arch->getTamanio());
	}
}


void ErrorLog::abrirLog(void)
{
	arch = new Archivo("c:\\Proyecto_grafo.log",false);
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
