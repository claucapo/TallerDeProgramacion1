#ifndef EDIFICIO_H
#define EDIFICIO_H

#include "Entidad.h"
#include "Enumerados.h"

#include <queue>

using namespace std;

struct training_dummy {
	string name;
	unsigned int typeID;
	int totalTicks;
};

class Edificio : public Entidad {
private:
	int cant_entrenables;

	// Cantidad de unidades en produccion en un momento dado
	// No puede exceder a la constante MAX_PRODUCCION
	int cant_en_produccion;

	std::queue<training_dummy> cola_produccion;

	// Metodo que se tiene que llamar cada vez que una unidad nueva
	// pasa al frente de la cola de produccion. Recalcula los
	// ticks_totales, y ticks_restantes.
	void recalcularTicks(void);

public:
		
	int ticks_restantes;
	int ticks_totales;
	std::string entrenables[MAX_ENTRENABLES];


	Edificio(unsigned int id, string name, tipoEntidad_t pType);
	~Edificio();

	virtual af_result_t avanzarFrame(Escenario* scene);

	// Dado un identificador de entidad, devuelve true si el edificio puede entrenar entidades de ese tipo.
	bool puedeEntrenar(string name);

	// Ordena al edificio a insertar en la cola de produccion una nueva entidad
	// correspondiente al identificador "name". Devuelve true si la operación
	// fue existosa. Si la cola de produccion ya estaba llena, o el edificio no
	// puede entrenar ese tipo e entidad, devuelve false.
	bool entrenarUnidad(unsigned int typeID, int totalTicks);	

	void finalizarEntrenamiento(void);
	void resetearTicks(void);

	// Devuelve la relación entre el tiempo elapsado en el entrenamiento de una unidad
	// y el tiempo total necesario para completar el entrenamiento.
	// Devuelve -1 si no hay unidades en la cola de produccion.
	float porcentajeEntrenado(void);


	// Cambia el tipo de entidad de acuerdo a si el edificio está terminado o no,
	// para que se lo trate apropiadamente.
	void setEnConstruccion(bool construyendo);

};



#endif // EDIFICIO_H