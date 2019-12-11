
#pragma once

#include "util.h"
#include "graphic.h"

/*** Définitions ***/

/* A SUPPRIMER :
typedef enum _alpha_blending_mode alpha_blending_mode;
typedef enum _scalar_type scalar_type;
*/

typedef struct _picture {
	float* pixels;
	int width;
	int height;
	int pos_x;
	int pos_y;
} picture;


/*** Fonctions : ***/


picture* create_picture  (int pos_x, int pos_y, int width, int height);
void     destroy_picture (picture* pic);

void     save_picture    (char* file_path, picture* pic, int number_at_end);
picture* read_picture    (char* file_path, int pos_x, int pos_y);

/*
    shift_picture() décale l'image de (dx;dy)
*/
void shift_picture (picture* pic, int dx, int dy);

/*
    pic_to_picture() applique l'image 'pic' à l'image 'screen'
*/
void pic_to_picture (picture* screen, picture* pic, alpha_blending_mode MODE);

/*
	create_background (...) :
		param == 0 : fond rouge sur les bords et noir au centre
		param == 1 : fond blanc sur les bords et cyan au centre
		param == 2 : fond uniforme gris foncé
		param == 3 : fond uniforme gris clair
*/
picture* create_background (int width, int height, int param);

/*
    size est l'épaisseur de la bande pour 'scalar1D'
*/
picture* scalar1D_to_picture (scalar_1D* field, int size, direction DIR, scalar_type MODE);
picture* scalar2D_to_picture (scalar_2D* field, scalar_type MODE);
