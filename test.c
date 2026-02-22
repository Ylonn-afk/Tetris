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

void lancerMusique(tetris *jeu)
{
    if (jeu->son == true)
    {
        system("mpg123 -q --loop -1 tetris4.mp3 &");
    }
    else
    {
        system("pkill mpg123");
    }
}

void arreterMusique()
{
    system("pkill mpg123"); // tue le processus mpg123
}

bool estDansGrille(int ligne, int colonne)
{
    if ((ligne <= -1 || ligne >= N) || (colonne <= -1 || colonne >= M))
    {
        return false;
    }
    return true;
}

void copieGrille(Case copie[N][M], Case grille[N][M])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            copie[i][j] = grille[i][j];
        }
    }
}

Case initialiseCase()
{
    Case newcase;
    newcase.couleur = 0;
    newcase.etat = 0;
    newcase.estRotation = false;
    return newcase;
}

void initialiseGrille(Case grille[N][M])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            grille[i][j] = initialiseCase(); // NULL correspond a qu'il n'y a pas de tétrominos sur cette case peut potentiellement être changer
        }
    }
}

tetris initialiseStruct(void)
{
    tetris tetris;
    initialiseGrille(tetris.grille);
    tetris.cycle_index = 0;
    tetris.score = 0;
    tetris.niveau = 1;
    tetris.estGameOver = false;
    tetris.lignes = 0;
    tetris.son = true;
    char tab[7] = {'I', 'O', 'T', 'L', 'J', 'Z', 'S'};
    srand(time(NULL));

    for (int i = 0; i < 4; i++)
    {
        tetris.cycle[i][0] = tab[(rand() % 7)];
        tetris.cycle[i][1] = rand() % 9 + 1 + '0';
    }
    return tetris;
}

bool estLibre(const tetris *tetris, int i, int j)
{
    if (estDansGrille(i, j) == false)
    {
        return false;
    }
    else
    {
        if (tetris->grille[i][j].etat < 2)
        { // Si la case est une case libre ou un tétromino en déplacement
            return true;
        }
        return false;
    }
}

bool estCoupValide(tetris *tetris, const int horizontale, const int verticlale)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (estTetroDeplacement(tetris, i, j))
            { // Est un tétrmonieau en déplacement
                if (estLibre(tetris, i + verticlale, j + horizontale) == false)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void setCaseLibre(Case grille[N][M], int i, int j)
{
    grille[i][j].etat = 0;
    grille[i][j].estRotation = false;
    grille[i][j].couleur = 0;
}

bool estTetroDeplacement(tetris *tetris, int i, int j)
{
    if ((*tetris).grille[i][j].etat == 1)
    {
        return true;
    }
    return false;
}

void realiseCoupHorizontale(tetris *tetris, int horizontale)
{
    if (estCoupValide(tetris, horizontale, 0))
    {
        system("mpg123 mouvement.mp3 &");
        if (horizontale == 1)
        {
            for (int i = 0; i <= N - 1; i++)
            {
                for (int j = M - 1; j >= 0; j--)
                {
                    if (estTetroDeplacement(tetris, i, j))
                    {
                        tetris->grille[i][j + 1] = tetris->grille[i][j];
                        setCaseLibre(tetris->grille, i, j);
                    }
                }
            }
        }
        else if (horizontale == -1)
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < M; j++)
                {
                    if (estTetroDeplacement(tetris, i, j))
                    {
                        tetris->grille[i][j - 1] = tetris->grille[i][j];
                        setCaseLibre(tetris->grille, i, j);
                    }
                }
            }
        }
    }
}

bool realiseCoupVerticale(tetris *tetris, int verticale)
{
    if (estCoupValide(tetris, 0, verticale))
    {
        for (int i = N - 1; i >= 0; i--)
        {
            for (int j = 0; j < M; j++)
            {
                if (estTetroDeplacement(tetris, i, j))
                {
                    tetris->grille[i + verticale][j] = tetris->grille[i][j];
                    setCaseLibre(tetris->grille, i, j);
                }
            }
        }
        return false;
    }
    else
    {
        for (int i = N - 1; i >= 0; i--)
        {
            for (int j = 0; j < M; j++)
            {
                if (estTetroDeplacement(tetris, i, j))
                {
                    tetris->grille[i][j].etat = 2;
                }
            }
        }
        supprimesLignes(tetris);
        return partieTerminee(tetris);
    }
}

void trouvePointRotation(tetris *tetris, int *ligne, int *colonne)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (tetris->grille[i][j].estRotation == true && estTetroDeplacement(tetris, i, j))
            {
                *ligne = i;
                *colonne = j;
                return;
            }
        }
    }
}

bool estRotationValide(tetris *tetris, Case copie[N][M])
{
    int ligne = -1;
    int colonne = -1;
    trouvePointRotation(tetris, &ligne, &colonne);
    if (ligne == -1)
    {
        return false;
    }
    int nouvPointX;
    int nouvPointY;
    int rotation = 1;
    for (int i = 2; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (estTetroDeplacement(tetris, i, j))
            {
                setCaseLibre(copie, i, j);
            }
        }
    }
    for (int i = 2; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (tetris->grille[i][j].etat == 1)
            {
                nouvPointX = colonne - (rotation * (i - ligne));
                nouvPointY = ligne + (rotation * (j - colonne));
                copie[nouvPointY][nouvPointX] = tetris->grille[i][j];
                if (estLibre(tetris, nouvPointY, nouvPointX) == false || nouvPointY < 2)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void previsualise(tetris *tetriss)
{
    tetris cptetris = initialiseStruct();
    cptetris.son = false;
    int index = cptetris.cycle_index;
    int indice = -1;
    cptetris.cycle[0][0] = ' ';
    cptetris.cycle[1][0] = ' ';
    copieGrille(cptetris.grille, tetriss->grille);
    while (index == cptetris.cycle_index)
    {
        indice++;
        realiseCoupVerticale(&cptetris, 1);
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (tetriss->grille[i][j].etat == 0 && tetriss->grille[i][j].couleur != 0)
            {
                tetriss->grille[i][j].couleur = 0;
            }
        }
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (estTetroDeplacement(tetriss, i, j))
            {
                tetriss->grille[i + indice][j].couleur = tetriss->grille[i][j].couleur;
            }
        }
    }
}

void realiseRotation(tetris *tetriss)
{
    Case cpgrille[N][M];
    copieGrille(cpgrille, tetriss->grille);
    if (estRotationValide(tetriss, cpgrille) == false)
    {
        tetris cptetris = initialiseStruct();
        cptetris.son = false;
        copieGrille(cptetris.grille, tetriss->grille);
        realiseCoupHorizontale(&cptetris, -1);
        copieGrille(cpgrille, cptetris.grille);
        if (estRotationValide(&cptetris, cpgrille) == false)
        {
            copieGrille(cptetris.grille, tetriss->grille);
            realiseCoupHorizontale(&cptetris, 1);
            copieGrille(cpgrille, cptetris.grille);
            if (estRotationValide(&cptetris, cpgrille) == false)
            {
                copieGrille(cptetris.grille, tetriss->grille);
                realiseCoupVerticale(&cptetris, 1);
                copieGrille(cpgrille, cptetris.grille);
                if (estRotationValide(&cptetris, cpgrille) == false)
                {
                    return;
                }
            }
        }
    }
    copieGrille(tetriss->grille, cpgrille);
}

void cycleTetrmonios(tetris *tetris)
{
    char tab[7] = {'I', 'O', 'T', 'L', 'J', 'Z', 'S'};
    srand(time(NULL));
    int i = (tetris->cycle_index + 4) % 8;
    for (; i < (tetris->cycle_index + 4) % 8 + 4; i++)
    {
        tetris->cycle[i][0] = tab[(rand() % 7)];
        tetris->cycle[i][1] = rand() % 9 + 1 + '0'; // En caractère
    }
}

void CycleSommet(tetris *tetris)
{
    char type = tetris->cycle[tetris->cycle_index][0];
    int couleur = tetris->cycle[tetris->cycle_index][1] - '0';
    if (tetris->cycle_index == 0 || tetris->cycle_index == 4)
    {
        cycleTetrmonios(tetris);
    }
    tetris->cycle_index = (tetris->cycle_index + 1) % 8;
    int x = M / 2;

    Case newCase = initialiseCase();
    newCase.etat = 1;
    newCase.couleur = couleur;
    Case newCase2 = newCase;
    newCase2.estRotation = true;
    switch (type)
    {
    case 'I':
        tetris->grille[0][x - 1] = newCase;
        tetris->grille[0][x] = newCase2;
        tetris->grille[0][x + 1] = newCase;
        tetris->grille[0][x + 2] = newCase;
        break;

    case 'O':
        tetris->grille[0][x] = newCase;
        tetris->grille[0][x + 1] = newCase;
        tetris->grille[1][x] = newCase;
        tetris->grille[1][x + 1] = newCase;
        break;

    case 'T':
        tetris->grille[0][x - 1] = newCase;
        tetris->grille[0][x] = newCase2;
        tetris->grille[0][x + 1] = newCase;
        tetris->grille[1][x] = newCase;
        break;

    case 'L':
        tetris->grille[0][x - 1] = newCase;
        tetris->grille[0][x] = newCase2;
        tetris->grille[0][x + 1] = newCase;
        tetris->grille[1][x - 1] = newCase;
        break;

    case 'J':
        tetris->grille[0][x - 1] = newCase;
        tetris->grille[0][x] = newCase2;
        tetris->grille[0][x + 1] = newCase;
        tetris->grille[1][x + 1] = newCase;
        break;

    case 'Z':
        tetris->grille[0][x - 1] = newCase;
        tetris->grille[0][x] = newCase;
        tetris->grille[1][x] = newCase2;
        tetris->grille[1][x + 1] = newCase;
        break;

    case 'S':
        tetris->grille[0][x] = newCase;
        tetris->grille[0][x + 1] = newCase;
        tetris->grille[1][x] = newCase2;
        tetris->grille[1][x - 1] = newCase;
        break;
    }

    realiseCoupVerticale(tetris, 1);
    realiseCoupVerticale(tetris, 1);
}

bool partieTerminee(tetris *tetris)
{
    // On vérifie les 2 premières lignes (lignes invisibles)
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (tetris->grille[i][j].etat == 2)
            {
                return true; // Il y a un bloc -> partie terminée
            }
        }
    }
    CycleSommet(tetris);
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (tetris->grille[i][j].etat == 2)
            {
                return true; // Il y a un bloc -> partie terminée
            }
        }
    }
    return false; // Tout est vide -> partie continue
}

void supprimesLignes(tetris *tetris)
{
    int score[5] = {0, 40, 100, 300, 1200};
    int nbLigne = 0;
    int *ptindice = calloc(0, sizeof(int));
    int *ptcopie;
    if (ptindice == NULL)
    {
        printf("Pas assez de mémoire\n");
        exit(1);
    }
    for (int i = 2; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (tetris->grille[i][j].etat != 2)
            {
                break;
            }
            else if (j == M - 1)
            {
                nbLigne = nbLigne + 1;
                ptcopie = realloc(ptindice, nbLigne * sizeof(int));
                if (ptcopie == NULL)
                {
                    printf("Pas assez de mémoire\n");
                    exit(1);
                }
                ptindice = ptcopie;
                ptindice[nbLigne - 1] = i;
                for (int z = 0; z < M; z++)
                {
                    setCaseLibre(tetris->grille, i, z);
                }
            }
        }
    }
    tetris->lignes = tetris->lignes + nbLigne;
    if (nbLigne == 4 && tetris->son == true)
    {
        system("mpg123 -q ligne4.mp3 &");
    }
    else if (nbLigne >= 1 && tetris->son == true)
    {
        system("mpg123 -q ligne.mp3 &");
    }
    for (int z = 0; z <= nbLigne - 1; z++)
    {
        for (int i = ptindice[z] - 1; i >= 0; i--)
        {
            for (int j = 0; j < M; j++)
            {
                tetris->grille[i + 1][j] = tetris->grille[i][j];
                setCaseLibre(tetris->grille, i, j);
            }
        }
    }
    if ((tetris->lignes / (tetris->niveau * 10)) >= 1)
    {
        tetris->niveau++;
        system("mpg123 -q niveau.mp3 &");
        demandeTemporisation(0.8f * pow(0.8f, tetris->niveau) * 1000);// formule qui donne le nouveau temps selon le niveau la fonction pow correspond a exposant
    }
    tetris->score = tetris->score + (score[nbLigne] * tetris->niveau);
}

// void affichage(tetris *tetris)
// {
//     printf("SCORE : %d", tetris->score);
//     for (int i = 2; i < N; i++)
//     {
//         for (int j = 0; j < M; j++)
//         {
//             if (tetris->grille[i][j].etat > 0)
//             {
//                 printf("□ ");
//             }
//             else
//             {
//                 printf(". ");
//             }
//         }
//         printf("\n");
//     }
// }

void genererClavier(tetris *tetris)
{
    char touche = caractereClavier();
    switch (touche)
    {
    case 'D':
    case 'd':
        realiseCoupHorizontale(tetris, 1);
        break;

    case 'Q':
    case 'q':
        realiseCoupHorizontale(tetris, -1);
        break;

    case 'S':
    case 's':
        tetris->score++;
        system("mpg123 mouvement.mp3 &");
        if (realiseCoupVerticale(tetris, 1))
        {
            tetris->estGameOver = true;
        }
        break;
    case 'Z':
    case 'z':
        system("mpg123 rotation.mp3 &");
        realiseRotation(tetris);
        break;
    case ' ': // Espace
        system("mpg123 tombe.mp3 &");
        int indice = tetris->cycle_index;
        while (indice == tetris->cycle_index)
        {
            tetris->score = tetris->score + 2;
            if (realiseCoupVerticale(tetris, 1))
            {
                tetris->estGameOver = true;
            }
        }
        break;
    case 27: // ESC pr quitter
        arreterMusique();
        termineBoucleEvenements();
        break;

    default:
        printf("Touche non gérée: %c (%d)\n", touche, touche);
        printf("oui");
    }
}
void gestionEvenement(EvenementGfx evenement)
{
    typedef enum
    {
        MENU,
        EN_JEU,
        EN_PAUSE
    } EtatJeu;
    static EtatJeu etat = MENU;
    static tetris jeu;
    // static bool estGameOver = false;

    switch (evenement)
    {
    case Initialisation:
        jeu = initialiseStruct();
        jeu.son = true;
        CycleSommet(&jeu);
        demandeTemporisation(800);
        rafraichisFenetre();
        break;

    case Affichage:
        if (jeu.estGameOver == false)
        {
            previsualise(&jeu);
        }

        effaceFenetre(15, 15, 15);

        // ECRAN MENU 
        if (etat == MENU)
        {
            rafraichisFenetre();
            effaceFenetre(15, 15, 15);

            couleurCourante(255, 255, 255);
            afficheChaine("TETRIS", 50, largeurFenetre() / 2 - 80, hauteurFenetre() - 100);

            float bouton_x1 = largeurFenetre() / 2 - 80;
            float bouton_y1 = hauteurFenetre() / 2 - 30;
            float bouton_x2 = largeurFenetre() / 2 + 80;
            float bouton_y2 = hauteurFenetre() / 2 + 30;

            couleurCourante(116, 208, 241);
            rectangle(bouton_x1, bouton_y1, bouton_x2, bouton_y2);

            couleurCourante(0, 0, 0);
            epaisseurDeTrait(2);
            afficheChaine("JOUER", 30, bouton_x1 + 30, bouton_y1 + 10);

            return;
        }

        // ECRAN PAUSE 
        if (etat == EN_PAUSE)
        {
            effaceFenetre(20, 20, 20);
            couleurCourante(255, 255, 255);
            afficheChaine("JEU EN PAUSE", 30, largeurFenetre() / 2 - 110, hauteurFenetre() - 100);

            const char *labels[] = {"Reprendre", "Recommencer", "Accueil"};
            for (int i = 0; i < 3; i++)
            {
                float x1 = largeurFenetre() / 2 - 100;
                float y1 = hauteurFenetre() / 2 - 40 - (i * 70);
                float x2 = x1 + 200;
                float y2 = y1 + 50;

                couleurCourante(116, 208, 241);
                rectangle(x1, y1, x2, y2);
                couleurCourante(255, 255, 255);
                afficheChaine(labels[i], 22, x1 + 40, y1 + 15);
            }

            return;
        }

        float referenceLargeur = 800.0; // largeur de base pour ton design
        float multiplicationLargeur = largeurFenetre() / referenceLargeur;
        float referenceHauteur = 600.0; // hauteur de base pour ton design
        float multiplicationHauteur = hauteurFenetre() / referenceHauteur;
        float multipli = multiplicationLargeur;
        if (multiplicationHauteur < multiplicationLargeur)
        {
            multipli = multiplicationHauteur;
        }
        // ECRAN DE JEU
        int nouvTaille = multipli * TAILLE_CASE;
        float cadre_x1 = (largeurFenetre() - (M * nouvTaille) - 200) / 2;
        float cadre_y1 = (hauteurFenetre() - 20 * nouvTaille) / 2 + 20;
        float cadre_x2 = (largeurFenetre() + (M * nouvTaille) - 200) / 2;
        float cadre_y2 = (hauteurFenetre() + 20 * nouvTaille) / 2 + 20;

        couleurCourante(30, 30, 30);
        rectangle(cadre_x1, cadre_y1, cadre_x2, cadre_y2);

        couleurCourante(0, 0, 0);
        epaisseurDeTrait(2);
        for (int i = 0; i <= 20; i++)
        {
            float y = cadre_y1 + i * nouvTaille;
            ligne(cadre_x1, y, cadre_x2, y);
        }
        for (int j = 0; j <= M; j++)
        {
            float x = cadre_x1 + j * nouvTaille;
            ligne(x, cadre_y1, x, cadre_y2);
        }

        for (int i = 2; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                float x1 = cadre_x1 + j * nouvTaille;
                float y1 = cadre_y1 + (21 - i) * nouvTaille;
                float x2 = x1 + nouvTaille;
                float y2 = y1 + nouvTaille;

                if (jeu.grille[i][j].etat == 1 || jeu.grille[i][j].etat == 2)
                {
                    foncCouleur(jeu.grille[i][j].couleur);
                    rectangle(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
                }
                else
                {
                    if (jeu.grille[i][j].couleur != 0)
                    {
                        foncCouleur(jeu.grille[i][j].couleur);
                        epaisseurDeTrait(2);
                        float x1 = cadre_x1 + j * nouvTaille;
                        float y1 = cadre_y1 + (21 - i) * nouvTaille;
                        float x2 = x1 + nouvTaille;
                        float y2 = y1 + nouvTaille;
                        ligne(x1, y1, x1, y2);
                        ligne(x2, y1, x2, y2);
                        ligne(x1, y1, x2, y1);
                        ligne(x1, y2, x2, y2);
                    }
                }
            }
        }

        // Affichage prochaine pièce
        float nextBox_x1 = cadre_x2 + 50 * multipli;
        float nextBox_y1 = cadre_y2 - 120 * multipli;
        float nextBox_x2 = nextBox_x1 + 325 * multipli;
        float nextBox_y2 = nextBox_y1 + 80 * multipli;

        couleurCourante(48, 48, 48);
        rectangle(nextBox_x1, nextBox_y1, nextBox_x2, nextBox_y2);
        dessineMiniTetromino(jeu.cycle[jeu.cycle_index][0], nextBox_x1, nextBox_y2 - 20 * multipli, jeu.cycle[jeu.cycle_index][1] - '0', multipli);
        dessineMiniTetromino(jeu.cycle[(jeu.cycle_index + 1) % 8][0], nextBox_x1 + 100 * multipli, nextBox_y2 - 20 * multipli, jeu.cycle[(jeu.cycle_index + 1) % 8][1] - '0', multipli);
        dessineMiniTetromino(jeu.cycle[(jeu.cycle_index + 2) % 8][0], nextBox_x1 + 200 * multipli, nextBox_y2 - 20 * multipli, jeu.cycle[(jeu.cycle_index + 2) % 8][1] - '0', multipli);
        // Affichage score
        float scoreBox_x1 = nextBox_x1;
        float scoreBox_y1 = nextBox_y1 - 50 * multipli;

        couleurCourante(255, 255, 255);
        char bufferScore[50];
        sprintf(bufferScore, "Score : %d", jeu.score);
        afficheChaine(bufferScore, 30 * multipli, scoreBox_x1 + 10 * multipli, scoreBox_y1 + 10 * multipli);
        char bufferNiveau[50];
        sprintf(bufferNiveau, "Niveau : %d", jeu.niveau);
        afficheChaine(bufferNiveau, 30 * multipli, scoreBox_x1 + 10 * multipli, scoreBox_y1 - 30 * multipli);
        char bufferLignes[50];
        sprintf(bufferLignes, "Lignes supprimes : %d", jeu.lignes);
        afficheChaine(bufferLignes, 25 * multipli, scoreBox_x1 + 10 * multipli, scoreBox_y1 - 70 * multipli);

        afficheChaine("Q=Gauche  D=Droite", 22 * multipli, scoreBox_x1 + 10 * multipli, scoreBox_y1 - 130 * multipli);
        afficheChaine("S=Bas  Z=Rotation", 22 * multipli, scoreBox_x1 + 10 * multipli, scoreBox_y1 - 170 * multipli);
        afficheChaine("Espace=Chute  ESC=Quitter", 18 * multipli, scoreBox_x1 + 10 * multipli, scoreBox_y1 - 210 * multipli);

        // BOUTON REJOUER 
        float boutonRejouer_x1 = largeurFenetre() - 130;
        float boutonRejouer_y1 = hauteurFenetre() - 50;
        float boutonRejouer_x2 = largeurFenetre() - 20;
        float boutonRejouer_y2 = hauteurFenetre() - 20;

        couleurCourante(159, 232, 85);
        rectangle(boutonRejouer_x1, boutonRejouer_y1, boutonRejouer_x2, boutonRejouer_y2);
        couleurCourante(255, 255, 255);
        afficheChaine("Rejouer", 18, boutonRejouer_x1 + 23, boutonRejouer_y1 + 10);

        // BOUTON ARRETER MUSIQUE 
        float boutonStopMusique_x1 = largeurFenetre() - 250;
        float boutonStopMusique_y1 = hauteurFenetre() - 50;
        float boutonStopMusique_x2 = largeurFenetre() - 140;
        float boutonStopMusique_y2 = hauteurFenetre() - 20;

        couleurCourante(120, 120, 120);
        rectangle(boutonStopMusique_x1, boutonStopMusique_y1, boutonStopMusique_x2, boutonStopMusique_y2);
        if (jeu.son == true)
        {
            couleurCourante(255, 255, 255);
            afficheChaine("Son OFF", 18, boutonStopMusique_x1 + 20, boutonStopMusique_y1 + 10);
        }
        else
        {
            couleurCourante(255, 255, 255);
            afficheChaine("Son ON", 18, boutonStopMusique_x1 + 20, boutonStopMusique_y1 + 10);
        }

        // BOUTON PAUSE 
        float boutonPause_x1 = 20;
        float boutonPause_y1 = hauteurFenetre() - 50;
        float boutonPause_x2 = 130;
        float boutonPause_y2 = hauteurFenetre() - 20;

        couleurCourante(200, 80, 80);
        rectangle(boutonPause_x1, boutonPause_y1, boutonPause_x2, boutonPause_y2);
        couleurCourante(255, 255, 255);
        afficheChaine("Pause", 18, boutonPause_x1 + 25, boutonPause_y1 + 8);

        // GAME OVER
        if (jeu.estGameOver)
        {
            arreterMusique();
            system("mpg123 fin.mp3 &");
            const char *msg = "GAME OVER";
            float taille = 80;
            float largeur = tailleChaine(msg, taille * multipli);
            float x = (largeurFenetre() - largeur) / 2.0;
            float y = hauteurFenetre() / 2.0;

            couleurCourante(255, 0, 0);
            afficheChaine(msg, taille * multipli, x + 1, y - 1);

            couleurCourante(255, 0, 0);
            afficheChaine(msg, taille * multipli, x, y);
        }

        break;

    case BoutonSouris:
    {
        if (etatBoutonSouris() == GaucheAppuye)
        {
            int x = abscisseSouris();
            int y = ordonneeSouris();

            // === MENU PRINCIPAL : clic sur "JOUER"
            if (etat == MENU)
            {
                float bouton_x1 = largeurFenetre() / 2 - 80;
                float bouton_y1 = hauteurFenetre() / 2 - 30;
                float bouton_x2 = largeurFenetre() / 2 + 80;
                float bouton_y2 = hauteurFenetre() / 2 + 30;

                if (x >= bouton_x1 && x <= bouton_x2 && y >= bouton_y1 && y <= bouton_y2)
                {
                    jeu = initialiseStruct();
                    CycleSommet(&jeu);
                    etat = EN_JEU;
                    lancerMusique(&jeu);
                    rafraichisFenetre();
                }
            }

            // EN_JEU clic sur Pause ou Rejouer ou son
            else if (etat == EN_JEU)
            {
                float pause_x1 = 20;
                float pause_y1 = hauteurFenetre() - 50;
                float pause_x2 = 130;
                float pause_y2 = hauteurFenetre() - 20;

                float rejouer_x1 = largeurFenetre() - 130;
                float rejouer_y1 = hauteurFenetre() - 50;
                float rejouer_x2 = largeurFenetre() - 20;
                float rejouer_y2 = hauteurFenetre() - 20;

                float stopMusique_x1 = largeurFenetre() - 250;
                float stopMusique_y1 = hauteurFenetre() - 50;
                float stopMusique_x2 = largeurFenetre() - 140;
                float stopMusique_y2 = hauteurFenetre() - 20;

                if (x >= pause_x1 && x <= pause_x2 && y >= pause_y1 && y <= pause_y2)
                {
                    etat = EN_PAUSE;
                    arreterMusique();
                    rafraichisFenetre();
                    return;
                }
                else if (x >= rejouer_x1 && x <= rejouer_x2 && y >= rejouer_y1 && y <= rejouer_y2)
                {
                    bool ancienSon = jeu.son;
                    jeu = initialiseStruct();
                    jeu.son = ancienSon;

                    CycleSommet(&jeu);
                    etat = EN_JEU;
                    arreterMusique();
                    lancerMusique(&jeu);
                    rafraichisFenetre();
                    return;
                }
                else if (x >= stopMusique_x1 && x <= stopMusique_x2 && y >= stopMusique_y1 && y <= stopMusique_y2)
                {
                    if (jeu.son == false)
                    {
                        jeu.son = true;
                    }
                    else
                    {
                        jeu.son = false;
                    }
                    lancerMusique(&jeu);
                    rafraichisFenetre();
                    return;
                }
            }

            // === EN_PAUSE : clic sur les 3 boutons
            else if (etat == EN_PAUSE)
            {
                for (int i = 0; i < 3; i++)
                {
                    float x1 = largeurFenetre() / 2 - 100;
                    float y1 = hauteurFenetre() / 2 - 40 - (i * 70);
                    float x2 = x1 + 200;
                    float y2 = y1 + 50;

                    if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
                    {
                        if (i == 0) // Reprendre
                        {
                            etat = EN_JEU;
                            lancerMusique(&jeu);
                        }
                        else if (i == 1) //Recommencer
                        {
                            bool ancienSon = jeu.son;
                            jeu = initialiseStruct();
                            jeu.son = ancienSon;

                            CycleSommet(&jeu);
                            etat = EN_JEU;
                            lancerMusique(&jeu);
                        }
                        else if (i == 2) //  Accueil
                        {
                            etat = MENU;
                            arreterMusique();
                        }
                        rafraichisFenetre();
                    }
                }
            }
        }
        break;
    }

    case Clavier:
        if (!jeu.estGameOver)
        {
            genererClavier(&jeu);
            // affichage(&jeu);
            rafraichisFenetre();
        }
        if (etat != EN_JEU)
            break;

        break;

    case Temporisation:
        if (etat != EN_JEU)
            break;

        if (jeu.estGameOver)
            break;

        jeu.estGameOver = realiseCoupVerticale(&jeu, 1);
        rafraichisFenetre();

        if (etat != EN_JEU)
            break;

        break;

    default:
        break;
    }
}

void foncCouleur(int couleur)
{
    switch (couleur)
    {
    case 1:
        couleurCourante(173, 54, 196);
        break;
    case 2:
        couleurCourante(246, 128, 0);
        break;
    case 3:
        couleurCourante(0, 198, 232);
        break;
    case 4:
        couleurCourante(35, 98, 249);
        break;
    case 5:
        couleurCourante(254, 1, 177);
        break;
    case 6:
        couleurCourante(0, 211, 0);
        break;
    case 7:
        couleurCourante(255, 225, 0);
        break;
    case 8:
        couleurCourante(0, 86, 27);
        break;
    case 9:
        couleurCourante(0, 211, 0);
        break;
    default:
        couleurCourante(0, 198, 232);
        break;
    }
}

void dessineMiniTetromino(char type, float xOffset, float yOffset, int couleur, float multipli)
{
    int forme[4][4] = {{0}};
    switch (type)
    {
    case 'I':
        forme[1][0] = forme[1][1] = forme[1][2] = forme[1][3] = 1;
        break;
    case 'O':
        forme[1][1] = forme[1][2] = forme[2][1] = forme[2][2] = 1;
        break;
    case 'T':
        forme[1][1] = forme[1][0] = forme[1][2] = forme[2][1] = 1;
        break;
    case 'L':
        forme[1][0] = forme[1][1] = forme[1][2] = forme[2][0] = 1;
        break;
    case 'J':
        forme[1][0] = forme[1][1] = forme[1][2] = forme[2][2] = 1;
        break;
    case 'S':
        forme[1][1] = forme[1][2] = forme[2][0] = forme[2][1] = 1;
        break;
    case 'Z':
        forme[1][0] = forme[1][1] = forme[2][1] = forme[2][2] = 1;
        break;
    }

    float taille = TAILLE_CASE * 0.9 * multipli;

    int minI = 4, maxI = -1, minJ = 4, maxJ = -1;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (forme[i][j])
            {
                if (i < minI)
                    minI = i;
                if (i > maxI)
                    maxI = i;
                if (j < minJ)
                    minJ = j;
                if (j > maxJ)
                    maxJ = j;
            }

    int hauteur = maxI - minI + 1;
    int largeur = maxJ - minJ + 1;

    float totalWidth = largeur * taille;
    float totalHeight = hauteur * taille;

    float correctionX = -10 * multipli;

    float startX = xOffset + (150 * multipli - totalWidth) / 2.0 + correctionX;
    float startY = yOffset - (80 * multipli - totalHeight) / 2.0;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (forme[i][j])
            {
                float x1 = startX + (j - minJ) * taille;
                float y1 = startY - (i - minI) * taille;
                float x2 = x1 + taille;
                float y2 = y1 + taille;
                foncCouleur(couleur);
                rectangle(x1 + 1, y1 + 1, x2 - 1, y2 - 1);

                couleurCourante(0, 0, 0);
                epaisseurDeTrait(1);
                ligne(x1, y1, x2, y1);
                ligne(x2, y1, x2, y2);
                ligne(x2, y2, x1, y2);
                ligne(x1, y2, x1, y1);
            }
        }
    }
}
