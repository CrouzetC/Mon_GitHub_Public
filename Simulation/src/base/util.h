
#pragma once

#include <stdlib.h>
#include <stdio.h>


/*** Définitions ***/

#ifndef PI
#define PI 3.141592654
#endif

// comme 'graphic_set' mais pour les cas où tous les éléments sont du même type.
typedef struct _general_set {

	int nb;
	void** elems;  // tableau de pointeurs vers les éléments

} general_set;

typedef enum _direction {
	RIGHT,
	LEFT,
	TOP,
	BOTTOM,
} direction;

typedef struct _scalar_1D {
	float* data;
	int length;
} scalar_1D;

typedef struct _scalar_2D {
	float* data;
	int width;
	int height;
} scalar_2D;

//
// Idée : faire une simulation avec :
//       des points dispersés, et ceux-ci doivent s'aligner entre eux en s'évitant les uns les autres
//		 (cf la façon dont les niveau d'allignent dans les invocations synchro de YGO)
//       -> on peut faire 1 force de répulsion entre les niveaux + une 'fosse de potentiel' (=> donne la ligne où les point sdoivent se placer)
//

/*** Fonctions : ***/


general_set*  create_general_set ();
void  add_elem    (general_set* set, void* elem);
void  remove_elem (general_set* set, void* elem);
/*
    destroy_set(...)  NE détruit PAS les éléments qu'il contient 
		en effet, on ne sait pas de quel type d'éléments il s'agit.
*/
void  destroy_set (general_set* set);

scalar_1D*  create_scalar1D  (int length);
void      destroy_scalar1D  (scalar_1D* field);

scalar_2D*  create_scalar2D  (int height, int width);
void      destroy_scalar2D  (scalar_2D* field);

/*  new_file (...) :
	number_at_end == 1 : 'name 003.pic'
	number_at_end == 0 : '003 name.pic'
*/
FILE* new_file(char* path, int number_at_end);


float get_time_in_seconds();

int max (int a, int b);

float  get_rand_float();  // renvoie un flottant entre 0.0 et 1.0 (loi uniforme)
double get_rand_double();  // idem avec un double

// retourne l'exposant de la plus grande puissance de 2 qui est <= a
  // ex : pour 16, retourne 4 ; pour 17, retourne 4.
int my_log2 (int a);

/*
	Ecrit dans le fichier 'file' les 'sizeof(float)' octets codant le flottant 'f'.

	ATTENTION : avant d'utiliser write_float(...) ou read_float(...),
				il faut s'assurer que la taille des floats dans la machine ayant exécuté write_float(...)
				EST LA MEME que celle des floats dans la machine exécutant read_float(...).
*/
void write_float(FILE* file, float f);
/*
	Lit 4 octets dans le fichier 'file' et renvoie le flottant 'f' codé par ces octets
		(le premier octet lu est celui d'adresse : &f,
		le deuxième octet lu est celui d'adresse : ((char*)(&f)) + 1,
		et le dernier octet lu est celui d'adresse : ((char*)(&f)) + sizeof(float) - 1
*/
float read_float(FILE* file);
