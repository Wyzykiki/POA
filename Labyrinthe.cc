#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <cstring>
#include <string>
#include <fstream>
#include <map>
#include <iostream>

using namespace std;

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
	
	//Le nombres de jonction de mur
	int nbCorners = 0;

	//Map (lettre, filename) pour les affiches
	map<char, string> affiches;

	//La ligne courante du fichier
	string line;

	//Indique si l'on est dans la partie du dessin du labyrinthe; dans le fichier
	bool zoneLaby = false;

	//=========== Première passe fichier ==================
	ifstream file(filename, ifstream::in);
	while (getline(file, line)) {
		if (line[0] != '#' && line != "") {
			
			if (!zoneLaby && line[0] == '+') {
				zoneLaby = true;
			}

			//Définition d'une affiche
			if (!zoneLaby && line[0] >= 'a' && line[0] <= 'z') {
				affiches[line[0]] = string(line, 2, string::npos);
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
									//TODO: exeption mauvais laby ?
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

    int i = 0;
	file.open(filename);
    while (getline(file, line)) {
		if (!zoneLaby && line[0] == '+') {
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
					this->_data[i][j] = (line[j] != ' ' && line[j] != 'C');
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
									guard = new Gardien(this, "drfreak");
									break;
								case 2:
									guard = new Gardien(this, "Marvin");
									break;
								case 3:
									guard = new Gardien(this, "Potator");
									break;
								default:
									guard = new Gardien(this, "garde");
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


	//Chaque jonction peut être touché par 4 mur maximum (on prend au plus large)
	Wall* tooMuchWalls = new Wall[nbCorners*4];

	//On initialise les murs
	int indexWall = 0;

	//Initialistaion des affiches
	this->_picts = new Wall[this->_npicts];
	int indexPicts = 0;

	// Passe mur horizontaux
	for (int x=0; x<this->lab_width; x++) {
		int y=0;
		while (y<this->lab_height) {
			if (matFile[x][y] == '+' && (matFile[x][y+1] == '-' || matFile[x][y+1] == '+' || (matFile[x][y+1] >= 'a' && matFile[x][y+1] <= 'z'))) {
				tooMuchWalls[indexWall]._x1 = x;
				tooMuchWalls[indexWall]._y1 = y;
				y++;
				while (matFile[x][y] == '-' || (matFile[x][y] >= 'a' && matFile[x][y] <= 'z')) {
					//Affichage de l'affiche
					if (matFile[x][y] >= 'a' && matFile[x][y] <= 'z') {
						this->_picts[indexPicts]._x1 = x;
						this->_picts[indexPicts]._y1 = y;
						this->_picts[indexPicts]._x2 = x;
						this->_picts[indexPicts]._y2 = y+2;
						//Si l'affiche est collée à un mur Est (va déborder), on la décale de 1 sur la gauche
						if (matFile[x][y+1] == '+') {
							this->_picts[indexPicts]._y1--;
							this->_picts[indexPicts]._y2--;
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
			if (matFile[x][y] == '+' && (matFile[x+1][y] == '|' || matFile[x+1][y] == '+' || (matFile[x+1][y] >= 'a' && matFile[x+1][y] <= 'z'))) {
				tooMuchWalls[indexWall]._x1 = x;
				tooMuchWalls[indexWall]._y1 = y;
				x++;
				while (matFile[x][y] == '|' || (matFile[x][y] >= 'a' && matFile[x][y] <= 'z')) {
					//Affichage de l'affiche
					if (matFile[x][y] >= 'a' && matFile[x][y] <= 'z') {
						this->_picts[indexPicts]._x1 = x;
						this->_picts[indexPicts]._y1 = y;
						this->_picts[indexPicts]._x2 = x+2;
						this->_picts[indexPicts]._y2 = y;
						//Si l'affiche est collée à un mur Sud (va déborder), on la décale de 1 vers le haut
						if (matFile[x+1][y] == '+') {
							this->_picts[indexPicts]._x1--;
							this->_picts[indexPicts]._x2--;
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


	this->_guards[0] = new Chasseur(this);
	this->_guards[0]->_x = hunter_x*scale;
	this->_guards[0]->_y = hunter_y*scale;
}