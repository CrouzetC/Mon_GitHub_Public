
#pragma once

#include "graphic.h"
#include "graphic_elem.h"


typedef struct _input_grid_for_scalar2D {

	scalar_2D* field;
	int square_size;
	int x;
	int y;

} input_grid_for_scalar2D;


input_grid_for_scalar2D*  create_scalar2D_input_grid (scalar_2D* field, int x, int y, int square_size);


void  destroy_scalar2D_input_grid  (input_grid_for_scalar2D* grid);


/*
    Modifie le champ 'field' associé à la 'grid' selon la position du clic de la souris (x_mouse; y_mouse)
*/
void check_for_input (input_grid_for_scalar2D* grid, int x_mouse, int y_mouse);


void add_grid_to_graphic_set (graphic_set* set, input_grid_for_scalar2D* grid);


