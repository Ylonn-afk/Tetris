# Tetris
Jeu Tetris développé en C

Pour faire fonctionner le programme il a quelques commandes nécessaires : 

sudo apt install mpg123

Puis dans le fichier Gfxlib.c changer la ligne 77 par :

#include <GL/freeglut.h>          

Enfin on compile et on execute avec: 

make
./main
