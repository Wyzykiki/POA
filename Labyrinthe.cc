#include "Labyrinthe.h"
#include <stdlib.h>
#include <stdio.h>
using namespace std;

Environnement* Environnement::init (char* filename){
	return new Labyrinthe (filename);
}

Labyrinthe::Labyrinthe (char* filename){

    //attention pas ajoutée dans le .h
    void initLaby(){
        //rewind -> changer de curseur, revenir au début du fichier
        string line;     
        int maxLength = 0;
        int maxHeight = 0;
        int nbWalls = 0;
        int nbPicts = 0;
        int nbBoxes = 0;
        int nbGards = 0;
        //=========== Première passe fichier ==================
        ifstream file(filename);
        while(getline(file, line)){
            if(line[0] != '#' && line[0] != '\n'){
                //C'est une lettre
                if(line[0] != ' ' && line[0] != '+'){
                    //Traitement affiche
                }
                else{
                    int sizeLine = line.size();
                    for(int i = 0; i< sizeLine; i++){
                        if (line[i] != ' ' && line[i] != 'C') {
                            
                            /* On ne peut pas marcher sur la case */
                            _data[maxHeight][i] = 1;

                            if(line[i] == '+'){ 
                                nbWalls++;
                            }
                            else if(line[i] >= 'a' && line[i] <= 'z'){ 
                                nbPicts++;
                            }
                            else if(line[i] == 'X'){ 
                                nbBoxes++;
                            }
                            else if(line[i] == 'G'){
                                nbGards++;
                            }
                        } else {
                            /* On peut marcher sur la case */
                            _data[maxHeight][i] = 0;
                        }
                    }

                    maxHeight++;
                    if(sizeLine > maxLength){
                        maxLength = sizeLine;
                    }

                }
            }
        }
        file.close();

        //=========== Deuxième passe fichier ================
        file.open(filename);
        char matFile[maxLength][maxHeight];
        



}