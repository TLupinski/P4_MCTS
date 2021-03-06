#include "MCTS.h"

// Cr�er un nouveau noeud en jouant un coup � partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour cr�er la racine
Node * new_node (Node * parent, Play p)
{
	Node * node = (Node *)malloc(sizeof(Node));

	if ( parent != NULL) {
		node->state = copy_state(parent->state);
		play(node->state, p);
		node->play = p;
		node->player = AUTRE_JOUEUR(parent->player);
	}
	else {
		node->state = NULL;
		node->player = 0;
	}
	node->parent = parent;
	node->nb_childs = 0;

	// POUR MCTS:
	node->nb_wins = 0;
	node->nb_simus = 0;


	return node;
}

// Ajouter un enfant � un parent en jouant un coup
// retourne le pointeur sur l'enfant ajout�
Node * add_child(Node * parent, Play play) {
	Node * child = new_node(parent, play ) ;
	parent->childs[parent->nb_childs] = child;
	parent->nb_childs++;
	return child;
}

// Lib�re la m�moire d'un noeud et de tous ses enfants
void free_node (Node * node) {
	if ( node->state != NULL)
		free_state(node->state);

	while ( node->nb_childs > 0 ) {
		free_node(node->childs[node->nb_childs-1]);
		node->nb_childs --;
	}
	free(node);
}

//Fonction calculant avec UCB la valeur de chaque noeud et renvoyant un  des noeuds ayant la plus grande valeur.
Node * choose_best_UCB(Node * current)
{
    int i,nb_best_nodes = 0;
    Node * best_node[current->nb_childs];
    best_node[0] = NULL;
    double best_result = -INFINITY;
    for (i = 0; i < current->nb_childs; i++)
    {
        Node * node = current->childs[i];
        double node_result = (node->nb_wins/node->nb_simus) + C * sqrt((2.0 * log(current->nb_simus)) / node->nb_simus);
        if (node_result > best_result)
        {
            best_node[0] = node;
            nb_best_nodes = 1;
            best_result = node_result;
        } else if (node_result == best_result)
        {
            best_node[nb_best_nodes] = node;
            nb_best_nodes++;
        }
    }
    if (nb_best_nodes > 1)
    {
        time_t t;
        srand(time(&t));
        best_node[0] = best_node[rand()%nb_best_nodes];
    }
    return best_node[0];

}//Fonction prenant la valeur de chaque noeud et renvoyant un des noeuds ayant la plus grande valeur.
Node * choose_best_max(Node * current)
{
    int i,nb_best_nodes, player;
    Node * best_node[current->nb_childs];
    best_node[0] = NULL;
    player = current->player;
    double best_result = -INFINITY;
    for (i = 0; i < current->nb_childs; i++)
    {
        Node * node = current->childs[i];
        double node_result = fabs((1-player) - (node->nb_wins/node->nb_simus));
        if (node_result > best_result)
        {
            best_node[0] = node;
            nb_best_nodes = 1;
            best_result = node_result;
        } else if (node_result == best_result)
        {
            best_node[nb_best_nodes] = node;
            nb_best_nodes++;
        }
    }
    if (nb_best_nodes > 1)
    {
        time_t t;
        srand(time(&t));
        best_node[0] = best_node[rand()%nb_best_nodes];
    }
    return best_node[0];
}

Node * choose_best_robuste(Node * current)
{
    int i,nb_best_nodes, player;
    Node * best_node[current->nb_childs];
    best_node[0] = NULL;
    player = current->player;
    double best_result = -INFINITY;
    for (i = 0; i < current->nb_childs; i++)
    {
        Node * node = current->childs[i];
        double node_result = node->nb_simus;
        if (node_result > best_result)
        {
            best_node[0] = node;
            nb_best_nodes = 1;
            best_result = node_result;
        } else if (node_result == best_result)
        {
            best_node[nb_best_nodes] = node;
            nb_best_nodes++;
        }
    }
    if (nb_best_nodes > 1)
    {
        time_t t;
        srand(time(&t));
        best_node[0] = best_node[rand()%nb_best_nodes];
    }
    return best_node[0];
}

//Fonction pour agrandir l'arbre de recherche en ajoutant une nouvelle feuille
Node * expand(Node * Tree)
{
    Play * plays = get_valid_plays(Tree->state);
    Node * n = new_node(Tree,plays[Tree->nb_childs]);
    Tree->childs[Tree->nb_childs] = n;
    Tree->nb_childs++;
    free(plays);
    return n;
}

//Fonction de s�lection d'un noeud � explorer
Node * select(Node * Tree) {
    Node * current = Tree;
    while (final_state(current->state) == 0)
    {
        if (current->state->nb_successor > current->nb_childs)
        {
            return expand(current);
        }
        current = choose_best_UCB(current);
    }
    return current;
}

//Fonction simulant des coups al�atoires � partir d'un �tat donn� jusqu'� arriv� � un �tat final.
double simulate(Node * Tree)
{
    time_t t;
    int i, r;
    srand(time(&t));
    State * current = copy_state(Tree->state), * test;
    double result = final_state(current);
    while(result == 0)
    {
        Play * plays = get_valid_plays(current);
        for (i = 0; i < current->nb_successor; i++)
        {
            test = copy_state(current);
            play(test,plays[i]);
            result = final_state(test);
            if (result != 0)
            {
                int player = test->player;
                free(test);
                return player;
            }
            free(test);
        }
        r = rand()%current->nb_successor;
        play(current, plays[r]);
        result = final_state(current);
        free(plays);
    }
    int player = current->player;
    free(current);
    return player;
}

//Fonction permettant de mettre � jours le nombre de victoires/simulations dans un arbre � partir de la feuille explor�e.
void backtrack(Node * Tree, double value)
{
    Node * current = Tree;
    while(current != NULL)
    {
        current->nb_simus += 1.;
        current->nb_wins += fabs(value - current->player);
        current = current->parent;
    }
}

//fonction impl�mentant l'algorithm MCTS en l'appliquant pendant un temps [time_limit] et renvoyant le meilleur coup calcul�.
Play mcts_algorithm(State * start, time_t time_limit)
{
	clock_t tic, toc;
	tic = clock();

	int spent_time, iter = 0;
    Node * Tree;
    Tree = new_node((Node*)NULL, new_play(0));
    Tree->state = copy_state(start);
    Tree->player = Tree->state->player;
    do
    {
        Node * selected_node = select(Tree);
        double value = simulate(selected_node);
        backtrack(selected_node, value);

		toc = clock();
        spent_time = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( spent_time < time_limit);
    printf("Nombre d'iterations : %d\n",iter);
    Node * max = choose_best_max(Tree);
    Node * robuste = choose_best_robuste(Tree);
    printf("Pourcentage de victoire : %f/%f = %f\n",Tree->nb_wins,Tree->nb_simus,Tree->nb_wins/Tree->nb_simus);
    //printf("R�sultat \"Max\" :\n");
    print_state(max->state);
    //printf("R�sultat \"Robuste\" :\n");
    print_state(robuste->state);
    Play result = new_play(max->play.column);
    free_node(Tree);
    return result;
}
