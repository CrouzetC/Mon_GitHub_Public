
#pragma once

#include <stdlib.h>
#include <stdio.h>


/*** D�finitions ***/

#ifndef PI
#define PI 3.141592654
#endif

// comme 'graphic_set' mais pour les cas o� tous les �l�ments sont du m�me type.
typedef struct _general_set {

	int nb;
	void** elems;  // tableau de pointeurs vers les �l�ments

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
// Id�e : faire une simulation avec :
//       des points dispers�s, et ceux-ci doivent s'aligner entre eux en s'�vitant les uns les autres
//		 (cf la fa�on dont les niveau d'allignent dans les invocations synchro de YGO)
//       -> on peut faire 1 force de r�pulsion entre les niveaux + une 'fosse de potentiel' (=> donne la ligne o� les point sdoivent se placer)
//

/*** Fonctions : ***/


general_set*  create_general_set ();
void  add_elem    (general_set* set, void* elem);
void  remove_elem (general_set* set, void* elem);
/*
    destroy_set(...)  NE d�truit PAS les �l�ments qu'il contient 
		en effet, on ne sait pas de quel type d'�l�ments il s'agit.
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
				il faut s'assurer que la taille des floats dans la machine ayant ex�cut� write_float(...)
				EST LA MEME que celle des floats dans la machine ex�cutant read_float(...).
*/
void write_float(FILE* file, float f);
/*
	Lit 4 octets dans le fichier 'file' et renvoie le flottant 'f' cod� par ces octets
		(le premier octet lu est celui d'adresse : &f,
		le deuxi�me octet lu est celui d'adresse : ((char*)(&f)) + 1,
		et le dernier octet lu est celui d'adresse : ((char*)(&f)) + sizeof(float) - 1
*/
float read_float(FILE* file);
