

#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "graphic_elem.h"


/***
	Basique (create, destroy)
***/

square* create_square (int x_bottom_left, int y_bottom_left, int size_x, int size_y, pixel* pix) {

	square* sq = (square*) malloc (sizeof(square));

	sq->x_bottom_left = x_bottom_left;
	sq->y_bottom_left = y_bottom_left;
	sq->x_top_right   = x_bottom_left + size_x - 1;
	sq->y_top_right   = y_bottom_left + size_y - 1;
	sq->r = pix->r;
	sq->g = pix->g;
	sq->b = pix->b;
	sq->a = pix->a;

	return sq;
}

empty_square* create_empty_square (int x_bottom_left, int y_bottom_left, int size_x, int size_y, pixel* pix) {

	empty_square* sq = (empty_square*) malloc (sizeof(empty_square));

	sq->x_bottom_left = x_bottom_left;
	sq->y_bottom_left = y_bottom_left;
	sq->x_top_right = x_bottom_left + size_x - 1;
	sq->y_top_right = y_bottom_left + size_y - 1;
	sq->r = pix->r;
	sq->g = pix->g;
	sq->b = pix->b;
	sq->a = pix->a;

	return sq;
}

vertex_set* create_vertex_set (int nb_vertex) {

	vertex_set* verts = (vertex_set*) malloc (sizeof(vertex_set));
	verts->nb_vertex = nb_vertex;
	verts->x_tab  = (int*)   malloc (nb_vertex * sizeof(int));
	verts->y_tab  = (int*)   malloc (nb_vertex * sizeof(int));
	verts->pixels = (float*) malloc (4 * nb_vertex * sizeof(float));

	return verts;
}

vertex_set* create_curved_link (int x_left, int y_left, int x_right, int y_right, pixel* pix_left, pixel* pix_right) {

	int nb_vertex = x_right - x_left + 1;

	vertex_set* verts = create_vertex_set(nb_vertex);

	int width  = x_right - x_left;
	int height = y_right - y_left;

	if (width <= 0) {
		fprintf(stderr, "Error : curved_link's width is <= 0\n");
		destroy_vertex_set(verts);
		return NULL;
	}

	float coeff;
	int y_previous, y;
	int index;

	for (int x2 = 0; x2 < nb_vertex; x2++) {  // x2 correspond à x - x_left (<=> abscisse en prenant x_left comme origine)

		coeff = x2*1.0 / width;

		y = y_left + round(6 * height * (pow(((double)x2) / width, 2) / 2.0 - pow(((double)x2) / width, 3) / 3.0));

		verts->x_tab[x2] = x_left + x2;
		verts->y_tab[x2] = y;
		
		verts->pixels[4 * x2] = pix_left->r * (1-coeff) + pix_right->r * coeff;
		verts->pixels[4*x2+1] = pix_left->g * (1-coeff) + pix_right->g * coeff;
		verts->pixels[4*x2+2] = pix_left->b * (1-coeff) + pix_right->b * coeff;
		verts->pixels[4*x2+3] = pix_left->a * (1-coeff) + pix_right->a * coeff;
		
		// s'il y a un écart entre 2 points successifs, on complète :
		if (x2 != 0) {

			while ( (y_previous < y - 1 && y_left < y_right) || (y_previous > y + 1 && y_left > y_right) ) {

				coeff = ( 1.0 * y_previous - y_left ) / height;

				if (coeff < 0)
					coeff *= -1;

				// les points rajoutés ne sont pas insérés mais mis A LA FIN des tableaux :
				verts->nb_vertex += 1;
				
				verts->x_tab = (int*) realloc (verts->x_tab, verts->nb_vertex * sizeof(float));
				verts->y_tab = (int*) realloc (verts->y_tab, verts->nb_vertex * sizeof(int));
				verts->pixels = (float*) realloc (verts->pixels, 4 * verts->nb_vertex * sizeof(int));

				index = verts->nb_vertex - 1;

				verts->x_tab[index] = x_left + x2;
				verts->y_tab[index] = y_left < y_right ? y_previous + 1 : y_previous - 1;

				verts->pixels[4 * index]     = pix_left->r * (1 - coeff) + pix_right->r * coeff;
				verts->pixels[4 * index + 1] = pix_left->g * (1 - coeff) + pix_right->g * coeff;
				verts->pixels[4 * index + 2] = pix_left->b * (1 - coeff) + pix_right->b * coeff;
				verts->pixels[4 * index + 3] = pix_left->a * (1 - coeff) + pix_right->a * coeff;

				y_previous = y_left < y_right ? y_previous + 1 : y_previous - 1;
			}
		}

		y_previous = y;
	}

	return verts;
}


void destroy_square(square* square) {
	free(square);
}

void destroy_empty_square(empty_square* square) {
	free(square);
}

void destroy_vertex_set(vertex_set* verts) {
	free(verts->x_tab);
	free(verts->y_tab);
	free(verts->pixels);
	free(verts);
}

/*
    fonction pour décaler les éléments :
*/

void shift_line (line* line_pt, int dx, int dy) {
	line_pt->x_bottom_left += dx;
	line_pt->x_top_right   += dx;
	line_pt->y_bottom_left += dy;
	line_pt->y_top_right   += dy;
}

void shift_square (square* square_pt, int dx, int dy) {
	square_pt->x_bottom_left += dx;
	square_pt->x_top_right   += dx;
	square_pt->y_bottom_left += dy;
	square_pt->y_top_right   += dy;
}

void shift_empty_square (empty_square* square_pt, int dx, int dy) {
	square_pt->x_bottom_left += dx;
	square_pt->x_top_right   += dx;
	square_pt->y_bottom_left += dy;
	square_pt->y_top_right   += dy;
}

void shift_vertex_set (vertex_set* verts, int dx, int dy) {

	for (int i = 0; i < verts->nb_vertex; i++) {
		verts->x_tab[i] += dx;
		verts->y_tab[i] += dy;
	}
}


/***
	" element_to_picture (...) "
***/

void square_to_picture (picture* pic, square* sq, alpha_blending_mode MODE) {

	int indice_pixels = 0;  // pour les pixels de l'image

	// On repère le cadre de l'écran correspondant à l'image :
	int start_x = sq->x_bottom_left >= 0 ? sq->x_bottom_left : 0;
	int start_y = sq->y_bottom_left >= 0 ? sq->y_bottom_left : 0;
	int end_x = sq->x_top_right < pic->width  ? sq->x_top_right : pic->width - 1;
	int end_y = sq->y_top_right < pic->height ? sq->y_top_right : pic->height - 1;

	for (int y = start_y; y <= end_y; y += 1) {

		indice_pixels = 4 * (start_x + y * pic->width);

		for (int x = start_x; x <= end_x; x++) {

			set_pixel( pic->pixels + indice_pixels, sq->r, sq->g, sq->b, sq->a, MODE );

			indice_pixels += 4;
		}
	}
}

void empty_square_to_picture (picture* pic, empty_square* sq, alpha_blending_mode MODE) {
	
	int bottom_position = 4 * sq->y_bottom_left * pic->width;
	int top_position    = 4 * sq->y_top_right   * pic->width;

	for (int x = sq->x_bottom_left; x <= sq->x_top_right; x++) {

		if (x >= 0 && x < pic->width) {
			// Ligne du bas :
			if ( sq->y_bottom_left >= 0 && sq->y_bottom_left < pic->height ) {
				set_pixel(pic->pixels + bottom_position + 4 * x, sq->r, sq->g, sq->b, sq->a, MODE);
			}
			// Ligne du haut :
			if ( sq->y_top_right >= 0 && sq->y_top_right < pic->height     ) {
				set_pixel(pic->pixels + top_position + 4 * x, sq->r, sq->g, sq->b, sq->a, MODE);
			}
		}
	}

	for (int y = sq->y_bottom_left + 1; y <= sq->y_top_right - 1; y++) {
		/*
		    Il y a '+1' et '-1' car ces cas correspondent aux lignes du heut et du bas déjà traitées,
		    il est nécessaire de ne pas re-traiter ces points car avec MODE = OVER (par exemple) le résultat serait faux
		*/
		if (y >= 0 && y < pic->height) {
			// Ligne de gauche :
			if (sq->x_bottom_left >= 0 && sq->x_bottom_left < pic->width) {
				set_pixel( pic->pixels + 4 * (sq->x_bottom_left + y * pic->width), sq->r, sq->g, sq->b, sq->a, MODE );
			}
			// Ligne de droite :
			if (sq->x_top_right >= 0 && sq->x_top_right < pic->width) {
				set_pixel( pic->pixels + 4 * (sq->x_top_right   + y * pic->width), sq->r, sq->g, sq->b, sq->a, MODE );
			}
		}
	}
	return;
}

void vertex_set_to_picture (picture* pic, vertex_set* verts, alpha_blending_mode MODE) {

	int x, y, position;

	for (int i = 0; i < verts->nb_vertex; i++) {
	
		x = verts->x_tab[i];
		y = verts->y_tab[i];

		if (x >= 0 && x < pic->width && y >= 0 && y < pic->height) {

			position = 4 * (y * pic->width + x);

			for (int delta = 0; delta < 4; delta++) {  // delta = 1, 2, 3 puis 4 ; ce qui correspond aux 4 valeurs R, G, B, A

				pic->pixels[position + delta] = verts->pixels[4*i + delta];

			}
		}

	}

	return;
}


/*
    Utile :
*/

void set_pixel_in_vertex_set (vertex_set* set, int pixel_index, int x, int y, pixel* pix) {

	set->x_tab[pixel_index] = x;
	set->y_tab[pixel_index] = y;

	set->pixels[4*pixel_index    ] = pix->r;
	set->pixels[4*pixel_index + 1] = pix->g;
	set->pixels[4*pixel_index + 2] = pix->b;
	set->pixels[4*pixel_index + 3] = pix->a;

	return;
}


