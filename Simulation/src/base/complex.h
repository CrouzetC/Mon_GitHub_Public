
#pragma once

#include "util.h"
#include "picture.h"
#include "graphic.h"

/* A SUPPRIMER :
  // Je ne sais PAS pourquoi, mais il faut ces typedef : ...
typedef struct _graphic_set graphic_set;
typedef struct _picture picture;
*/

//typedef struct _pixel pixel;


typedef enum _behaviour_at_border {

	ZERO,      // on fait comme si : re[-1] = 0.0
	LOOP,      // on fait comme si : re[-1] = re[length-1]
	CONSTANT,  // on fait comme si : re[-1] = re[0]
	CONSTANT_DERIVATIVE,  // on fait comme si la dérivée était constante au bord
	CONSTANT_2ND_DERIVATIVE  // on fait comme si la dérivée seconde était constante au bord (utile pour le laplacian(...))

} behaviour_at_border;


// Attention : _complex est déjà défini dans 'corecrt_math.h' ( par "struct { double x, y; };" )

typedef struct _my_complex { 

	// ATTENTION : le code utilise des affectations du type : " complex c = { 1.0, 2.0 } "
	// donc il ne faut pas modifier l'ordre des attributs ci-dessous sous peine d'inverser 're' et 'im' dans les affectations évoquées précédemment !

	double re;
	double im;
} complex;


typedef struct _complex_1D {
	double* re;  // parties réelles
	double* im;  // parties imaginaires
	int length;
	double dx;   // 'espace' entre les valeurs
} complex_1D;

typedef struct _complex_2D {
	double* re;
	double* im;
	int width;
	int height;
	double dx;
	double dy;
} complex_2D;


/*** Fonctions principales ***/


complex_1D* create_complex_1D   (int length, double dx);
void        destroy_complex_1D  (complex_1D* field);

complex_2D* create_complex_2D   (int height, int width, double dx, double dy);
void        destroy_complex_2D  (complex_2D* field);



/***  Fonctions utiles  ***/


/** Dérivées **/

  // <=> return { field->re[i], field->im[i] }
  // (en prenant en compte les bords).
complex get_value(complex_1D* field, int index, behaviour_at_border MODE);

/*
	quality == 0 -> calcul basique en prenant 2 valeurs
	SI une dérivée est calculée entre 2 valeurs, le résultat est un décalage :
		- vers la droite si shift_direction == 1,
		- vers la gauche si shift_direction == -1.
*/
complex_1D* derivative_1D_field (complex_1D* field, int shift_direction, int quality, behaviour_at_border MODE);

complex_1D* laplacian_1D_field (complex_1D* field, int quality, behaviour_at_border MODE);

complex laplacian_1D (complex_1D* field, int index, behaviour_at_border MODE);

/*  DANS ce commentaire, on prend l'exemple size_fields_tab == 5, mais la fonction peut aussi marcher pour size_fields_tab != 5 (et >= 1)
		(cela dépend toutefois de quality).

    Calcule le nouveau champ complexe à partir de sa dérivée à l'instant considéré,

         à partir d'un tableau 'fields_tab' de 5 complex_1D* tels que :
            - fields_tab[0] est le champ à l'instant t-1
            - fields_tab[1] est le champ à l'instant t-2
            - fields_tab[2] est le champ à l'instant t-3
            - fields_tab[3] est le champ à l'instant t-4
            - fields_tab[4] est le champ à l'instant t-5

        et 'derivative' est la dérivée souhaitée du champ à l'instant courant.

        A la fin de l'exécution :
            - fields_tab[0] est le champ à l'instant t
            - fields_tab[1] est le champ à l'instant t-1
            - fields_tab[2] est le champ à l'instant t-2
            - fields_tab[3] est le champ à l'instant t-3
            - fields_tab[4] est le champ à l'instant t-4

        // ATTENTION : ici, on fait l'approximation : " f'(t) = ...f(t-5).....f(t-1) ",
            alors que la formule correcte est :  f'(t) = ...f(t-4).....f(t)
            (cela impliquerait de manipuler des formules propres à chaque équation différentielle étudiée).

        Cela est valable pour quality == 1, si quality == 0 alors on utilise " f'(t) = (f(t+1)-f(t)) / h ".
*/
void next_step_with_derivative (complex_1D** fields_tab, int size_fields_tab, complex_1D* derivative, double dt, int quality);


/** Calculs généraux **/

double complex_module (double re, double im);
double complex_arg    (double re, double im);

// donne l'intégrale de |field|^2  (en prenant en compte dx)
double  get_square_module_integral (complex_1D* field);

double  get_real_integral  (complex_1D* field);
double  get_imaginary_integral  (complex_1D* field);

// fait en sorte que l'intégrale de |field|^2 soit de 1.0
void  normalize_module_square (complex_1D* field);

// fait du 'field' une courbe gaussienne en prenant x = 0 :
// si 'field->length' est pair : entre les 2 valeurs centrales, sinon : à la valeur centrale
// (avec un argument de 0.0 en tout point : Re = |field| et Im = 0.0).
void  make_centered_gaussian (complex_1D* field, double sigma);


// calcule la FFT du signal source et place le résultat dans 'target' (qui doit avoir la même taille)
  // la source doit avoir une taille en 2^N
  // inversed = 0 donne la FFT,
  // inversed = 1 donne l'IFFT (Transformée de Fourier Inverse rapide).
void fft(complex_1D* source, complex_1D* target, int inversed);


/** Filtres **/

void filter_order_1(complex_1D** field, double tau);
void filter_order_1_rigorous(complex_1D** field, double tau);  // filtrage par convolution (avec le sens inversé par rapport à la convoluion ?) avec la réponse impultionnelle d'un filtre passe-bas : k/a*exp(-a*k)
void filter_2(complex_1D** field, double coeff);
/*
	filter_cutting_high_freq(complex_1D field, complex_1D target_for_fft, float percent_cut) :

		- filtre 'field' en supprimant ces harmoniques les plus élevées
		- 'percent_cut' est la proportion de fréquences à couper,
		ex : percent_cut = 0.50 -> les 50% des fréquences qui sont les plus élevées seront coupées
		(attention, on parle des hautes fréquences en considérant que la 2è moitié de 'target_for_fft' comme des fréquences négatives, et non-pas des fréquences plus élevées)
		- le filtre modifiera 'target_for_fft' pour en faire la transformée de Fourier du résultat.

		* 'target_for_fft' doit être créée et de même longueur que 'field',
		* mais ne doit pas forcément être déjà calculée (la FFT sera (re)calculée dans la fonction de filtrage)
*/
void filter_cutting_high_freq(complex_1D* field, complex_1D* target_for_fft, float percent_cut);
/*
    filter_attenuating_high_freq(...) :

        voir schéma sur image .png
*/
void filter_attenuating_high_freq(complex_1D* field, complex_1D* target_for_fft);


/***  Affichage  ***/


// donne la couleur associée à l'argument du nombre complexe donné en argument :
void  arg_to_pix (pixel* pix, double real, double imaginary);


typedef enum _complex_1D_graph_mode {
	MODULE_WITH_ARG,
	MODULE_NO_ARG,
	REAL,
	IMAGINARY
} complex_1D_graph_mode;

typedef struct _complex_1D_graph {
	
	int x_0;
	int y_0;  // ordonnée dans 'picture' du point (0;0) du graphe

	int size;  // nombre de 'valeurs' à afficher ( = taille de graph->y[] )
	int* y;    // ordonnées DANS LE GRAPHE (-> y_in_picture = y_0 + y[i])

	pixel* pixels;  // pixels[0] donne le pixel du point le plus à gauge de la ligne du graphe
	int line_width;

} complex_1D_graph;

/*
    complex_1D_graph_module(...) :

		si line_width >= 0 : trace une ligne d'épaisseur 'line_width'
		si line_width == 0 : trace une ligne et REMPLIT l'aire sous la courbe
		
		y_1 : ordonnée du point d'ordonnée 1.0 dans le graphe,
			  ex: pour la valeur 1.0 : y = y_0 + 1.0 * y_1
		
*/
complex_1D_graph*  create_complex_1D_graph (complex_1D* field,
											int x_0, int y_0, int y_1,
											int line_width, complex_1D_graph_mode MODE);

void  destroy_complex_1D_graph (complex_1D_graph* graph);

/*
	complex_1D_graph_to_picture(...) :

		il n'y a PAS de 'alpha_blending_mode' : c'est OVER par défaut.

*/
void  complex_1D_graph_to_picture (picture* pic, complex_1D_graph* graph);

// Fonction utile pour complex_1D_graph_to_picture(...)
void modify_Y_start_end_to_link_points(int* y_start, int* y_end, complex_1D_graph* graph, int i);


/*
    create_arg_reference() crée l'image d'un disque

		dont les couleurs montrent la correspondance "argument/couleur" pour les nombres complexes.

*/
picture* create_argument_reference (int x, int y, int width);

