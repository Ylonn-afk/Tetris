#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "GfxLib.h"
#include "BmpLib.h" // Cet include permet de manipuler des fichiers BMP
#include "ESLib.h"
#define N 22
#define M 10
#define TAILLE_CASE 25
#include "test.h"

int main(int argc, char **argv)
{

    initialiseGfx(argc, argv);
    prepareFenetreGraphique("Tetris", 400, 600);

    // Lancement de la boucle d'événements
    lanceBoucleEvenements(); // elle fait l'appel gestion d'eveemnt et gere la fermetureFenetreDemandee

    return 0;
}

