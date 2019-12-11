
#pragma once

/* A SUPPRIMER :
#include "picture.h"
#include "graphic_elem.h"
#include "complex.h"  // pour 'complex_1D_graph'

struct _picture;
typedef struct _picture picture;
*/


/*** D�finitions ***/

typedef enum _graphic_type {
	GRAPHIC_SET,
	PICTURE,
	COMPLEX_1D_GRAPH,
	LINE,
	VERTEX_SET,
	SQUARE,
	EMPTY_SQUARE
} graphic_type;

typedef enum _alpha_blending_mode {  // Voir sur Wikipedia, ou l'image "Alpha Blending (Wikipedia).png" dans D:\Fichiers
	REPLACE,
	OVER,
	XOR
} alpha_blending_mode;

typedef enum _scalar_type {  // Pour la conversion : scalaire -> pixel
	RGB,
	BLACK,
	WHITE
} scalar_type;

typedef struct _graphic_set {

	int nb;
	void** elements;
	graphic_type* elements_types;
	/*
	   Exemple : elements = { [pic1], [pic1], [vertexs],} ;
	             elements_types = { PICTURE, PICTURE, VERTEX_SET }
	*/
} graphic_set;

typedef struct _pixel {
	float r;
	float g;
	float b;
	float a;
} pixel;

typedef struct _picture picture;


/*** Fonctions : ***/

graphic_set* create_set   ();
pixel*       create_pixel (float r, float g, float b, float a);
void  destroy_set   (graphic_set* set);
void  destroy_pixel (pixel* pix);

// pour d�caler chaque �l�ment de (dx;dy) :
void shift_graphic_set (graphic_set* set, int dx, int dy);

/*
    remove_graphic_elem() retire l'�l�ment d'adresse 'elem' de 'set' mais ne le d�truit pas
	(on suppose qu'un �l�ment n'est pr�sent qu'une fois dans le 'set')
*/
void add_graphic_elem    (graphic_set* set, void* elem, graphic_type TYPE);
void remove_graphic_elem (graphic_set* set, void* elem);

void set_to_picture (picture* pic, graphic_set* set, alpha_blending_mode MODE);

/*
    'pixel' est un tableau : pixel[0] = R, pixel[1] = G, pixel[2] = B, pixel[3] = A
*/
void float_to_pixel (float* pixel, float scalar, scalar_type MODE);

/*
	'pixel' est un tableau : pixel[0] = R, pixel[1] = G, pixel[2] = B, pixel[3] = A
	set_pixel (...) modifie 'pixel' pour lui ajouter les valeurs (r, g, b, a) selon le MODE demand�
*/
void set_pixel (float* pixel, float r, float g, float b, float a, alpha_blending_mode MODE);

// pour appliquer des couleurs � un 'pixel', sans choix de mode (MODE = REPLACE)
void set_pixel (pixel* pix, float r, float g, float b, float a);
void set_pixel (pixel* destination_pix, pixel* source_pix);

