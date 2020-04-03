#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
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
	this->_nguards = 0;
	
	//Le nombres de jonction de mur
	int nbCorners = 0;

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
				//Traitement affiche
			}

			if (zoneLaby) {
				int sizeLine = line.size();
				for (int i = 0; i<sizeLine; i++){
					if (line[i] != ' ' && line[i] != 'C') {
						
						if (line[i] == '+') {
							nbCorners++;
						}
						else if (line[i] == 'X') {
							// TODO: this->_nboxes++;
						}
						else if (line[i] == 'G') {
							// TODO: this->_nguards++;
						}
						else if (line[i] >= 'a' && line[i] <= 'z') {
							// TODO: this->_npicts++;
						}
						//else TODO: exeption mauvais laby ?
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



    //test affichage fichier
    for (int a = 0; a < this->lab_width+1; a++) {
        for (int b = 0; b < this->lab_height+1; b++) {
            cout<<matFile[a][b];
        }
        cout<<endl;
    }

	//Chaque jonction peut être touché par 4 mur maximum (on prend au plus large)
	Wall* tooMuchWalls = new Wall[nbCorners*4];

	//On initialise les murs
	int indexWall = 0;
	// Passe mur horizontaux
	for (int x=0; x<this->lab_width; x++) {
		int y=0;
		while (y<this->lab_height) {
			if (matFile[x][y] == '+' && (matFile[x][y+1] == '-' || matFile[x][y+1] == '+' || (matFile[x][y+1] >= 'a' && matFile[x][y+1] <= 'z'))) {
				tooMuchWalls[indexWall]._x1 = x;
				tooMuchWalls[indexWall]._y1 = y;
				y++;
				while (matFile[x][y] == '-' || (matFile[x][y] >= 'a' && matFile[x][y] <= 'z')) {
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















	// le chasseur et les 4 gardiens.
	this->_nguards = 1+4;
	this->_guards = new Mover* [_nguards];
	this->_guards [0] = new Chasseur (this);
	this->_guards [1] = new Gardien (this, "drfreak");
	this->_guards [2] = new Gardien (this, "Marvin"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.;
	this->_guards [3] = new Gardien (this, "Potator"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	this->_guards [4] = new Gardien (this, "garde"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;

	// indiquer qu'on ne marche pas sur les gardiens.
	_data [(int)(_guards [1] -> _x / scale)][(int)(_guards [1] -> _y / scale)] = 1;
	_data [(int)(_guards [2] -> _x / scale)][(int)(_guards [2] -> _y / scale)] = 1;
	_data [(int)(_guards [3] -> _x / scale)][(int)(_guards [3] -> _y / scale)] = 1;
	// _data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;
}