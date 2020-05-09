#include "Chasseur.h"
#include "Gardien.h"
#include "Labyrinthe.h"
#include <cmath>
#include <ctime>
#include <algorithm>

/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	/** La case physique où il se trouve avant son déplacement */
	int preMoveI = (int) this->_x/Environnement::scale;
	int preMoveJ = (int) this->_y/Environnement::scale;

	/** La case physique où il se trouve après son déplacement */
	int postMoveI = (int) (this->_x+dx)/Environnement::scale;
	int postMoveJ = (int) (this->_y+dy)/Environnement::scale;


	if (this->_l->data(postMoveI, postMoveJ) == 1) {
		return false;
	} else {

		/** Vérification si le chasseur utilise un téléporteur */
		if (postMoveI!=preMoveI || postMoveJ !=preMoveJ) {

			Labyrinthe* lab = ((Labyrinthe*) _l);
			for (int i=0; i<lab->nPads; i++) {
				if (postMoveI == lab->pads[i].x && postMoveJ == lab->pads[i].y) {
					this->_x = lab->pads[i].sibling->x*Environnement::scale+2;
					this->_y = lab->pads[i].sibling->y*Environnement::scale+2;
				}
			}
		}

		this->_x += dx;
		this->_y += dy;

		return true;
	}
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;

	/** Position de la boule après le déplacement */
	int ballX = (this->_fb ->get_x() + dx) / Environnement::scale;
	int ballY = (this->_fb ->get_y() + dy) / Environnement::scale;

	// on bouge que dans le vide!
	if (EMPTY == _l -> data (ballX, ballY))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	// teste si on a touch� le tr�sor: juste pour montrer un exemple de la
	// fonction � partie_terminee �.
	if (ballX == _l->_treasor._x && ballY == _l->_treasor._y)
	{
		partie_terminee (true);
	}

	for (int i=1; i<this->_l->_nguards; i++) {
		Gardien* guard = ((Gardien*) this->_l->_guards[i]);

		int guardX = round(guard->_x/ Environnement::scale);
		int guardY = round(guard->_y/ Environnement::scale);


		if (ballX == guardX && ballY == guardY) {
			guard->hit();
		}
	}

	Labyrinthe* lab = ((Labyrinthe*) _l);

	for (int i=0; i<lab->nBWalls; i++) {
		if (ballX == lab->bWalls[i].x && ballY == lab->bWalls[i].y) {
			lab->breakWall(i);
		}
	}

	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
}

/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */

void Chasseur::right_click (bool shift, bool control) {
	// if (shift)
	// 	_l -> _guards [1] -> rester_au_sol ();
	// else
	// 	_l -> _guards [1] -> tomber ();
}

void Chasseur::hit() {
	if (this->pointDeVie > 0) {
		this->pointDeVie--;
		if (this->pointDeVie == 0) {
			partie_terminee(false);
		}
	}
	this->lastHit = time(NULL);
}

void Chasseur::update() {
	/** Gestion des points de vie */
	if (time(NULL) - this->lastHit > this->tempsRegen) {
		this->lastHit = time(NULL);
		this->pointDeVie = std::min(this->vieMax, this->pointDeVie+1);
	}
}