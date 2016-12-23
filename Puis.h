#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#define LARGEUR_MAX 7
#define NB_COLONNES 7
#define NB_LIGNES 6
#define TIME_LIMIT 2

#define  VIDE ' '
#define ORDINATEUR 'x'
#define HUMAIN 'o'

#define TRUE 1
#define FALSE 0

#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))


typedef struct
{
    int dg;
    int vert;
    int dd;
    int horz;
}Case ;


typedef struct
{
    int column;
}Play;

Play new_play(int x);

bool same_play(Play p1, Play p2);

typedef struct
{
    char ** grille;
    int width, height, nb_successor;
    int player;

}State;

State * new_state(unsigned int nb_ligne, unsigned int nb_colonne);

State * copy_state(State * SP);

bool same_state(State * S1, State * S2);

int play(State * S, Play play);

Play * get_valid_plays(State * S);

bool valid_play(State * S, Play play);

State ** get_succesors(State * S);

double final_state(State * S);

void free_state(State * S);

void free_successors(State ** SUCC, int nb_successor);

void print_state(State * S);
