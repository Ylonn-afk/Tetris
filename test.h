typedef struct
{
    int etat;
    int couleur;
    bool estRotation;
} Case;

typedef struct
{
    Case grille[N][M]; // grille de Case
    char cycle[9][2];  // Première position du tableau correspond au type du tetrominos deuxième correspond a ca couleur en char il faut donc faire -'0' si on veut l'avoir en entier
    int cycle_index;
    int score;
    int niveau;
    int lignes;
    bool estGameOver;
    bool son;
} tetris;

void dessineMiniTetromino(char type, float xOffset, float yOffset, int couleur, float multipli);

void foncCouleur(int couleur);

bool estDansGrille(int ligne, int colonne);

void copieGrille(Case copie[N][M], Case grille[N][M]);

void initialiseGrille(Case grille[N][M]);

tetris initialiseStruct(void);

bool estLibre(const tetris *tetris, int i, int j);

bool estCoupValide(tetris *tetris, const int horizontale, const int verticlale);

void setCaseLibre(Case grille[N][M], int i, int j);

bool estTetroDeplacement(tetris *tetris, int i, int j);

void realiseCoupHorizontale(tetris *tetris, int horizontale);

bool realiseCoupVerticale(tetris *tetris, int verticale);

void cycleTetrmonios(tetris *tetris);

bool partieTerminee(tetris *tetris);

void CycleSommet(tetris *tetris);

void affichage(tetris *tetris);

void supprimesLignes(tetris *tetris);

void trouvePointRotation(tetris *tetris, int *ligne, int *colonne);

bool estRotationValide(tetris *tetris, Case copie[N][M]);

void realiseRotation(tetris *tetris);

void genererClavier();

void previsualise(tetris *tetriss);

void arreterMusique();

void lancerMusique(tetris *jeu);