#include <stdio.h>
#include <list>
#include <conio.h>
#include "Posicion.h"
#include <map>
#include "Grafo.h"
#include "Escenario.h"
#include "ErrorLog.h"
#include <fstream>
#include "Spritesheet.h"
#include "BibliotecaDeImagenes.h"
#include <SDL.h>
#include <SDL_image.h>


	// COPIO A LO TURBIO LAS FUNCIONES DE SDL

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image
SDL_Surface* loadSurface( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if( !( IMG_Init( imgFlags ) & imgFlags ) )
			{
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				success = false;
			}
			else
			{
				//Get window surface
				gScreenSurface = SDL_GetWindowSurface( gWindow );
			}
		}
	}

	return success;
}

void close()
{


	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

	// <------ hasta acá xd


//YAML 0.3!!
#include <yaml-cpp\yaml.h>

using namespace std;

/* Función auxiliar para imprimir si estuvo OK o no. */
void print_test(char* name, bool result) {
	printf("%s: %s\n", name, result? "OK" : "ERROR");
}

void testEscenarioChico(){
	
	Escenario ej(5, 4);

	Grafo mapa = ej.verMapa();

	print_test("Tam. grafo correcto", mapa.size()==20);
	print_test("(0,0) pertenece", mapa.elementoPertenece(Posicion(0,0)));
	print_test("(2,3) pertenece", mapa.elementoPertenece(Posicion(2,3)));
	print_test("(4,3) pertenece", mapa.elementoPertenece(Posicion(4,3)));
	print_test("(5,5) no pertenece", !mapa.elementoPertenece(Posicion(5,5)));
	print_test("(0,0) y (0,1) conectados", mapa.elementosEstanConectados(Posicion(0,0), Posicion(0,1)));
	print_test("(2,3) y (1,3) conectados", mapa.elementosEstanConectados(Posicion(2,3), Posicion(1,3)));
	print_test("(3,3) y (3,2) conectados", mapa.elementosEstanConectados(Posicion(3,3), Posicion(3,2)));
	print_test("(2,3) y (1,2) conectados", mapa.elementosEstanConectados(Posicion(2,3), Posicion(1,2)));
	print_test("(1,1) y (3,3) no conectados", !mapa.elementosEstanConectados(Posicion(1,1), Posicion(3,3)));
	


}

void testEscenarioGrande(){
	Escenario ej(100, 100);

	Grafo mapa = ej.verMapa();

	print_test("Tam. grafo correcto", mapa.size()== 10000);
	print_test("(0,0) pertenece", mapa.elementoPertenece(Posicion(0,0)));
	print_test("(99,99) pertenece", mapa.elementoPertenece(Posicion(99,99)));
	print_test("(56,78) pertenece", mapa.elementoPertenece(Posicion(56,78)));
	print_test("(500,500) no pertenece", !mapa.elementoPertenece(Posicion(500,500)));
	print_test("(99,99) y (99,98) conectados", mapa.elementosEstanConectados(Posicion(99,99), Posicion(99,98)));
	print_test("(54,31) y (53,30) conectados", mapa.elementosEstanConectados(Posicion(54,31), Posicion(53,30)));
	print_test("(37,13) y (38,13) conectados", mapa.elementosEstanConectados(Posicion(37,13), Posicion(38,13)));
	print_test("(21,39) y (22,40) conectados", mapa.elementosEstanConectados(Posicion(21,39), Posicion(22,40)));
	print_test("(23,18) y (34,35) no conectados", !mapa.elementosEstanConectados(Posicion(23,18), Posicion(34,35)));

}

struct UnidadTest {
	std::string nombre;
	int ataque;
};

void operator >> (const YAML::Node& node, UnidadTest& unit) {
	node["nombre"] >> unit.nombre;
	node["ataque"] >> unit.ataque;
}

void testEmitirYAML() {
	YAML::Emitter out;

	// Secuencia de categorías
	out << YAML::BeginSeq;
	// Primer categoría
	out << YAML::BeginMap;
	out << YAML::Key << "categoria" << YAML::Value << "Civiles";
	out << YAML::Key << "unidades" << YAML::Value << YAML::BeginSeq;
	// Aldeano
	out << YAML::BeginMap << YAML::Key << "nombre" << YAML::Value << "Aldeano";
	out << YAML::Key << "ataque" << YAML::Value << "1" << YAML::EndMap;
	// Carreta
	out << YAML::BeginMap << YAML::Key << "nombre" << YAML::Value << "Carreta Mercante";
	out << YAML::Key << "ataque" << YAML::Value << "0" << YAML::EndMap;
	// Fin de unidades << Fin de categoría
	out << YAML::EndSeq << YAML::EndMap;

	// Segunda categoria	
	out << YAML::BeginMap;
	out << YAML::Key << "categoria" << YAML::Value << "Infanteria";
	out << YAML::Key << "unidades" << YAML::Value << YAML::BeginSeq;
	
	// Unidades
	std::string nombres[] = {"Milicia", "Hombre de armas", "Espadachin de espada larga", "Espadachin de mandoble", "Campeon"};
	int ataques[] = {4, 6, 9, 11, 13};
	for (int i = 0; i < 5; i++) {
		out << YAML::BeginMap << YAML::Key << "nombre" << YAML::Value << nombres[i];
		out << YAML::Key << "ataque" << YAML::Value << ataques[i] << YAML::EndMap;
	}
	out << YAML::EndSeq << YAML::EndMap << YAML::EndSeq;
	
	ofstream outFile;
	outFile.open("test.yaml");
	outFile << out.c_str();
	outFile.close();
}

void testParsearYAML(){
	//TODO: Testear el parseo con el archivo que se genera en testEmitirYAML()
}

void testUsarLog(void){
	ErrorLog log;
	log.abrirLog();
	log.habilitarFlags(true,true,true);
	log.escribirLog("pruebaLog");
	log.cerrarLog();
}

void testSpritesheet(){
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{

				SDL_FillRect(gScreenSurface, NULL, 0x000000);

				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
				}

			

	//		
				Spritesheet mySprite("champion_move.png", 8, 10, 0, 0);
				mySprite.cambirCoordPantalla(100, 100);

				SDL_Rect rectangulo, rectDestino;
				SDL_SetColorKey( mySprite.devolverImagenAsociada(), true, SDL_MapRGB( mySprite.devolverImagenAsociada()->format, 4, 2, 4) );
				
				rectangulo.x =  mySprite.verOffsetX();
				rectangulo.y =  mySprite.verOffsetY();
				rectangulo.w = 330 /mySprite.getCols();
				rectangulo.h = 480/ mySprite.getRows();

				rectDestino.x = mySprite.verCoordXPantalla();
				rectDestino.y = mySprite.verCoordYPantalla();

				SDL_BlitSurface( mySprite.devolverImagenAsociada(), &rectangulo, gScreenSurface, &rectDestino );

				//Update the surface
				SDL_UpdateWindowSurface( gWindow );
			}
		}

	//Free resources and close SDL
	close();

}

int main( int argc, char** argv ){
	/*testEscenarioChico();
	testEscenarioGrande();
	
	//Test YAML
	testEmitirYAML();
	testParsearYAML();

	testUsarLog();
	getch();
	*/
	testSpritesheet();

	return 0;
}