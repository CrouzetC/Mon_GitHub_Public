
#pragma once

#include "util.h"
#include "picture.h"/* A SUPPRIMER :
                      // a priori, nécessaire même s'il y a 'graphic.h',
					  // car on peut si on importe 'graphic_elem.h' dans 'graphic.h',
					  // on n'a pas encore importé 'picture.h'
                    */
#include "graphic.h"

/*** Définitions ***/

/* A SUPPRIMER :
typedef enum _alpha_blending_mode alpha_blending_mode;
typedef enum _scalar_type scalar_type;
typedef struct _pixel pixel;
typedef struct _picture picture;  // nécessaire même s'il y a '#include picture.h',
								  // MAIS je ne sais PAS pourquoi !!!
*/

typedef struct _line {  // Ligne droite
	int x_bottom_left;
	int x_top_right;
	int y_bottom_left;
	int y_top_right;
	float r, g, b, a;
} line;

typedef struct _square {
	int x_bottom_left;
	int x_top_right;
	int y_bottom_left;
	int y_top_right;
	float r, g, b, a;
} square;

typedef struct _empty_square {
	int x_bottom_left;
	int x_top_right;
	int y_bottom_left;
	int y_top_right;
	float r, g, b, a;
} empty_square;

typedef struct _vertex_set {
	int nb_vertex;
	int* x_tab;
	int* y_tab;
	float* pixels;  // tableau de 4 * nb_vertex valeur flottantes
} vertex_set;


/*** Fonctions : ***/

square*        create_square (int x_bottom_left, int y_bottom_left, int size_x, int size_y, pixel* pix);

empty_square*  create_empty_square (int x_bottom_left, int y_bottom_left, int size_x, int size_y, pixel* pix);

vertex_set*    create_vertex_set   (int nb_vertex);

vertex_set*    create_curved_link  (int x_left, int y_left, int x_right, int y_right,
									pixel* pix_left       ,	pixel* pix_right          );


void  destroy_square       (square*       square);
void  destroy_empty_square (empty_square* square);
void  destroy_vertex_set   (vertex_set*   verts);

// pour décaler de (dx;dy) :
void  shift_line  (line* line_pt, int dx, int dy);
void  shift_square  (square* square_pt, int dx, int dy);
void  shift_empty_square  (empty_square* square_pt, int dx, int dy);
void  shift_vertex_set  (vertex_set* verts_pt, int dx, int dy);

void  square_to_picture       (picture* pic, square* sq,        alpha_blending_mode MODE);
void  empty_square_to_picture (picture* pic, empty_square* sq,  alpha_blending_mode MODE);
void  vertex_set_to_picture   (picture* pic, vertex_set* verts, alpha_blending_mode MODE);


/*
	" set_pixel_in_vertex_set (set, 3, pix) " met le 3ème pixel de 'set' aux valeurs de 'pix'
*/
void  set_pixel_in_vertex_set (vertex_set* set, int pixel_index, int x, int y, pixel* pix);


