
#include <stdlib.h>

#include "graphic.h"
#include "graphic_elem.h"
#include "picture.h"
#include "complex.h"  // pour 'complex_1D_graph'

/***
	create()
***/

graphic_set* create_set() {

	graphic_set* set = (graphic_set*) malloc (sizeof(graphic_set));
	
	set->nb = 0;
	set->elements = NULL;
	set->elements_types = NULL;

	return set;
}

pixel* create_pixel (float r, float g, float b, float a) {

	pixel* pix = (pixel*) malloc (sizeof(pixel));
	pix->r = r;
	pix->g = g;
	pix->b = b;
	pix->a = a;
	return pix;
}

/***
	destroy()
***/

void destroy_set (graphic_set* set) {

	for (int i = 0; i < set->nb; i++) {

		switch (set->elements_types[i]) {

		case GRAPHIC_SET:
			destroy_set( (graphic_set*) (set->elements[i]) );
			break;

		case PICTURE:
			destroy_picture ( (picture*) (set->elements[i]) );
			break;

		case COMPLEX_1D_GRAPH:
			destroy_complex_1D_graph ( (complex_1D_graph*) (set->elements[i]) );
			break;

		case LINE:
			//
			//
			//
			break;

		case VERTEX_SET :
			destroy_vertex_set ( (vertex_set*) (set->elements[i]) );
			break;

		case SQUARE:
			destroy_square ( (square*) (set->elements[i]) );
			break;

		case EMPTY_SQUARE:
			destroy_empty_square  ( (empty_square*) (set->elements[i]) );
			break;
		}
	}

	free(set->elements_types);
	free(set->elements);
	free(set);

	return;
}

void  destroy_pixel (pixel* pix) {
	free(pix);
}

/***
	other
***/

void shift_graphic_set (graphic_set* set, int dx, int dy) {

	for (int i = 0; i < set->nb; i++) {

		switch (set->elements_types[i]) {

		case GRAPHIC_SET:
			shift_graphic_set((graphic_set*)(set->elements[i]), dx, dy);
			break;

		case PICTURE:
			shift_picture((picture*)(set->elements[i]), dx, dy);
			break;

		case COMPLEX_1D_GRAPH:
			// Rien
			break;

		case LINE:
			shift_line ((line*)(set->elements[i]), dx, dy);
			break;

		case VERTEX_SET:
			shift_vertex_set((vertex_set*)(set->elements[i]), dx, dy);
			break;

		case SQUARE:
			shift_square((square*)(set->elements[i]), dx, dy);
			break;

		case EMPTY_SQUARE:
			shift_empty_square((empty_square*)(set->elements[i]), dx, dy);
			break;
		}
	}

	return;
}

void add_graphic_elem (graphic_set* set, void* elem, graphic_type TYPE) {

    if (set->nb % GRAPHIC_SET_BLOC_SIZE == 0)
    {
        set->elements       = (void**)        realloc (set->elements      , (set->nb + GRAPHIC_SET_BLOC_SIZE) * sizeof(void*));
        set->elements_types = (graphic_type*) realloc (set->elements_types, (set->nb + GRAPHIC_SET_BLOC_SIZE) * sizeof(graphic_type));
    }
	set->elements      [set->nb] = elem;
	set->elements_types[set->nb] = TYPE;
	set->nb++;

	return;
}

void remove_graphic_elem (graphic_set* set, void* elem) {

	int position = 0;

	while ( set->elements[position] != elem && position < set->nb ) {
		position += 1;
	}

	if (position == set->nb) {
		fprintf(stderr, "Attention : l'element d'adresse %p n'a pas ete trouve dans le 'set' en argument de 'remove_graphic_elem'\n", elem);
		return;
	}

	for (int i = position; i < set->nb - 1; i++) {
		set->elements[i] = set->elements[i+1];
		set->elements_types[i] = set->elements_types[i+1];
	}

    if (set->nb == 1)
    {
        free(set->elements);
        free(set->elements_types);
	    set->elements       = NULL;
	    set->elements_types = NULL;
    }
    else if (set->nb % GRAPHIC_SET_BLOC_SIZE == 1 && set->nb != 0)
    {
	    set->elements       = (void**)        realloc (set->elements      , (set->nb - GRAPHIC_SET_BLOC_SIZE) * sizeof(void*));
	    set->elements_types = (graphic_type*) realloc (set->elements_types, (set->nb - GRAPHIC_SET_BLOC_SIZE) * sizeof(graphic_type));
    }
	set->nb -= 1;

	return;
}


void set_to_picture (picture* pic, graphic_set* set, alpha_blending_mode MODE) {

	if (!pic) {
		fprintf(stderr, "%s\n", "Erreur : set_to_picture(...) appliquee avec pic == NULL");
		return;
	}

	for (int i = 0; i < set->nb; i++) {

		switch (set->elements_types[i]) {

		case GRAPHIC_SET:
			set_to_picture(pic, (graphic_set*) set->elements[i], MODE);
			break;

		case PICTURE :
			pic_to_picture (pic, (picture*) set->elements[i], MODE);
			break;

		case COMPLEX_1D_GRAPH:
			complex_1D_graph_to_picture(pic, (complex_1D_graph*) set->elements[i]);
			break;

		case LINE:
			//
			//
			//
			break;

		case VERTEX_SET :
			vertex_set_to_picture (pic, (vertex_set*)set->elements[i], MODE);
			break;

		case SQUARE:
			square_to_picture (pic, (square*) set->elements[i], MODE);
			break;

		case EMPTY_SQUARE:
			empty_square_to_picture (pic, (empty_square*) set->elements[i], MODE);
			break;
		}

	}

	return;
}


void float_to_pixel(float* pixel, float scalar, scalar_type MODE) {

	if (scalar <= 0.0)
		scalar = 0.0;
	if (scalar >= 1.0)
		scalar = 1.0;
	
	switch (MODE) {

	case RGB :
		
		////
		////// ICI : il faudrait peut etre faire des divisions de taille egales :
		//////       de 0 a 1/6, 2/6... plutot que 1, 1/4, 3/8...
		////

		if (scalar <= 0.25) {
			pixel[0] = 0.0;
			pixel[1] = 0.0;
			pixel[2] = 4*scalar;
			pixel[3] = 1.0;
		}
		else if (scalar > 0.25 && scalar <= 0.375) {
			pixel[0] = 0.0;
			pixel[1] = 8 * (scalar - 0.25);
			pixel[2] = 1.0;
			pixel[3] = 1.0;
		}
		else if (scalar > 0.375 && scalar <= 0.5) {
			pixel[0] = 0.0;
			pixel[1] = 1.0;
			pixel[2] = 1.0 - 8 * (scalar - 0.375);
			pixel[3] = 1.0;
		}
		else if (scalar > 0.5 && scalar <= 0.625) {
			pixel[0] = 8 * (scalar-0.5);
			pixel[1] = 1.0;
			pixel[2] = 0.0;
			pixel[3] = 1.0;
		}
		else if (scalar > 0.625 && scalar <= 0.75) {
			pixel[0] = 1.0;
			pixel[1] = 1.0 - 8 * (scalar - 0.625);
			pixel[2] = 0.0;
			pixel[3] = 1.0;
		}
		else {
			pixel[0] = 1.0;
			pixel[1] = 4*(scalar-0.75);
			pixel[2] = 4*(scalar-0.75);
			pixel[3] = 1.0;
		}
		break;
		
	case BLACK:
		pixel[0] = 0.0;
		pixel[1] = 0.0;
		pixel[2] = 0.0;
		pixel[3] = scalar;
		break;

	case WHITE:
		pixel[0] = 1.0;
		pixel[1] = 1.0;
		pixel[2] = 1.0;
		pixel[3] = scalar;
		break;
	}
	
	return;
}

void set_pixel (float* pixel, float r, float g, float b, float a, alpha_blending_mode MODE) {

	float new_alpha, new_color;

	switch (MODE) {

	case REPLACE:
		pixel[0] = r;
		pixel[1] = g;
		pixel[2] = b;
		pixel[3] = a;
		break;

	case OVER:

		new_alpha = a + pixel[3] *(1.0 - a);

		new_color = ( r * a + pixel[0] * pixel[3] * (1.0 - a) ) / new_alpha;
		pixel[0]  = new_color;

		new_color = ( g * a + pixel[1] * pixel[3] * (1.0 - a) ) / new_alpha;
		pixel[1] = new_color;

		new_color = ( b * a + pixel[2] * pixel[3] * (1.0 - a) ) / new_alpha;
		pixel[2] = new_color;

		pixel[3] = new_alpha;

		break;

	default :
		fprintf(stderr, "%s\n", "Avertissement : MODE de 'set_pixel(...)' non pris en charge, operation ignoree.");
		break;
	}

	return;
}

void set_pixel(pixel* pix, float r, float g, float b, float a) {
	pix->r = r;
	pix->g = g;
	pix->b = b;
	pix->a = a;
}

void set_pixel (pixel* destination_pix, pixel* source_pix) {
	destination_pix->r = source_pix->r;
	destination_pix->g = source_pix->g;
	destination_pix->b = source_pix->b;
	destination_pix->a = source_pix->a;
}
