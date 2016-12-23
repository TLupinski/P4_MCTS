#include "MCTS.h"

// Créer un nouveau noeud en jouant un coup à partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
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

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté
Node * add_child(Node * parent, Play play) {
	Node * child = new_node(parent, play ) ;
	parent->childs[parent->nb_childs] = child;
	parent->nb_childs++;
	return child;
}

// Libère la mémoire d'un noeud et de tous ses enfants
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
    int i,nb_best_nodes;
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
Node * choose_best(Node * current)
{
    int i,nb_best_nodes;
    Node * best_node[current->nb_childs];
    best_node[0] = NULL;
    double best_result = -INFINITY;
    for (i = 0; i < current->nb_childs; i++)
    {
        Node * node = current->childs[i];
        double node_result = (node->nb_wins/node->nb_simus);
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

//Fonction de sélection d'un noeud à explorer
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

//Fonction simulant des coups aléatoires à partir d'un état donné jusqu'à arrivé à un état final.
double simulate(Node * Tree)
{
    time_t t;
    int r;
    srand(time(&t));
    State * current = copy_state(Tree->state);
    double result = final_state(current);
    while(result == 0)
    {
        Play * plays = get_valid_plays(current);
        r = rand()%current->nb_successor;
        play(current, plays[r]);
        result = final_state(current);
        free(plays);
    }
    return current->player;
}

//Fonction permettant de mettre à jours le nombre de victoires/simulations dans un arbre à partir de la feuille explorée.
void backtrack(Node * Tree, double value)
{
    Node * current = Tree;
    while(current != NULL)
    {
        current->nb_simus++;
        current->nb_wins += fabs(value - current->player);
        current = current->parent;
    }
}

//fonction implémentant l'algorithm MCTS en l'appliquant pendant un temps [time_limit] et renvoyant le meilleur coup calculé.
Play mcts_algorithm(State * start, time_t time_limit)
{
	clock_t tic, toc;
	tic = clock();
	int spent_time, iter = 0;

    Node * Tree;
    Tree = new_node((Node*)NULL, new_play(0));
    int i;
    Tree->state = copy_state(start);
    do
    {
        Node * selected_node = select(Tree);
        double value = simulate(selected_node);
        backtrack(selected_node, value);

		toc = clock();
        spent_time = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( spent_time < time_limit);
    Node * best = choose_best(Tree);
    Play result = new_play(best->play.column);
    free_node(Tree);
    return result;
}
