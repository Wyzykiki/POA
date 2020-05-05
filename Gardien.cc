#include "Gardien.h"
#include "Labyrinthe.h"
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

//Objectif le faire se diriger vers le trésor de manière naturelle
void Gardien::modeDefense(){
	//On selectionne l'angle de la case vers laquelle on doit se diriger!
	int angle = caseProche((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale));
	std::cout<<this->_angle<<std::endl;
	this->_angle = angle;
	double dx = - this->vitesse * sin(this->_angle*PI/180);
	double dy = this->vitesse * cos(this->_angle*PI/180);
	move(dx, dy);

	// Il avance tout droit -> ne change pas d'angle
	// if(angle==0){
	// 	double dx = - this->vitesse * sin(this->_angle*PI/180);
	// 	double dy = this->vitesse * cos(this->_angle*PI/180);
	// 	move(dx, dy);
	// }
	// //On va devoir changer son angle de direction
	// else{
	// 	//On change l'angle de 45°
	// 	if(angle<=180 && angle > 0){
	// 		// this->_angle = (this->_angle + 45) % 360;
	// 		this->_angle =  45 ;
	// 		double dx = - this->vitesse * sin(this->_angle*PI/180);
	// 		double dy = this->vitesse * cos(this->_angle*PI/180);
	// 		std::cout<<move(dx, dy)<<std::endl;
	// 	}
	// 	// 180<angle
	// 	//On change l'angle de 315°
	// 	else{
	// 		// this->_angle = (this->_angle + 315) % 360;
	// 		this->_angle =  315 ;
	// 		double dx = - this->vitesse * sin(this->_angle*PI/180);
	// 		double dy = this->vitesse * cos(this->_angle*PI/180);
	// 		std::cout<<move(dx, dy)<<std::endl;

	// 	}
	// }
	
}

//Fonction qui retourne l'id de la case la plus proche
//cx, cy étant les coordonnées de cette case
//angle, l'angle vers lequel on se déplace
int Gardien::caseProche(int x, int y){
	std::cout<<" x: "<<x<<" y: "<<y<<std::endl;
	Labyrinthe* lab = ((Labyrinthe*) _l);
	int cx = x;
	int cy = y;
	int angle = 0;
	int minVal = INFINITY;

	//case en haut
	int haut = lab->getDistanceTresor(x-1,y);
	if (haut < minVal){
		minVal = haut;
		cx = x-1;
		cy = y;
		angle = 90;
	}

	//Case en bas
	int bas = lab->getDistanceTresor(x+1,y);
	if (bas < minVal){
		minVal = haut;
		cx = x+1;
		cy = y;
		angle = 270;
	}

	//Case à gauche
	int gauche = lab->getDistanceTresor(x,y-1);
	if (gauche < minVal){
		minVal = gauche;
		cx = x;
		cy = y-1;
		angle = 180;
	}

	//Case à droite
	int droite = lab->getDistanceTresor(x,y+1);
	if (droite < minVal){
		minVal = droite;
		cx = x;
		cy = y+1;
		angle = 0;
	}

	//Case en haut à droite
	int hautdroite = lab->getDistanceTresor(x-1,y+1);
	if (hautdroite < minVal){
		minVal = hautdroite;
		cx = x-1;
		cy = y+1;
		angle = 45;
	}

	//Case en haut à gauche
	int hautgauche = lab->getDistanceTresor(x-1,y-1);
	if (hautgauche < minVal){
		minVal = hautgauche;
		cx = x-1;
		cy = y-1;
		angle = 135;
	}

	//Case en bas à droite
	int basdroite = lab->getDistanceTresor(x+1,y+1);
	if (basdroite < minVal){
		minVal = basdroite;
		cx = x+1;
		cy = y+1;
		angle = 315;
	}

	//Case en bas à gauche
	int basgauche = lab->getDistanceTresor(x-1,y+1);
	if (basgauche < minVal){
		minVal = basgauche;
		cx = x-1;
		cy = y+1;
		angle = 225;
	}
	std::cout<<"Case proche "<<cx<<" "<<cy<<"angle "<<angle<<std::endl;
	return angle;
}

void Gardien::update() {
	majPotentielDefense();
	switch (this->etat) {
		case patrouille:
			this->patrol();
			break;
		case attaque:
			break;
		case defense:
			this->modeDefense();
			break;
	}
}

//Permet de vérifier que ton déplacement n'est pas vers un obstacle
bool Gardien::move(double dx, double dy) {
	std::cout<<dx<< " "<<dy<<std::endl;
    /** La case physique où il se trouve avant son déplacement */
    int preMoveI = (int) round((this->_x)/Environnement::scale);
    int preMoveJ = (int) round((this->_y)/Environnement::scale);

    /** La case physique où il se trouve après son déplacement */
    int postMoveI = (int) round((this->_x+dx)/Environnement::scale);
    int postMoveJ = (int) round((this->_y+dy)/Environnement::scale);
	std::cout<<"actuel ("<<preMoveI<<","<<preMoveJ<<") suivant ("<<postMoveI<<','<<postMoveJ<<")"<<std::endl;
    /** Le gardien avance ~1/10 de case, 
     * s'il regarde sa case il doit continuer d'avancer
     */
    if ((postMoveI!=preMoveI || postMoveJ !=preMoveJ) && this->_l->data(postMoveI, postMoveJ) == 1) {
        return false;
    } else {
        Labyrinthe* lab = ((Labyrinthe*) _l);
        lab->setData((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale), 0);
        this->_x += dx;
        this->_y += dy;
        lab->setData((int) round(this->_x/Environnement::scale), (int) round(this->_y/Environnement::scale), 1);
        return true;
    }
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