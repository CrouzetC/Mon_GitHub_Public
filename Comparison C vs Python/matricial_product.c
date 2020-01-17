
#include <stdio.h>
#include <stdlib.h>

void init_matrice_0(float** M, int lignes, int colonnes) {

    int ligne; int colonne;
    for (ligne = 0; ligne < lignes; ligne ++) {
        for (colonne = 0; colonne < colonnes; colonne ++) {
            *( *(M+ligne)+colonne ) = 0.0;
        }
    }
    return ;
}

void init_matrice(float** M, int lignes, int colonnes) {

    int ligne; int colonne;
    for (ligne = 0; ligne < lignes; ligne ++) {
        for (colonne = 0; colonne < colonnes; colonne ++) {
            *( *(M+ligne)+colonne ) = (float) (ligne+colonne);
        }
    }
    return ;
}

void produit_matriciel(float** A, float** B, float** C, int i, int j, int k) {  // Il faut que les matrices aient les tailles : A (i,k) ; B (k,j) ; C(i,j)

    int i_; int j_; int k_;
    float s;
    for (i_ = 0; i_ < i; i_ ++) {
        for (j_ = 0; j_ < j; j_ ++) {
            s = 0;
            for (k_ = 0; k_ < k; k_ ++) {
                s += *( *(A+i_) + k_ ) * *( *(B+k_) + j_ );
            }
            *( *(C+i_) + j_ ) = s;
        }
    }
    return ;
}

void affiche_matrice(float** M, int lignes, int colonnes) {

    int ligne;
    int colonne;
    float f;
    for (ligne = 0; ligne < lignes; ligne ++) {
        for (colonne = 0; colonne < colonnes; colonne ++) {
            f = *( *(M+ligne) + colonne );
            printf("%f ;   ", f);
        }
        printf("\n");
    }
    printf("\n");
    return ;
}

float** creation_matrice(int lignes, int colonnes) {

    float** M = malloc(lignes * sizeof(float*));

    int ligne;
    for (ligne = 0; ligne < lignes; ligne ++) {
        *(M+ligne) = malloc(colonnes * sizeof(float));
    }
    return M;
}

void free_matrice(float** M, int lignes) {

    int ligne = 0;
    for (ligne = 0; ligne < lignes; ligne ++) {
        free(*(M+ligne));
    }
    free(M);
    return ;
}

void start_simple() {

    int n;
    char input[12]; // Max 10 chiffres pour n

    printf("\nTaille des matices : ");
    fgets(input, 12, stdin);
    sscanf(input, "%d\n", &n);
    printf("\n");

    float** A = creation_matrice(n, n);
    init_matrice(A, n, n);
    float** B = creation_matrice(n, n);
    init_matrice(B, n, n);

    float** C = creation_matrice(n, n);
    init_matrice_0(C, n, n);

    produit_matriciel(A, B, C, n, n, n);

    printf("Matrice A : \n");
    affiche_matrice(A, n, n);
    printf("Matrice B : \n");
    affiche_matrice(B, n, n);
    printf("Matrice C = A * B : \n");
    affiche_matrice(C, n, n);

    free_matrice(A, n);
    free_matrice(B, n);
    free_matrice(C, n);
    return ;
}


int main() {

    start_simple();

    return 0;
}

