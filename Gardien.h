#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
/**
 * @file Gardien.h
 * @brief Permet de switcher entre les différents états du gardien.
 * 
 */
class Labyrinthe;

enum EtatGardien {patrouille, defense, attaque};

class Gardien : public Mover {
private:
	EtatGardien etat = patrouille; // Par défaut le gardien est en mode patrouille au début du jeu
	int distVision = 5; //La distance de vision qu'a le gardien autours de lui 
	float vitesse = 0.5; // La vitesse à laquelle il se déplace
	bool fireBallActive = false;
	float potentielDefense = 1; //Initialisé le plus loin du trésor
	bool mort = false; // Permet de savoir si le gardien est toujours en vie
	bool changeCase = false ; //Permet de savoir si le gardien va changer de case au prochain pas de temps
	
	/** Gestion des points de vie */
	int vieMax = 3; // Le nombre maximum de points de vie que peut avoir le gardien au cours d'une partie
	int pointDeVie = vieMax; //nombre de points de vie qu'à le gardien au début
	int tempsRegen = 8; //Le nombre de seconde après lequel il peut récupérer un point de vie.
	int lastHit = 0; // Combien de seconde se sont écoulés depuis la dernière fois ou il s'est fait toucher.


	//Définition des différents mode du gardien
	/** Mode Patrouille */
	void patrol();
	/** Mode Défense */
	void modeDefense();
	/** Scanne ses environs */
	void scout();
	/** Mode attaque */
	void attack();

	/**
	 * @brief Accepte ou non un deplacement.
	 * 
	 * @param dx : Coordonnée x du vecteur de déplacement 
	 * @param dy :Coordonnée y du vecteur de déplacement 
	 * @return true : Le chasseur se déplace
	 * @return false : Le chasseur ne se déplace pas
	 */
	bool move_aux(double dx, double dy);

public:
	Gardien (Labyrinthe* l, const char* modele, bool enDefense);

	/**
	 * @brief Mise à jour des points de vie du gardien
	 */
	void update (void);
	
	/**
	 * @brief Permet de déterminer l'angle de la case qui se rapproche le plus près du trésor.
	 * 
	 * @param x la case physique x ou il se trouve
	 * @param y la case physique y ou il se trouve
	 * @return int: l'angle vers lequel il doit se diriger
	 */
	int caseProche(int x, int y);

	/**
	 * @brief Vérifie qu'il y a assez ou pas trop de gardien en mode défense pour protéger le trésor. Et modifie si besoin l'état du gardien.
	 */
	void majPotentielDefense(void);
	
	/**
	 * @brief S'il y a des obstacles permet de "glisser" sur eux, et de ne pas rester bloquer. 
	 * 
	 * @param dx Coordonnée x du vecteur de déplacement 
	 * @param dy Coordonnée y du vecteur de déplacement
	 * @return true : Le chasseur se déplace
	 * @return false : Le chasseur ne se déplace pas
	 */
	bool move (double dx, double dy);

	/**
	 * @brief Déclenche l'initialisation d'une boule de feu
	 * 
	 * @param angle_vertical L'angle vers lequel il tire
	 */
	void fire (int angle_vertical);

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
	 * @brief Retire des points de vie au gardien quand il est touché. 
	 * 
	 */
	void hit();
};

#endif
