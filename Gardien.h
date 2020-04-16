#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"

class Labyrinthe;

enum EtatGardien {patrouille, defense, attaque};

class Gardien : public Mover {
private:
	EtatGardien etat = patrouille;
	int distVision = 5;
	float vitesse = 0.5;

	void patrol();

public:
	Gardien (Labyrinthe* l, const char* modele, bool enDefense);

	void update (void);

	bool move (double dx, double dy);
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }
};

#endif
