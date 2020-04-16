#include "Gardien.h"
#include <cmath>

#include <iostream>//TODO: remove

#define PI 3.14159265



Gardien::Gardien (Labyrinthe* l, const char* modele, bool enDefense) : Mover (120, 80, l, modele)
{
	srand (time(NULL));
	if (enDefense)
		this->etat = EtatGardien::defense;
}


void Gardien::patrol() {
	/** Le gardien s'est tourné sur ce tick */
	bool turned = false;

	/** Le déplacement du gardien en x et en y */
	double dx = - this->vitesse * sin(this->_angle*PI/180);
	double dy = this->vitesse * cos(this->_angle*PI/180);

	/** La case physique où il se trouve avant son déplacement */
	int preMoveI = (int) round((this->_x)/Environnement::scale);
	int preMoveJ = (int) round((this->_y)/Environnement::scale);

	/** La case physique où il se trouve après son déplacement */
	int postMoveI = (int) round((this->_x+dx)/Environnement::scale);
	int postMoveJ = (int) round((this->_y+dy)/Environnement::scale);

	/** S'il change de case physique mais qu'elle est occupé, il se tourne. 
	 * (le gardien avance ~1/10 de case, 
	 * s'il regarde sa case il doit continuer d'avancer)
	 */
	while ((postMoveI!=preMoveI || postMoveJ !=preMoveJ) && this->_l->data(postMoveI, postMoveJ) == 1) {
		this->_angle = rand() % 360;
		turned = true;
		dx = - this->vitesse * sin(this->_angle*PI/180);
		dy = this->vitesse * cos(this->_angle*PI/180);
		postMoveI = (int) round((this->_x+dx)/Environnement::scale);
		postMoveJ = (int) round((this->_y+dy)/Environnement::scale);
	}

	if (!turned)
		move(dx, dy);
}

void Gardien::update() {
	switch (this->etat) {
		case patrouille:
			this->patrol();
			break;
		case attaque:
			break;
		case defense:
			break;
	}

}

bool Gardien::move(double dx, double dy) {
	this->_l->data2((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale), 0);
	this->_x += dx;
	this->_y += dy;
	this->_l->data2((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale), 1);
	return true;
}