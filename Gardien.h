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
	bool fireBallActive = false;

	/** Mode Patrouille */
	void patrol();

	/** Scanne ses environs */
	void scout();
	/** Mode attaque */
	void attack();

public:
	Gardien (Labyrinthe* l, const char* modele, bool enDefense);

	void update (void);

	bool move (double dx, double dy);

	void fire (int angle_vertical);

	bool process_fireball (float dx, float dy);
};

#endif
