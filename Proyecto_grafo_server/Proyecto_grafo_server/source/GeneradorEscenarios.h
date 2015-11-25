#ifndef GENERADOR_H
#define GENERADOR_H

#include <string>

using namespace std;

const int MAX_JUGADORES = 4;
const int MIN_JUGADORES = 1;
const int MIN_SIZE = 30;
const int MAX_SIZE = 150;

enum tile_content {
	TILE_EMPTY,
	TILE_PLAYER,
	TILE_TREE,
	TILE_FOOD,
	TILE_GOLD,
	TILE_STONE,
	TILE_TOWN_CENTRE1,
	TILE_TOWN_CENTRE2,
	TILE_TOWN_CENTRE3,
	TILE_TOWN_CENTRE4,
	TILE_SETTLER1,
	TILE_SETTLER2,
	TILE_SETTLER3,
	TILE_SETTLER4,
	TILE_CRIT_UNIT1,
	TILE_CRIT_UNIT2,
	TILE_CRIT_UNIT3,
	TILE_CRIT_UNIT4
};


const std::string tile_content_entity[] = {"unknown", "unknown", "tree", "forage bush", "gold mine", "stone mine", "town center", 
									"town center", "town center", "town center", "villager", "villager", "villager", "villager",
									"unknown", "unknown", "unknown", "unknown"};
const unsigned int tile_content_owner[] = {999, 999, 0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};

struct pos_t {
	int x, y;
	
	pos_t(int _x, int _y) {
		x = _x;
		y = _y;
	}
};

struct seeding_param {
	int count;
	float strengh_mean;
	float strengh_delta;
	float decay_mean;
	float decay_delta;

	seeding_param(int _count, float _strengh_mean, float _strengh_delta, float _decay_mean, float _decay_delta) {
		count = _count;
		strengh_mean = _strengh_mean;
		strengh_delta = _strengh_delta;
		decay_mean = _decay_mean;
		decay_delta = _decay_delta;
	}
};

tile_content* generarEscenario(int size, int cant_jugadores);

#endif //GENERADOR_H