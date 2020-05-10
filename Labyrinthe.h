#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

/**
 * @file Labyrinthe.h
 * @brief Permet de créer notre labyrinthe et d'initialiser toutes nos variables par rapport à notre fichier
 * 
 */

//Structure permettant de gérer la téléportation
struct TeleportPad {
	char id;
	int x;
	int y;
	TeleportPad* sibling;
};

//Structure permettant de gérer les murs cassables
struct BreakableWall {
	int x;
	int y;
	int pictIndex;
	bool broken = false;
};

class Labyrinthe : public Environnement {
private:
	char	**_data;	// indique si la case est libre ou occup�e.
	int		lab_width;	// dimensions du rectangle.
	int		lab_height;	// englobant le labyrinthe.
	
	/** Matrice des distances au trésor */
	int** treasor_distance;

	/** Dernière position connue du chasseur */
	int lastPos[3] = {-1, -1, -1};

public:
	Labyrinthe (char*);

	int width () { return lab_width;}	// retourne la largeur du labyrinthe.
	int height () { return lab_height;}	// retourne la longueur du labyrinthe.
	
	/** Les socles de téléportations */
	TeleportPad* pads;
	int nPads;

	/** Les murs cassables */
	BreakableWall* bWalls;
	int nBWalls;

	/* Distance la plus grande entre une case et un trésor*/
	float distanceMax;

	// retourne l'�tat (occupation) de la case (i, j).
	char data (int i, int j)
	{
		return _data [i][j];
	}

	/** Modifie la case [i,j] avec la valeur de c */
	void setData(int i, int j, char c) {
		this->_data[i][j] = c;
	}
	//Recupère la dernière position
	int* getLastPos() {
		return this->lastPos;
	}

	//Met à jour la dernière position dans lequel il a été
	void updateLastPos(int x, int y, int alpha) {
		this->lastPos[0] = x;
		this->lastPos[1] = y;
		this->lastPos[2] = alpha;
	}
	// retourne la distance à la case
	int getDistanceTresor (int i, int j)
	{
		return treasor_distance [i][j];
	}

	/** Casse le i-ème mur */
	void breakWall(int index);
};

#endif
