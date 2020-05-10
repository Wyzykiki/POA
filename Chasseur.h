#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

/**
 * @file Chasseur.h
 * @brief Gère le personnage du chasseur qui est controlé par l'utilisateur
 */

class Labyrinthe;

class Chasseur : public Mover {
private:
	/**
	 * @brief Accepte ou non un deplacement.
	 * 
	 * @param dx : Coordonnée x du vecteur de déplacement 
	 * @param dy :Coordonnée y du vecteur de déplacement 
	 * @return true : Le chasseur se déplace
	 * @return false : Le chasseur ne se déplace pas
	 */
	bool move_aux (double dx, double dy);

	/** Gestion des points de vie */
	int vieMax = 15; // Le nombre maximum de points de vie que peut avoir le chasseur au cours d'une partie
	int pointDeVie = vieMax; //nombre de points de vie qu'à le chasseur au début
	int tempsRegen = 8; //Le nombre de seconde après lequel il peut récupérer un point de vie.
	int lastHit = 0; // Combien de seconde se sont écoulés depuis la dernière fois ou il s'est fait toucher.

public:
	/*
	 *	Le son...
	 */
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touch�.
	static Sound*	_wall_hit;		// on a tap� un mur.

	Chasseur (Labyrinthe* l);
	
	/**
	 * @brief S'il y a des obstacles permet de "glisser" sur eux, et de ne pas rester bloquer. 
	 * 
	 * @param dx Coordonnée x du vecteur de déplacement 
	 * @param dy Coordonnée y du vecteur de déplacement
	 * @return true : Le chasseur se déplace
	 * @return false : Le chasseur ne se déplace pas
	 */
	bool move (double dx, double dy) {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}

	/**
	 * @brief Mise à jour des points de vie du chasseur
	 */
	void update();


	/**
	 * @brief Gère le déplacement des boules de feu, et déclenche la fin du jeu, si une boule de feu atteint le trésor
	 * 
	 * @param dx Coordonnée x du vecteur de déplacement 
	 * @param dy Coordonnée y du vecteur de déplacement
	 * @return true La boule de feu est rentrée en collision avec quelque chose 
	 * @return false La boule de feu est entrain de se déplacer dans le vide
	 */
	bool process_fireball (float dx, float dy);

	/**
	 * @brief Déclenche l'initialisation d'une boule de feu
	 * 
	 * @param angle_vertical L'angle vers lequel il tire
	 */
	void fire (int angle_vertical);


	/**
	 * @brief Retire des points de vie au chasseur quand il est touché. S'il arrive à 0 point de vie, alors la partie est terminée.
	 * 
	 */
	void hit();
};

#endif
