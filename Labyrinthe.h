#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

class Labyrinthe : public Environnement {
private:
	char	**_data;	// indique si la case est libre ou occup�e.
	int		lab_width;	// dimensions du rectangle.
	int		lab_height;	// englobant le labyrinthe.
	
	/** Matrice des distances au trésor */
	int** treasor_distance;

public:
	Labyrinthe (char*);
	int width () { return lab_width;}	// retourne la largeur du labyrinthe.
	int height () { return lab_height;}	// retourne la longueur du labyrinthe.
	// retourne l'�tat (occupation) de la case (i, j).
	char data (int i, int j)
	{
		return _data [i][j];
	}
};

#endif
