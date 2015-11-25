#include "GeneradorEscenarios.h"
#include <math.h>
#include <time.h>
#include <queue>


double rnd_percent(void) {
	return (rand()/(double)RAND_MAX);
}

int seed_elem(float strengh, float decay, pos_t init, float* density, tile_content* tiles, int size, tile_content elem) {
	if (tiles[init.x * size + init.y] != TILE_EMPTY)
		return 0;

	int ady = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			pos_t aux(0,0);
			aux.x = init.x + i;
			aux.y = init.y + j;
			if (aux.x >= 0 && aux.x < size && aux.y >= 0 && aux.x < size)
				if (tiles[aux.x * size + aux.y] != TILE_EMPTY && tiles[aux.x * size + aux.y] != elem)
					ady++;
		}
	}
	if (ady >= 3)
		return 0;

	int count = 0;
	queue<pos_t> cola = queue<pos_t>();
	cola.push(init);
	density[init.x * size + init.y] = strengh;
	tiles[init.x * size + init.y] = elem;

	while (!cola.empty()) {
		pos_t act = cola.front();
		cola.pop();

		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				pos_t aux(0,0);
				aux.x = act.x + i;
				aux.y = act.y + j;
				if (aux.x >= 0 && aux.x < size) {
					if (aux.y >= 0 && aux.y < size) {
						if (tiles[aux.x * size + aux.y] == TILE_EMPTY) {
							float rnd_decay = (rand()/(double)RAND_MAX);
							rnd_decay = rnd_decay * decay;
							float nStrengh = density[act.x * size + act.y] * rnd_decay;
							if (nStrengh > 1) {
								cola.push(aux);
								density[aux.x * size + aux.y] = nStrengh;
								tiles[aux.x * size + aux.y] = elem;
								count++;
							}
						}
					}
				}
			}
		}
	}
	return count;
}

int seed_population(float* density, tile_content* tiles, int size, tile_content elem, seeding_param params) {
	int total_spawns = 0;
	pos_t pos(0,0);
	if (params.count <= 0)
		return 0;

	float act_strengh;
	float act_decay;

	for (int i = 0; i < params.count; i++) {
		bool placed = false;
		while (!placed) {
			pos.x = (size) * rnd_percent();
			pos.y = (size) * rnd_percent();
			if (tiles[pos.x * size + pos.y] == TILE_EMPTY) {
				act_strengh = rnd_percent() * params.strengh_delta + params.decay_mean;
				act_decay = rnd_percent() * params.decay_delta + params.decay_mean;
				total_spawns += seed_elem(act_strengh, act_decay, pos, density, tiles, size, elem);
				placed = true;
			}
		}
	}
	return total_spawns;
}

void reserve_space(tile_content* tiles, float* density, int size, int cant_jugadores) {
	if (cant_jugadores < 0 || cant_jugadores > MAX_JUGADORES)
		return;

	int area = 8;

	pos_t player1(0,0);
	pos_t player2(size - area, size - area);
	pos_t player3(size - area, 0);
	pos_t player4(0, size - area);

	pos_t tiles_iniciales[MAX_JUGADORES] = {player1, player2, player3, player4};

	for (int i = 0; i < cant_jugadores; i++) {
		for (int j = 0; j < area; j++) {
			for (int k = 0; k < area; k++) {
				pos_t aux = tiles_iniciales[i];
				aux.x += j;
				aux.y += k;
				tiles[aux.x * size + aux.y] = TILE_PLAYER;
			}
		}
	}

	for (int i = 0; i < size/2; i++) {
		pos_t aux1(i, i);
		pos_t aux2(i+1, i);
		pos_t aux3(size - 1 - i, i);
		pos_t aux4(size - 2 - i, i);
		pos_t aux5(i, size - 1 - i);
		pos_t aux6(i+1, size - 1 - i);
		pos_t aux7(size - 1 - i, size - 1 - i);
		pos_t aux8(size - 2 - i, size - 1 - i);
		pos_t posiciones[8] = {aux1, aux2, aux3, aux4, aux5, aux6, aux7, aux8};
		for (int j = 0; j < 8; j++) {
			tiles[posiciones[j].x * size + posiciones[j].y] = TILE_PLAYER;
		}
	}

	int dist = 3;
	pos_t food1(dist + area, dist);
	pos_t gold1(dist, dist + area);
	seed_elem(4, 0.7, food1, density, tiles, size, TILE_FOOD);
	seed_elem(3, 0.6, gold1, density, tiles, size, TILE_GOLD);

	pos_t food2(dist, size - area - dist);
	pos_t gold2(dist + area, size - dist);
	seed_elem(4, 0.7, food2, density, tiles, size, TILE_FOOD);
	seed_elem(3, 0.6, gold2, density, tiles, size, TILE_GOLD);
	
	pos_t food3(size - area - dist, dist);
	pos_t gold3(size - dist, dist + area);
	seed_elem(4, 0.7, food3, density, tiles, size, TILE_FOOD);
	seed_elem(3, 0.6, gold3, density, tiles, size, TILE_GOLD);

	pos_t food4(size - area - dist, size - dist);
	pos_t gold4(size - dist, size - area - dist);
	seed_elem(4, 0.7, food4, density, tiles, size, TILE_FOOD);
	seed_elem(3, 0.6, gold4, density, tiles, size, TILE_GOLD);
}

void install_players(tile_content* tiles, int size, int cant_jugadores) {
	
	int area = 7;
	pos_t player1(1,1);
	pos_t player2(size - area + 1, size - area + 1);
	pos_t player3(size - area + 1, 1);
	pos_t player4(1, size - area + 1);
	
	pos_t tiles_iniciales[MAX_JUGADORES] = {player1, player2, player3, player4};

	pos_t flag1(area - 1, 1);
	pos_t flag2(size - area, size - 2);
	pos_t flag3(size - area, 1);
	pos_t flag4(area - 1, size - 2);
	
	pos_t flags_iniciales[MAX_JUGADORES] = {flag1, flag2, flag3, flag4};

	pos_t settler1(2, 6);
	pos_t settler2(size - area + 2, size - area);
	pos_t settler3(size - area + 2, 6);
	pos_t settler4(2, size - area);
	
	pos_t settler_iniciales[MAX_JUGADORES] = {settler1, settler2, settler3, settler4};

	for (int k = 0; k < cant_jugadores; k++){	
		/*
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				tiles[(tiles_iniciales[k].x + i) * size + tiles_iniciales[k].y + j] = tile_content(TILE_TOWN_CENTRE1 + k);
			}
		}
		*/
		tiles[(tiles_iniciales[k].x) * size + tiles_iniciales[k].y] = tile_content(TILE_TOWN_CENTRE1 + k);
		for (int i = 0; i < 3; i++) {
			tiles[(settler_iniciales[k].x + i) * size + settler_iniciales[k].y] = tile_content(TILE_SETTLER1 + k);
		}
		tiles[(flags_iniciales[k].x) * size + flags_iniciales[k].y] = tile_content(TILE_CRIT_UNIT1 + k);
	}
}

tile_content* generarEscenario(int size, int cant_jugadores) {
	if (size < MIN_SIZE || size > MAX_SIZE)
		return nullptr;

	if (cant_jugadores < MIN_JUGADORES || cant_jugadores > MAX_JUGADORES)
		return nullptr;

	float* density = new float[size * size];
	if (!density)
		return nullptr;

	tile_content* tiles = new tile_content[size * size];
	if (!tiles) {
		delete[] density;
		return nullptr;
	}
	
	for (int i = 0; i < size*size; i++) {
		density[i] = 0;
		tiles[i] = TILE_EMPTY;
	}
	
	srand(time(NULL));

	reserve_space(tiles, density, size, cant_jugadores);

	seeding_param forest_params = seeding_param(15 * size * size/2500, 40, 15, 0.77, 0.05);
	seeding_param food_params = seeding_param(20 * size * size/2500, 1, 5, 0.6, 0);
	seeding_param gold_params = seeding_param(15 * size * size/2500, 1, 5, 0.6, 0.05);
	seeding_param stone_params = seeding_param(10 * size * size/2500, 1, 5, 0.6, 0.05);
	
	seed_population(density, tiles, size, TILE_FOOD, food_params);
	seed_population(density, tiles, size, TILE_TREE, forest_params);
	seed_population(density, tiles, size, TILE_GOLD, gold_params);
	seed_population(density, tiles, size, TILE_STONE, stone_params);
	
	install_players(tiles, size, cant_jugadores);

	delete[]density;
	return tiles;
}
