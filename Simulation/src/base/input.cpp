
#include "input.h"


input_grid_for_scalar2D* create_scalar2D_input_grid (scalar_2D* field, int x, int y, int square_size) {

	input_grid_for_scalar2D* grid = (input_grid_for_scalar2D*) malloc (sizeof(input_grid_for_scalar2D));
	grid->field = field;
	grid->square_size = square_size;
	grid->x = x;
	grid->y = y;

	return grid;
}

void destroy_scalar2D_input_grid (input_grid_for_scalar2D* grid) {
	free(grid);
	return;
}


void check_for_input(input_grid_for_scalar2D* grid, int x_mouse, int y_mouse) {

	//
	float epsilon = 0.00001;

	if (   x_mouse < grid->x || x_mouse >= grid->x + grid->field->width  * grid->square_size
		|| y_mouse < grid->y || y_mouse >= grid->y + grid->field->height * grid->square_size) {

		return;
	}

	int x = (x_mouse - grid->x) / grid->square_size;
	int y = (y_mouse - grid->y) / grid->square_size;

	scalar_2D* field = grid->field;
	float scalar = field->data[x + y * field->width];

	if (scalar < 0.5)
		field->data[x + y * field->width] = 1.0;
	else
		field->data[x + y * field->width] = 0.0;

	return;
}


void add_grid_to_graphic_set(graphic_set* set, input_grid_for_scalar2D* grid) {

	//
	float epsilon = 0.00001;
	float grey_0 = 0.6;  // intensité du gris en bas à gauche

	// [RGB du pixel en haut à droite] = [RGB du pixel en bas à gauche] + pixel_grey_total_shift :
	// (différence d'intensité entre les gris des 2 extrémités)
	float pixel_grey_total_shift = 0.3;


	pixel* pix = create_pixel (0.6, 0.6, 0.6, 1.0);
	scalar_2D* field = grid->field;
	int size = grid->square_size;
	float scalar = 0.0;
	float grey = 0.0;

	for (int y = 0; y < field->height; y++) {

		for (int x = 0; x < field->width; x++) {

			scalar = field->data[x + y * field->width];

			if (scalar >= 0.5) {
				pix->r = 0.0;
				pix->g = 0.0;
				pix->b = 0.0;
			} else {
				grey = grey_0 + ( (float) x + y ) / (field->width + field->height - 2) * pixel_grey_total_shift;
				pix->r = grey;
				pix->g = grey;
				pix->b = grey;
			}

			add_graphic_elem (set, create_square(grid->x + x*size, grid->y + y*size, size, size, pix), SQUARE);

		}
	}

	return;
}

