#include "ErrorLog.h"
#include <ctime>

#define LOG_DIR_DEFAULT "C:\\log.log"

bool ErrorLog::hay_instancia = false;
ErrorLog* ErrorLog::singleton = nullptr;
Archivo* ErrorLog::arch = nullptr;

bool _warnings = true;
bool _info = true;
bool _errores = true;

ErrorLog::~ErrorLog(void) {
	if (arch->is_open())
		arch->cerrar();
	delete arch;
	singleton = nullptr;
	arch = nullptr;
	hay_instancia = false;
}

ErrorLog* ErrorLog::getInstance(void) {
	if (!hay_instancia) {
		singleton = new ErrorLog();
		arch = new Archivo(LOG_DIR_DEFAULT);
		hay_instancia = true;
	}
	return singleton;
}

string obtenerFechaHora(){
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,80,"%d/%m/%Y %I:%M:%S",timeinfo);
  std::string str(buffer);

  return "[" + str + "]";
}

void ErrorLog::escribirLog(string msj, log_lvl_t lvl) {
	switch(lvl) {
	case LOG_ALLWAYS:
		arch->escribir(obtenerFechaHora()+ log_separator + log_lvl_str[LOG_ALLWAYS] + log_separator + msj);
		break;
	case LOG_WARNING:
		this->escribirLogW(msj);
		break;
	case LOG_ERROR:
		this->escribirLogE(msj);
		break;
	case LOG_INFO:
		this->escribirLogI(msj);
		break;
	}
}

void ErrorLog::escribirLogW(string msj) {
	if(_warnings){
	    arch->escribir(obtenerFechaHora()+ log_separator + log_lvl_str[LOG_WARNING] + log_separator + msj);
	}
}

void ErrorLog::escribirLogI(string msj) {
	if(_info){
		arch->escribir(obtenerFechaHora()+ log_separator + log_lvl_str[LOG_INFO] + log_separator + msj);
	}
}

void ErrorLog::escribirLogE(string msj) {
	if(_errores){
		arch->escribir(obtenerFechaHora()+ log_separator + log_lvl_str[LOG_ERROR] + log_separator + msj);
	}
}

void ErrorLog::cerrarLog(void) {
	if (hay_instancia)
		delete singleton;
}

void ErrorLog::habilitarFlags(bool warnings, bool info, bool errores) {
	_warnings = warnings;
	_info = info;
	_errores = errores;
}