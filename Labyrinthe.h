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
	/* Distance la plus grande entre une case et un trésor*/
	float distanceMax;

	// retourne l'�tat (occupation) de la case (i, j).
	char data (int i, int j)
	{
		return _data [i][j];
	}

	// retourne la distance à la case
	int getDistanceTresor (int i, int j)
	{
		return treasor_distance [i][j];
	}

	/** Modifie la case [i,j] avec la valeur de c */
    void /* setData */setData(int i, int j, char c) {
        this->_data[i][j] = c;
    }
};

#endif
