#ifndef EDIFICIO_H
#define EDIFICIO_H

#include "Entidad.h"
#include "Enumerados.h"

#include <queue>

using namespace std;

class Edificio : public Entidad {
private:
	int cant_entrenables;
	std::string entrenables[MAX_ENTRENABLES];

	// Cantidad de unidades en produccion en un momento dado
	// No puede exceder a la constante MAX_PRODUCCION
	int cant_en_produccion;

	int ticks_restantes;
	int ticks_totales;
	std::queue<Entidad*> cola_produccion;

	// Metodo que se tiene que llamar cada vez que una unidad nueva
	// pasa al frente de la cola de produccion. Recalcula los
	// ticks_totales, y ticks_restantes.
	void recalcularTicks(void);

public:

	Edificio(unsigned int id, string name, tipoEntidad_t pType);
	~Edificio();

	virtual af_result_t avanzarFrame(Escenario* scene);

	// Dado un identificador de entidad, devuelve true si el edificio puede entrenar entidades de ese tipo.
	bool puedeEntrenar(string name);

	// Obtiene la unidad terminada, en caso de que haya alguno.
	// De lo contrario devuelve null
	Entidad* obtenerUnidadEntrenada(void);

	// Ordena al edificio a insertar en la cola de produccion una nueva entidad
	// correspondiente al identificador "name". Devuelve true si la operación
	// fue existosa. Si la cola de produccion ya estaba llena, o el edificio no
	// puede entrenar ese tipo e entidad, devuelve false.
	bool entrenarUnidad(string name);

};



#endif // EDIFICIO_H