#ifndef INCLUDE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
#include "mcts.h"
#endif

Play new_play(int x)
{
    Play P;
    P.column = x;
    return P;
}

bool same_play(Play p1, Play p2)
{
    return (p1.column ==p2.column);
}

State * new_state(unsigned int nb_ligne, unsigned int nb_colonne)
{
    State * S;
    S = (State *) malloc(sizeof(State));
    S->grille = (char **)malloc(nb_colonne*sizeof(*S->grille));
    S->width = nb_colonne;
    S->height = nb_ligne;
    S->nb_successor = nb_colonne;
    S->player = 1;
    int i, j;
    for (i = 0; i < S->width; i++)
    {
        S->grille[i] = (char *)malloc(nb_ligne*sizeof(**S->grille));
        for (j = 0; j < S->height; j++)
        {
            S->grille[i][j] = VIDE;
        }
    }
    return S;
}

State * copy_state(State * SP)
{
    State * S;
    S = (State *)malloc(sizeof(SP)+1);
    S->width = SP->width;
    S->height = SP->height;
    S->nb_successor = SP->nb_successor;
    S->player = SP->player;
    S->grille = (char **)malloc(S->width*sizeof(*S->grille));
    int i, j;
    for (i = 0; i < S->width; i++)
    {
        S->grille[i] = (char *)malloc(S->height*sizeof(**S->grille));
        for (j = 0; j < S->height; j++)
        {
            S->grille[i][j] = SP->grille[i][j];
        }
    }
    for (i = 0; i < S->width; i++)
    {
        for (j = 0; j < S->height; j++)
        {
            S->grille[i][j] = SP->grille[i][j];
        }
    }
    return S;
}

bool same_state(State * S1, State * S2)
{
    bool result;
    result = true;
    if (S1->width == S2->width && S1->height == S2->height)
    {
        int i,j;
        for (i = 0; i < S1->width && result; i++)
        {
            for (j = 0; j < S1->height && result; j++)
            {
                if (S1->grille[i][j] != S2->grille[i][j])
                {
                    result = false;
                }
            }
        }
    } else {
        result = false;
    }
    return result;
}

Play * get_valid_plays(State * S)
{
    int i,k = 0;
    Play enum_plays[S->width];
    for (i = 0; i< S->width; i++)
    {
        Play play = (new_play(i));
        if (valid_play(S, play))
        {
            enum_plays[k] = play;
            k++;
        }
    }
    Play * result;
    if (k == 0)
    {
        result = (Play *) malloc(sizeof(Play));
        return result;
    }
    result = (Play *) malloc(k * sizeof(Play)+1);
    for (i = 0; i < k; i++)
    {
        result[i] = enum_plays[i];
    }
    S->nb_successor = k;
    return result;
}

bool valid_play(State * S, Play play)
{
    int i ;
    for (i = 0; i < S->height; i++)
    {
        if (S->grille[play.column][i] == ' ')
        {
            return true;
        }
    }
    return false;
}
int play(State * S, Play play)
{
    if (0 <= play.column && play.column < S->width)
    {
        int j;
        for (j = 0; j < S->height; j++)
        {
         if (S->grille[play.column][j] == ' ')
         {
             if (S->player == 0)
             {
                S->grille[play.column][j] = HUMAIN;
             } else
             {
                S->grille[play.column][j] = ORDINATEUR;
             }
            S->player = AUTRE_JOUEUR(S->player);
            return 0;
         }
        }
    }
    return -1;
}

State ** get_succesors(State * S)
{
    int i,k = 0;
    State * enum_state[S->width];
    for (i = 0; i< S->width; i++)
    {
            State * SP;
            SP = copy_state(S);
            if (play(SP,new_play(i)) != -1)
            {
                enum_state[k] = SP;
                k++;
            }
    }
    State ** result;
    if (k == 0)
    {
        result = (State **) malloc(2 *sizeof(State *));
        return result;
    }
    result = (State **) malloc(k * sizeof(State *));
    for (i = 0; i < k; i++)
    {
        result[i] = enum_state[i];
    }
    S->nb_successor = k;
    return result;
}

double final_state(State * S)
{

    Case game[S->width][S->height];
    int ligne, colonne, nb_cases_full = 0;
    for ( ligne = 0; ligne < S->height ; ligne++)
    {
        for ( colonne = 0; colonne < S->width; colonne++)
        {
            /*
            Par defaut il y a 1 pion sur 4 d'aligner correctement
            */
            game[colonne][ligne].horz=1;
            game[colonne][ligne].vert=1;
            game[colonne][ligne].dg=1;
            game[colonne][ligne].dd=1;
            if (S->grille[colonne][ligne] != VIDE)
            {
                nb_cases_full++;
            }
        }
    }

    // pour le horizontal
    for ( ligne = 0; ligne < S->height ; ligne++)
    {
        for ( colonne = 1; colonne < S->width; colonne++)
        {
            if ((S->grille[colonne][ligne]==S->grille[colonne-1][ligne])&&(S->grille[colonne][ligne]!=VIDE))
                // meme Element du S->grille
            {
                game[colonne][ligne].horz=game[colonne-1][ligne].horz+1;
                if (game[colonne][ligne].horz==4)
                    return 1;
            }
        }
    }
    // pour le vertical
    for ( ligne = 1; ligne < S->height ; ligne++)
    {
        for ( colonne = 0; colonne < S->width; colonne++)
        {
            if ((S->grille[colonne][ligne]==S->grille[colonne][ligne-1])&&(S->grille[colonne][ligne]!=VIDE))
                // meme couleur
            {
                game[colonne][ligne].vert=game[colonne][ligne-1].vert+1;
                if (game[colonne][ligne].vert==4)
                    return 1;
            }
        }
    }
    // pour le diagonal gauche:de bas à gauche vers haut à droite
    for ( ligne = 1; ligne < S->height ; ligne++)
    {
        for ( colonne = 0; colonne < S->width-1; colonne++)
        {
            if ((S->grille[colonne][ligne]==S->grille[colonne+1][ligne-1])&&(S->grille[colonne][ligne]!=VIDE))
                // meme couleur
            {
                game[colonne][ligne].dg=game[colonne+1][ligne-1].dg+1;
                if (game[colonne][ligne].dg==4)
                    return 1;
            }
        }
    }
    // pour le diagonal droite:de haut à gauche vers bas à droite
    for ( ligne = 1; ligne < S->height ; ligne++)
    {
        for ( colonne = 1; colonne < S->width; colonne++)
        {
            if ((S->grille[colonne][ligne]==S->grille[colonne-1][ligne-1])&&(S->grille[colonne][ligne]!=VIDE))
                // meme couleur
            {
                game[colonne][ligne].dd=game[colonne-1][ligne-1].dd+1;
                if (game[colonne][ligne].dd==4)
                    return 1;
            }
        }
    }
    if (S->height * S->width == nb_cases_full)
    {
        return 0.5;
    }
    return 0;
}

void free_state(State * S)
{
    int i;
    for (i = 0; i < S->width; i++)
    {
        free(S->grille[i]);
    }
    //free(S);
}

void free_successors(State ** SUCC, int nb_successor)
{
    int i;
    for (i = 0; i < nb_successor; i++)
    {
        free_state(SUCC[i]);
    }
    free(SUCC);
}


void print_state(State * S)
{
    int w, h;

    printf("|");
    for(w = 0; w < S->width; w++)
        printf(" %d ", w+1);  // on affiche le numero de la colonne
    printf("|\n");

    printf("|");
    for(w = 0; w < S->width; w++)
        printf("___"); // une ligne 'souligné' pour dessiner le cadre
    printf("|\n");

    for(h = S->height-1 ; h >= 0; h--) // on affiche la ligne du haut en haut et on descend pour avoir l'afficgage dans le bon sens
    {
        printf("|");
        for(w = 0; w < S->width; w++)
        {
            printf(" %c ", S->grille[w][h]);
        }
        printf("|\n");
    }
    printf("|");
    for(w = 0; w < S->width; w++)
        printf("___"); // une ligne 'souligné' pour dessiner le cadre
    printf("|\n");
    printf("FIN AFFICHAGE\n");
}

Play joueur1IA(State * S)
{
    //qui commence le jeu
   /* printf("Qui commence (0 : HUMAIN, 1 : ORDINATEUR) ? ");
	scanf("%d", &(etat->joueur) ); */

    int col=1;
    printf("JOUEUR: %s - quelle colonne voulez vous jouer?\n" , S->player == FALSE ? "ORDINATEUR" : "HUMAIN");
    scanf("%d", &col);
    return new_play(col - 1);
}

Play joueur2IA(State * S)
{
    return mcts_algorithm(S, TIME_LIMIT);
}

int coupvalide(char grille[NB_COLONNES][NB_LIGNES], int colonne) // numero entre 0 et 6 indiquant la colonne jouée
{
    int y;
    int lignefree=-1; // contient le numero de ligne de la case qui est libre pour cette colonne, -1 si la colonne est pleine

    colonne -=1; // pour passer de 1..NB_COLONNES à 0..NB_COLONNES-1

    if (colonne < 0 || colonne >= NB_COLONNES)
    {
        return FALSE; // si le numero de colonne est en dehors des bornes, le coup n'est pas valide
    }

    for ( y = NB_LIGNES-1; y >=0 ; y--)
    {
        if (grille[colonne][y] == VIDE)
            lignefree = y;
    }

    if (lignefree== -1)
    {
        return FALSE; // si la colonne est pleine, le coup n'est pas valide
    }
    return TRUE;
}

void Init(char grille[NB_COLONNES][NB_LIGNES])
{
    int w, h;
    for(w = 0; w < NB_COLONNES; w++)
    {
        for(h = 0; h < NB_LIGNES; h++)
        {
            grille[w][h]=VIDE; // on initialise la tableau pour qu'il soit vide au debut
        }
    }
}

void AfficheGrille(char tableau[NB_COLONNES][NB_LIGNES])
{
    int w, h;

    printf("|");
    for(w = 0; w < NB_COLONNES; w++)
        printf(" %d ", w+1);  // on affiche le numero de la colonne
    printf("|\n");

    printf("|");
    for(w = 0; w < NB_COLONNES; w++)
        printf("___"); // une ligne 'souligné' pour dessiner le cadre
    printf("|\n");

    for(h = NB_LIGNES-1 ; h >= 0; h--) // on affiche la ligne du haut en haut et on descend pour avoir l'afficgage dans le bon sens
    {
        printf("|");
        for(w = 0; w< NB_COLONNES; w++)
        {
            printf(" %c ", tableau[w][h]);
        }
        printf("|\n");
    }
    printf("|");
    for(w = 0; w < NB_COLONNES; w++)
        printf("___"); // une ligne 'souligné' pour dessiner le cadre
    printf("|\n");
}

int check(char grille[NB_COLONNES][NB_LIGNES])
/*
    Principe:
    Additioner le contenu de la case adjacente correcte
    au contenu de la case courante.
*/

{
    Case game[NB_COLONNES][NB_LIGNES];
    int ligne, colonne;
    for ( ligne = 0; ligne < NB_LIGNES ; ligne++)
    {
        for ( colonne = 0; colonne < NB_COLONNES; colonne++)
        {
            /*
            Par defaut il y a 1 pion sur 4 d'aligner correctement
            */
            game[colonne][ligne].horz=1;
            game[colonne][ligne].vert=1;
            game[colonne][ligne].dg=1;
            game[colonne][ligne].dd=1;
        }
    }

    // pour le horizontal
    for ( ligne = 0; ligne < NB_LIGNES ; ligne++)
    {
        for ( colonne = 1; colonne < NB_COLONNES; colonne++)
        {
            if ((grille[colonne][ligne]==grille[colonne-1][ligne])&&(grille[colonne][ligne]!=VIDE))
                // meme Element du grille
            {
                game[colonne][ligne].horz=game[colonne-1][ligne].horz+1;
                if (game[colonne][ligne].horz==4)
                    return TRUE;
            }
        }
    }
    // pour le vertical
    for ( ligne = 1; ligne < NB_LIGNES ; ligne++)
    {
        for ( colonne = 0; colonne < NB_COLONNES; colonne++)
        {
            if ((grille[colonne][ligne]==grille[colonne][ligne-1])&&(grille[colonne][ligne]!=VIDE))
                // meme couleur
            {
                game[colonne][ligne].vert=game[colonne][ligne-1].vert+1;
                if (game[colonne][ligne].vert==4)
                    return TRUE;
            }
        }
    }
    // pour le diagonal gauche:de bas à gauche vers haut à droite
    for ( ligne = 1; ligne < NB_LIGNES ; ligne++)
    {
        for ( colonne = 0; colonne < NB_COLONNES-1; colonne++)
        {
            if ((grille[colonne][ligne]==grille[colonne+1][ligne-1])&&(grille[colonne][ligne]!=VIDE))
                // meme couleur
            {
                game[colonne][ligne].dg=game[colonne+1][ligne-1].dg+1;
                if (game[colonne][ligne].dg==4)
                    return TRUE;
            }
        }
    }
    // pour le diagonal droite:de haut à gauche vers bas à droite
    for ( ligne = 1; ligne < NB_LIGNES ; ligne++)
    {
        for ( colonne = 1; colonne < NB_COLONNES; colonne++)
        {
            if ((grille[colonne][ligne]==grille[colonne-1][ligne-1])&&(grille[colonne][ligne]!=VIDE))
                // meme couleur
            {
                game[colonne][ligne].dd=game[colonne-1][ligne-1].dd+1;
                if (game[colonne][ligne].dd==4)
                    return TRUE;
            }
        }
    }
    return FALSE;
}

void joue(int player, char grille[NB_COLONNES][NB_LIGNES], int colonne )
/*
  colonne entre 1 et 7 inclus
*/
{
    int lignefree=-1; // contient le numero de ligne de la case qui est libre pour cette colonne, -1 si la colonne est pleine
    int y;
    colonne -=1;

    if (colonne < 0 || colonne >= NB_COLONNES)  // ne devrait pas arriver à ce stade si coupvalide() a été appelé avant cette fonction
    {
        return ; // si le numero de colonne est en dehors des bornes, le coup n'est pas valide
    }

    for ( y = NB_LIGNES-1; y >=0 ; y--)
    {
        if (grille[colonne][y] == VIDE)
            lignefree = y;
    }
    if (lignefree== -1) // ne devrait pas arriver a ce stade si coupvalide() a été appelé avant cette fonction
    {
        return;
    }

    grille[colonne][lignefree]= (player == FALSE) ? ORDINATEUR:HUMAIN;
    printf("%s a joue grille[%d][%d]\n", (player == FALSE) ? "ORDINATEUR":"HUMAIN", colonne+1, lignefree+1);
}

Play demande(State * S)
{
    Play play;
    printf("JOUEUR: %s - A votre tour...\n" , S->player == FALSE ? "ORDINATEUR" : "HUMAIN");
    switch (S->player)
    {
        case TRUE:
            play = joueur1IA(S);
            break;
        case FALSE:
            play = joueur2IA(S);
            break;
    }
    printf("JOUEUR: %s - joue colonne %d\n" , S->player == FALSE ? "ORDINATEUR" : "HUMAIN", play.column + 1);
    return play;
}

int main (int argc, char *argv[])
{
    Play play_chosen;
    int gameover = FALSE;
    int nbcoups = 0;
    State * S = new_state(NB_LIGNES, NB_COLONNES);
    print_state(S);

    while (!gameover)
    {
        do
        {
            play_chosen = demande(S);
            print_state(S);
            printf("L'ordinateur veut jouer %d\n", play_chosen.column);
            printf("LA fonction dit que %d\n", valid_play(S,play_chosen));
        }while(valid_play(S,play_chosen)==FALSE); // tant qu'il n'est pas possible de jouer là, on redemande
        play(S, play_chosen);
        printf("Joueur %s a joue:\n", S->player == FALSE ? "ORDINATEUR" : "HUMAIN");
        print_state(S);
        nbcoups++;
        if (final_state(S)==TRUE)
        {
            gameover = TRUE;
            printf("Victoire de %s au %dieme coups!", S->player == FALSE ? "ORDINATEUR" : "HUMAIN", nbcoups);
        }
        else if (nbcoups == NB_COLONNES*NB_LIGNES)
        {
            gameover = TRUE;
            printf("Match NUL (%d coups)\n", nbcoups);
        }
    }
    return 0;
}





