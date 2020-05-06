#include "Gardien.h"
#include "Labyrinthe.h"
#include "Chasseur.h"
#include <cmath>
#include <iostream> //TODO: remove

#define INFINITY 9999
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

	/** S'il ne peut pas se déplacer, il se tourne. */
	while (!move(dx, dy)) {
		this->_angle = rand() % 360;
		turned = true;
		dx = - this->vitesse * sin(this->_angle*PI/180);
		dy = this->vitesse * cos(this->_angle*PI/180);
	}

	/** S'il c'est tourné, il reviens à ça position initiale */
	if (turned)
		std::cout<<"Pas arriere "<<move(-dx, -dy)<<std::endl;
}

void Gardien::scout() {
	float visionRange = this->distVision * Environnement::scale;
	Mover* hunter = this->_l->_guards[0];

	float distHunter = sqrt((hunter->_x - this->_x) * (hunter->_x - this->_x) + (hunter->_y - this->_y) * (hunter->_y - this->_y));
	float alpha = acos((hunter->_y - this->_y) / distHunter);

	if (hunter->_x > this->_x) {
		alpha = 2 * PI - alpha; 
	}
		
	std::cout<<alpha*180/PI<<std::endl;

	if (distHunter <= visionRange ) {

		/** Vecteur (unitaire) collinéaire à la droite entre le gardien et le chasseur */
		double dx = -sin(alpha);
		double dy = cos(alpha);
		// std::cout<<"("<<dx<<","<<dy<<")"<<std::endl;
		
		

		int i = 1;

		/** La case physique où il se trouve avant son déplacement */
		int guardianX = (int) round((this->_x)/Environnement::scale);
		int guardianY = (int) round((this->_y)/Environnement::scale);
		
		/** La case physique que j'observe */
		int lookX = (int) round((this->_x+i*dx)/Environnement::scale);
		int lookY = (int) round((this->_y+i*dy)/Environnement::scale);

		int hunterX = (int) round((hunter->_x)/Environnement::scale);
		int hunterY = (int) round((hunter->_y)/Environnement::scale);
		while((lookX == guardianX && lookY == guardianY) || ((lookX != hunterX || lookY != hunterY) && this->_l->data(lookX, lookY) == 0)) {
			i++;
			lookX = (int) round((this->_x+i*dx)/Environnement::scale);
			lookY = (int) round((this->_y+i*dy)/Environnement::scale);
		}

		std::cout<<hunterX<<" "<<hunterY<<std::endl;

		/** S'il a vu le chasseur */
		if (lookX == hunterX && lookY == hunterY) {
			char* tmp = new char[128];
			sprintf(tmp,"Vu");
			message(tmp);
			std::cout<<"Vu"<<std::endl;
			
			Labyrinthe* lab = ((Labyrinthe*) _l);
			lab->updateLastPos(hunterX, hunterY, (alpha*180/PI));
			
			this->etat = EtatGardien::attaque;
		}
	}
}

void Gardien::attack() {
	Labyrinthe* lab = ((Labyrinthe*) _l);
	int* lastPos = lab->getLastPos();

	// std::cout<<lastPos[2] << " " << this->_angle<<std::endl;

	if (lastPos[2] != this->_angle) {

		if (this->_angle < lastPos[2]) {
			if (lastPos[2] - this->_angle > 180 ) {
				if (this->_angle == 0) {
					this->_angle = 359;	
				} else {
					this->_angle--;
				}
			} else {
				this->_angle = (this->_angle + 1) % 360;
			}
		} else {
			if (this->_angle - lastPos[2] > 180 ) {
				this->_angle = (this->_angle + 1) % 360;
			} else {
				if (this->_angle == 0) {
					this->_angle = 359;	
				} else {
					this->_angle--;
				}
			}
		}
		std::cout<<"turning"<<std::endl;
	} else {
		//si pas boule de feu, tirer
		if (!this->fireBallActive) {
			std::cout<<"shooting"<<std::endl;
			this->fire(0);
			this->fireBallActive = true;
		}

		/** Le déplacement du gardien en x et en y */
		double dx = - this->vitesse * sin(this->_angle*PI/180);
		double dy = this->vitesse * cos(this->_angle*PI/180);
		
		if (!move(dx, dy))
			this->etat = EtatGardien::patrouille;

		/** La case physique où il se trouve après son déplacement */
		int guardianX = (int) round((this->_x)/Environnement::scale);
		int guardianY = (int) round((this->_y)/Environnement::scale);

		if (guardianX == lastPos[0] && guardianY == lastPos[1]) {
			std::cout<<"Chasseur atteint"<<std::endl;
			this->etat = EtatGardien::patrouille;
		}
	}



}

//Objectif le faire se diriger vers le trésor de manière naturelle
void Gardien::modeDefense(){
	//On selectionne l'angle de la case vers laquelle on doit se diriger!
	int angle = caseProche((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale));
	//On calcule un angle aléatoire en prenant un cone de 90°
	int angleRandom = (rand() % 90) - 45;

	// S'il a changé de case, on retire un angle  
	if(changeCase){
		changeCase = false;
		//Entre O et 180, il se dirige vers la gauche
		if(angle<=180 && angle > 0){
			this->_angle =  angle + angleRandom;
			double dx = - this->vitesse * sin(this->_angle*PI/180);
			double dy = this->vitesse * cos(this->_angle*PI/180);
			move(dx,dy);
		}
		//Entre 180 et 360, il se dirige vers la droite 
		else{
			this->_angle =  angle + angleRandom;
			double dx = - this->vitesse * sin(this->_angle*PI/180);
			double dy = this->vitesse * cos(this->_angle*PI/180);
			move(dx,dy);
		}
	}
	// S'il n'a pas changé de case, on reste avec le même angle
	else{
		double dx = - this->vitesse * sin(this->_angle*PI/180);
		double dy = this->vitesse * cos(this->_angle*PI/180);
		//Dans le cas ou il est bloqué devant un obstacle, on le fait changer d'angle.
		if(!move(dx, dy)){changeCase = true;};
	}
}

//Fonction qui retourne l'id de la case la plus proche
//cx, cy étant les coordonnées de cette case
//angle, l'angle vers lequel on se déplace
int Gardien::caseProche(int x, int y){
	Labyrinthe* lab = ((Labyrinthe*) _l);
	int angle = 0;
	int minVal = INFINITY;

	//case en haut
	int haut = lab->getDistanceTresor(x-1,y);
	if (haut < minVal){
		minVal = haut;
		angle = 90;
	}

	//Case en bas
	int bas = lab->getDistanceTresor(x+1,y);
	if (bas < minVal){
		minVal = haut;
		angle = 270;
	}

	//Case à gauche
	int gauche = lab->getDistanceTresor(x,y-1);
	if (gauche < minVal){
		minVal = gauche;
		angle = 180;
	}

	//Case à droite
	int droite = lab->getDistanceTresor(x,y+1);
	if (droite < minVal){
		minVal = droite;
		angle = 0;
	}

	//Case en haut à droite
	int hautdroite = lab->getDistanceTresor(x-1,y+1);
	if (hautdroite < minVal){
		minVal = hautdroite;
		angle = 45;
	}

	//Case en haut à gauche
	int hautgauche = lab->getDistanceTresor(x-1,y-1);
	if (hautgauche < minVal){
		minVal = hautgauche;
		angle = 135;
	}

	//Case en bas à droite
	int basdroite = lab->getDistanceTresor(x+1,y+1);
	if (basdroite < minVal){
		minVal = basdroite;
		angle = 315;
	}

	//Case en bas à gauche
	int basgauche = lab->getDistanceTresor(x-1,y+1);
	if (basgauche < minVal){
		minVal = basgauche;
		angle = 225;
	}
	return angle;
}

void Gardien::update() {
	scout();
	majPotentielDefense();
	switch (this->etat) {
		case patrouille:
			this->patrol();
			break;
		case attaque:
			this->attack();
			break;
		case defense:
			this->modeDefense();
			break;
	}
	// std::cout<<"tick"<<std::endl;
}

/** Déplace le gardien si le déplacement est valide */
bool Gardien::move(double dx, double dy) {
	/** La case physique où il se trouve avant son déplacement */
	int preMoveI = (int) round((this->_x)/Environnement::scale);
	int preMoveJ = (int) round((this->_y)/Environnement::scale);

	/** La case physique où il se trouve après son déplacement */
	int postMoveI = (int) round((this->_x+dx)/Environnement::scale);
	int postMoveJ = (int) round((this->_y+dy)/Environnement::scale);

	/** Le gardien avance ~1/10 de case, 
	 * s'il regarde sa case il doit continuer d'avancer
	 */
	if ((postMoveI!=preMoveI || postMoveJ !=preMoveJ) && this->_l->data(postMoveI, postMoveJ) == 1) {
		return false;
	} else {
	// Si on change de case, on passe notre booléan à vrai.
		if (preMoveI != postMoveI || preMoveJ != postMoveJ){changeCase = true;}
		Labyrinthe* lab = ((Labyrinthe*) _l);
		lab->setData((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale), 0);
		this->_x += dx;
		this->_y += dy;
		lab->setData((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale), 1);
		return true;
	}
}

void Gardien::fire (int angle_vertical) {
	Chasseur* hunter = ((Chasseur*) this->_l->_guards[0]);
	message ("Woooshh...");
	hunter->_hunter_fire->play();
	this->_fb->init(this->_x, this->_y, 8., angle_vertical, (360 - this->_angle) % 360);
}

bool Gardien::process_fireball (float dx, float dy) {

	/** Position de la boule après le déplacement */
	int ballX = (int) (this->_fb ->get_x() + dx) / Environnement::scale;
	int ballY = (int) (this->_fb ->get_y() + dy) / Environnement::scale;

	/** La case physique du gardien */
	int guardianX = (int) round((this->_x)/Environnement::scale);
	int guardianY = (int) round((this->_y)/Environnement::scale);
	
	if ((ballX == guardianX && ballY == guardianY) || this->_l->data(ballX, ballY) == 0) {
		return true;
	}

	/** Collision */
	
	/** On joue le son de l'explosion */

	float hitX = (this->_fb ->get_x() + dx - this->_x) / Environnement::scale;
	float hitY = (this->_fb ->get_y() + dy - this->_y) / Environnement::scale;

	float distHit = hitX*hitX + hitY*hitY;
	float distMax = this->_l->width()*this->_l->width() + this->_l->height()*this->_l->height();

	Chasseur* hunter = ((Chasseur*) this->_l->_guards[0]);
	hunter->_wall_hit->play(1. - distHit/distMax);

	/** -- */

	// if (ballX == this->_l->_treasor._x && ballY == this->_l->_treasor._y) {
	// 	this->fireBallActive = false;
	// 	return false;
	// }

	
	this->fireBallActive = false;
	return false;
}

void Gardien::majPotentielDefense(){
	Labyrinthe* lab = ((Labyrinthe*) _l);

	int dActuelle =  lab->getDistanceTresor((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale));
	this->potentielDefense = (lab->distanceMax)/ (float) dActuelle;

	//Attention le premier gardien est en fait le chasseur.
	float sumPD = 0.0;
	int dernierGardienDefense = 0;
	int seuil = lab->distanceMax;//TODO: Pour le moment on initialise la valeur du seuil à Dmax

	//on parcourt la liste des gardiens, en commencant à 0 pour ne pas prendre en compte le chasseur 
	//sumPD contient la somme du potentiel defense des gardiens
	//dernierGardienDefense est l'id du gardien
	for (int g=1; g<this->_l->_nguards; g++) {
		Gardien* gard = ((Gardien*) lab->_guards[g]);
		if (gard->etat == EtatGardien::defense && gard->mort == false ){
			sumPD += gard->potentielDefense;
			dernierGardienDefense = g;
		}
	}

	//Si la somme des potentiels défenses est plus grande que le seuil -> défend trop 
	//Alors on retire le dernier gardien et le passe en mode patrouille
	if(sumPD > seuil){
		((Gardien*) lab->_guards[dernierGardienDefense])->etat = EtatGardien::patrouille;
	}
	//On est dans le cas ou pas trop de défenseurs
	else{
		//On vérifie qu'il y a encore un  gardien dans la liste après
		if(this->_l->_nguards != dernierGardienDefense+1){
			//Si on ajoute le potentiel défense du prochain gardien et que ça ne dépasse pas le seuil, il vient en défense
			if((sumPD +((Gardien*) lab->_guards[dernierGardienDefense+1])->potentielDefense) <= seuil){
				((Gardien*) lab->_guards[dernierGardienDefense+1])->etat = EtatGardien::defense;
			}
		}
	}
}