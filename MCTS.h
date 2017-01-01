#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
#include "Puis.h"

#define C 1.44

typedef struct s_Node
{
	int player; // joueur qui a joué pour arriver ici
	Play play;   // coup joué par ce joueur pour arriver ici

	State * state; // etat du jeu

	struct s_Node * parent;
	struct s_Node * childs[LARGEUR_MAX]; // liste d'enfants : chaque enfant correspond à un coup possible
	int nb_childs;	// nb d'enfants présents dans la liste

	// POUR MCTS:
	double nb_wins;
	double nb_simus;

} Node;

Node * new_node (Node * parent, Play p);

Node * add_child(Node * parent, Play play);

void free_node (Node * node);

Node * choose_best_UCB(Node * current);

Node * choose_best_max(Node * current);

Node * choose_best_robuste(Node * current);

Node * expand(Node * Tree);

Node * select(Node * Tree);

double simulate(Node * Tree);

Play mcts_algorithm(State * start, time_t time_limit);
