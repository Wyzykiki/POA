#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <cstring>
#include <string>
#include <fstream>
#include <map>
#include <queue>
#include <iostream>
#define INFINITY 9999

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touch�.
Sound*	Chasseur::_wall_hit;	// on a tap� un mur.

Environnement* Environnement::init (char* filename){
	return new Labyrinthe (filename);
}

Labyrinthe::Labyrinthe (char* filename){

	// Initialisation dimension du rectangle englobant
	this->lab_width = 0;
	this->lab_height = 0;

	// Initialisation des attributs "nombre de"
	this->_nwall = 0;
	this->_npicts = 0;
	this->_nboxes = 0;
	this->_nguards = 1;
	this->nPads = 0;
	this->nBWalls = 0;
	
	//Le nombres de jonction de mur
	int nbCorners = 0;

	//Map (lettre, filename) pour les affiches
	std::map<char, std::string> affiches;

	//Définition de la texture pour les murs cassables
	affiches['*'] = std::string("breakable.jpg");

	//La ligne courante du fichier
	std::string line;

	//Indique si l'on est dans la partie du dessin du labyrinthe; dans le fichier
	bool zoneLaby = false;

	//=========== Première passe fichier ==================
	std::ifstream file(filename, std::ifstream::in);
	while (getline(file, line)) {
		if (line[0] != '#' && line != "") {
			
			if (!zoneLaby && line.find('+') != std::string::npos) {
				zoneLaby = true;
			}

			//Définition d'une affiche
			if (!zoneLaby && line[0] >= 'a' && line[0] <= 'z') {
				affiches[line[0]] = std::string(line, 2, std::string::npos);
			}

			if (zoneLaby) {
				int sizeLine = line.size();
				for (int i = 0; i<sizeLine; i++) {
					if (line[i] != ' ' && line[i] != 'C') {
						
						switch (line[i]) {
							case '+':
								nbCorners++;
								break;
							case 'X':
								this->_nboxes++;
								break;
							case 'G':
								this->_nguards++;
								break;
							default:
								if (line[i] >= 'a' && line[i] <= 'z') {
									this->_npicts++;
								} else {
									if (line[i] >= '1' && line[i] <= '9') {
										this->nPads++;
									} else {
										if (line[i] == '*') {
											this->nBWalls++;
											this->_npicts++;
										}
									}
								}
								break;
						}
					}
				}

				this->lab_width++;
				if (sizeLine > this->lab_height) {
					this->lab_height = sizeLine;
				}
			}
		}
	}
	file.close();


	//=========== Deuxième passe fichier ================
	zoneLaby = false;
	//Matrice représentant le labyrinthe
	char matFile[this->lab_width+1][this->lab_height+1];

	//Initialisation de _data
	this->_data = new char*[this->lab_width];

	//Initialisation des boites
	this->_boxes = new Box[this->_nboxes];
	int indexBox = 0;

	// //Initialisation des gardiens
	this->_guards = new Mover*[this->_nguards];
	int indexGuardian = 1, hunter_x, hunter_y;

	//Initialisation des téléporteurs
	this->pads = new TeleportPad[this->nPads];
	int indexPads = 0;

    int i = 0;
	file.open(filename);
    while (getline(file, line)) {
		if (!zoneLaby && line.find('+') != std::string::npos) {
			zoneLaby = true;
		}
		//On est dans la zone du labyrinthe
		if (zoneLaby) {

			//Initialisation de _data
			this->_data[i] = new char[this->lab_height];

			int sizeLine = line.size();
			int j = 0;
			while (j < this->lab_height+1) {

				if (j < this->lab_height) {
					this->_data[i][j] = ((line[j] != ' ' && line[j] != 'C') && !(line[j] >= '1' && line[j] <= '9'));
				}

				if (j < sizeLine) {
					matFile[i][j] = line[j];
					
					switch (line[j]) {
						case 'X':
							this->_boxes[indexBox]._x = i;
							this->_boxes[indexBox]._y = j;
							this->_boxes[indexBox]._ntex = 0;
							indexBox++;
							break;
						case 'T':
							this->_treasor._x = i;
							this->_treasor._y = j;
							break;
						case 'G':
							Mover* guard;
							switch (indexGuardian%4) {
								case 1:
									guard = new Gardien(this, /* "Link2" */"drfreak", indexGuardian<4);
									break;
								case 2:
									guard = new Gardien(this, "Marvin", indexGuardian<4);
									break;
								case 3:
									guard = new Gardien(this, "Potator", indexGuardian<4);
									break;
								default:
									guard = new Gardien(this, "garde", false);
									break;
							}

							this->_guards[indexGuardian] = guard;
							this->_guards[indexGuardian]->_x = i*scale;
							this->_guards[indexGuardian]->_y = j*scale;
							indexGuardian++;
							break;
						case 'C':
							hunter_x = i;
							hunter_y = j;
							break;
						default:
							if (line[j] >= '1' && line[j] <= '9') {
								this->pads[indexPads].id = line[j];
								this->pads[indexPads].x = i;
								this->pads[indexPads].y = j;
								this->pads[indexPads].sibling = &(this->pads[indexPads]);
								indexPads++;
							}
							break;
					}

				} else {
					matFile[i][j] = ' ';
				}
				j++;
			}
			i++; // On change de ligne
		}
    }
    file.close();
	//On remplie la dernière ligne de la matrice
	for (int j=0; j<this->lab_height+1; j++) {
		matFile[this->lab_width][j] = ' ';
	}

	//On relie les téleporteurs
	for (int j=0; j<this->nPads; j++) {
		int k=j+1;
		bool founded = false;
		while (this->pads[j].sibling == &(this->pads[j]) && !founded && k<this->nPads) {
			if (this->pads[j].id == this->pads[k].id) {
				founded = true;
				this->pads[j].sibling = &(this->pads[k]);
				this->pads[k].sibling = &(this->pads[j]);
			}
			k++;
		}
	}

	//Chaque jonction peut être touché par 4 mur maximum (on prend au plus large)
	Wall* tooMuchWalls = new Wall[nbCorners*4];

	//On initialise les murs
	int indexWall = 0;

	//Initialistaion des affiches
	this->_picts = new Wall[this->_npicts];
	int indexPicts = 0;

	//Initialisation des murs cassables
	this->bWalls = new BreakableWall[this->nBWalls];
	int indexBWalls = 0;

	// Passe mur horizontaux
	for (int x=0; x<this->lab_width; x++) {
		int y=0;
		while (y<this->lab_height) {
			if (matFile[x][y] == '+' && (matFile[x][y+1] == '-' || matFile[x][y+1] == '*' || matFile[x][y+1] == '+' || (matFile[x][y+1] >= 'a' && matFile[x][y+1] <= 'z'))) {
				tooMuchWalls[indexWall]._x1 = x;
				tooMuchWalls[indexWall]._y1 = y;
				y++;
				while (matFile[x][y] == '-' || matFile[x][y] == '*' || (matFile[x][y] >= 'a' && matFile[x][y] <= 'z')) {
					//Affichage de l'affiche
					if ((matFile[x][y] >= 'a' && matFile[x][y] <= 'z') || matFile[x][y] == '*') {
						this->_picts[indexPicts]._x1 = x;
						this->_picts[indexPicts]._y1 = y-1;
						this->_picts[indexPicts]._x2 = x;
						this->_picts[indexPicts]._y2 = y+1;

						if (matFile[x][y] == '*') {
							this->bWalls[indexBWalls].x = x;
							this->bWalls[indexBWalls].y = y;
							this->bWalls[indexBWalls].pictIndex = indexPicts;
							indexBWalls++;
						}

						char* path = new char[128];
						strcpy(path, this->texture_dir);
						strcat(path, "/");
						strcat(path, (affiches[matFile[x][y]]).c_str());
						this->_picts[indexPicts]._ntex = this->wall_texture(path);
						indexPicts++;
					}
					y++;
				}
				//On sort du while donc matFile[x][y] == '+'
				tooMuchWalls[indexWall]._x2 = x;
				tooMuchWalls[indexWall]._y2 = y;
				tooMuchWalls[indexWall]._ntex = 0;
				indexWall++;
			} else {
				y++;
			}
		}
	}

	//Passe mur verticaux
	for (int y=0; y<this->lab_height; y++) {
		int x=0;
		while (x<this->lab_width) {
			if (matFile[x][y] == '+' && (matFile[x+1][y] == '|' || matFile[x+1][y] == '*' || matFile[x+1][y] == '+' || (matFile[x+1][y] >= 'a' && matFile[x+1][y] <= 'z'))) {
				tooMuchWalls[indexWall]._x1 = x;
				tooMuchWalls[indexWall]._y1 = y;
				x++;
				while (matFile[x][y] == '|' || matFile[x][y] == '*' || (matFile[x][y] >= 'a' && matFile[x][y] <= 'z')) {
					//Affichage de l'affiche
					if ((matFile[x][y] >= 'a' && matFile[x][y] <= 'z') || matFile[x][y] == '*') {
						this->_picts[indexPicts]._x1 = x-1;
						this->_picts[indexPicts]._y1 = y;
						this->_picts[indexPicts]._x2 = x+1;
						this->_picts[indexPicts]._y2 = y;

						if (matFile[x][y] == '*') {
							this->bWalls[indexBWalls].x = x;
							this->bWalls[indexBWalls].y = y;
							this->bWalls[indexBWalls].pictIndex = indexPicts;
							indexBWalls++;
						}

						char* path = new char[128];
						strcpy(path, this->texture_dir);
						strcat(path, "/");
						strcat(path, (affiches[matFile[x][y]]).c_str());
						this->_picts[indexPicts]._ntex = this->wall_texture(path);
						indexPicts++;
					}
					x++;
				}
				//On sort du while donc matFile[x][y] == '+'
				tooMuchWalls[indexWall]._x2 = x;
				tooMuchWalls[indexWall]._y2 = y;
				tooMuchWalls[indexWall]._ntex = 0;
				indexWall++;
			} else {
				x++;
			}
		}
	}

	this->_nwall = indexWall;
	this->_walls = new Wall[this->_nwall];
	for (int k=0; k<indexWall; k++) {
		this->_walls[k]._x1 = tooMuchWalls[k]._x1;
		this->_walls[k]._x2 = tooMuchWalls[k]._x2;
		this->_walls[k]._y1 = tooMuchWalls[k]._y1;
		this->_walls[k]._y2 = tooMuchWalls[k]._y2;
		this->_walls[k]._ntex = tooMuchWalls[k]._ntex;
	}

	delete[] tooMuchWalls;	

	//Placement du chasseur
	this->_guards[0] = new Chasseur(this);
	this->_guards[0]->_x = hunter_x*scale;
	this->_guards[0]->_y = hunter_y*scale;


	//Matrice des distances, on les initialise sans le +1, car pas nécéssaire
	this->treasor_distance = new int*[this->lab_width];	

	
	//La file qui va nous permettre de stocker les cases à traiter au fur et à mesure.
	std::queue<int> fileCase;
	//On initialise la matrice des distances. Le trésor est à 0 et le reste à une distance infinie
	for (int a = 0; a < this->lab_width; a++) {
		this->treasor_distance[a] = new int[this->lab_height];
        for (int b = 0; b < this->lab_height; b++) {
			if(matFile[a][b]=='T'){
				this->treasor_distance[a][b] = 0;
				//On ajoute la case avec le trésor à la file
				fileCase.push(a);
				fileCase.push(b);
				fileCase.push(0);
			}
			else{
				this->treasor_distance[a][b] = INFINITY;
			}
        }
    }
	// this->treasor_distance[_treasor._x][_treasor._y] = 0;
	while(!fileCase.empty()){

		//On retire le premier point
		int x = fileCase.front();
		fileCase.pop();
		int y = fileCase.front();
		fileCase.pop();

		//On récupère notre distance
		int d = fileCase.front() + 1;
		fileCase.pop();

		//On calcule la distance, si c'est C, G ou ' ' ok 
		//Case en haut
		if (matFile[x-1][y] == ' ' || matFile[x-1][y] == 'C' || matFile[x-1][y] == 'G' || (matFile[x-1][y] >= '1' && matFile[x-1][y] <= '9') || matFile[x-1][y] == '*'){
			if (this->treasor_distance[x-1][y]>d){
				this->treasor_distance[x-1][y] = d;
				fileCase.push(x-1);
				fileCase.push(y);
				fileCase.push(d);
			}
		}
		//Case en bas
		if (matFile[x+1][y] == ' ' || matFile[x+1][y] == 'C' || matFile[x+1][y] == 'G' || (matFile[x+1][y] >= '1' && matFile[x+1][y] <= '9') || matFile[x+1][y] == '*'){
			if (this->treasor_distance[x+1][y]>d){
				this->treasor_distance[x+1][y] = d;
				fileCase.push(x+1);
				fileCase.push(y);
				fileCase.push(d);
			}
		}
		//Case à gauche
		if (matFile[x][y-1] == ' ' || matFile[x][y-1] == 'C' || matFile[x][y-1] == 'G' || (matFile[x][y-1] >= '1' && matFile[x][y-1] <= '9') || matFile[x][y-1] == '*'){
			if (this->treasor_distance[x][y-1]>d){
				this->treasor_distance[x][y-1] = d;
				fileCase.push(x);
				fileCase.push(y-1);
				fileCase.push(d);
			}
		}

		//Case à droite
		if (matFile[x][y+1] == ' ' || matFile[x][y+1] == 'C' || matFile[x][y+1] == 'G' || (matFile[x][y+1] >= '1' && matFile[x][y+1] <= '9') || matFile[x][y+1] == '*'){
			if (this->treasor_distance[x][y+1]>d){
				this->treasor_distance[x][y+1] = d;
				fileCase.push(x);
				fileCase.push(y+1);
				fileCase.push(d);
			}
		}


		//Case en haut à droite
		if (matFile[x-1][y+1] == ' ' || matFile[x-1][y+1] == 'C' || matFile[x-1][y+1] == 'G' || (matFile[x-1][y+1] >= '1' && matFile[x-1][y+1] <= '9') || matFile[x-1][y+1] == '*'){
			if (this->treasor_distance[x-1][y+1]>d){
				this->treasor_distance[x-1][y+1] = d;
				fileCase.push(x-1);
				fileCase.push(y+1);
				fileCase.push(d);
			}
		}
		//Case en haut à gauche
		if (matFile[x-1][y-1] == ' ' || matFile[x-1][y-1] == 'C' || matFile[x-1][y-1] == 'G' || (matFile[x-1][y-1] >= '1' && matFile[x-1][y-1] <= '9') || matFile[x-1][y-1] == '*'){
			if (this->treasor_distance[x-1][y-1]>d){
				this->treasor_distance[x-1][y-1] = d;
				fileCase.push(x-1);
				fileCase.push(y-1);
				fileCase.push(d);
			}
		}
		//Case en bas à droite
		if (matFile[x+1][y+1] == ' ' || matFile[x+1][y+1] == 'C' || matFile[x+1][y+1] == 'G' || (matFile[x+1][y+1] >= '1' && matFile[x+1][y+1] <= '9') || matFile[x+1][y+1] == '*'){
			if (this->treasor_distance[x+1][y+1]>d){
				this->treasor_distance[x+1][y+1] = d;
				fileCase.push(x+1);
				fileCase.push(y+1);
				fileCase.push(d);
			}
		}

		//Case en bas à gauche
		if (matFile[x+1][y-1] == ' ' || matFile[x+1][y-1] == 'C' || matFile[x+1][y-1] == 'G' || (matFile[x+1][y-1] >= '1' && matFile[x+1][y-1] <= '9') || matFile[x+1][y-1] == '*'){
			if (this->treasor_distance[x+1][y-1]>d){
				this->treasor_distance[x+1][y-1] = d;
				fileCase.push(x+1);
				fileCase.push(y-1);
				fileCase.push(d);
			}
		}
	}
	// Affichage treasor
	// for (int a = 0; a < this->lab_width; a++) {
    //     for (int b = 0; b < this->lab_height; b++) {
	// 		std::cout<<treasor_distance[a][b];
	// 	}
	// 	std::cout<<std::endl;
	// }

	this->distanceMax = 0.0;
	//On calcule la distance max qu'il y a dans le labyrinthe
	for (int x = 0; x < this->lab_width; x++) {
        for (int y = 0; y < this->lab_height; y++) {
			if(this->treasor_distance[x][y] > this->distanceMax && this->treasor_distance[x][y] != INFINITY){
				this->distanceMax = this->treasor_distance[x][y];
			}
		}
	}
}

void Labyrinthe::breakWall(int index) {

	if (!this->bWalls[index].broken) {
		this->bWalls[index].broken = true;
		this->_data[this->bWalls[index].x][this->bWalls[index].y] = 0;

		//Change la texture du mur cassé
		char* path = new char[128];
		strcpy(path, this->texture_dir);
		strcat(path, "/broken.jpg");
		this->_picts[this->bWalls[index].pictIndex]._ntex = this->wall_texture(path);
		this->reconfigure();
	}
}