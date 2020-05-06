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
	float potentielDefense = 1; //Initialisé le plus loin du trésor
	int pointDeVie = 3;
	bool mort = false;
	bool changeCase = false ;

	/** Mode Patrouille */
	void patrol();
	/** Mode Défense */
	void modeDefense();
	/** Scanne ses environs */
	void scout();
	/** Mode attaque */
	void attack();

public:
	Gardien (Labyrinthe* l, const char* modele, bool enDefense);

	void update (void);
	
	int caseProche(int x, int y);

	void majPotentielDefense(void);
	
	bool move (double dx, double dy);

	void fire (int angle_vertical);

	bool process_fireball (float dx, float dy);

	/** Le gardien a pris un dégat */
	void hit();
};

#endif
