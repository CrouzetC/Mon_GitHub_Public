

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "picture.h"


  // pour fopen(_s) et fscanf(_s) :

  /*
      Explications : il semble que 'fopen_s' et 'fscanf_s' définit dans la norme C11
                     ne soient pas implémentés sur certaines (voire toutes) les distributions de Linux.
  */

#ifdef _WIN32  // Windows 32 bits et Windows 64 bits
    #define FSCANF fscanf_s
#else
    #define FSCANF fscanf
#endif


void save_picture(char* path, picture* pic, int number_at_end) {

	FILE* file = new_file(path, number_at_end);

	if (!file)
		return;

	fprintf(file, "height: %d, width: %d\n", pic->height, pic->width);
	fprintf(file, "format: RGBA\n");

	int indice = 0;
	for (int y = 0; y < pic->height; y++) {
		for (int x = 0; x < pic->width; x++) {

			fprintf(file, "%f,%f,%f,%f,", *(pic->pixels + indice), *(pic->pixels + indice + 1), *(pic->pixels + indice + 2), *(pic->pixels + indice + 3));
			indice += 4;
		}
		fprintf(file, "\n");
	}
	fclose(file);

	return;
}

picture* read_picture(char* file_path, int pos_x, int pos_y) {

	FILE* file;

#ifdef _WIN32
	fopen_s(&file, file_path, "r");
#else
    file = fopen(file_path, "r");
#endif

	if (!file) {
		fprintf(stderr, "Attention : echec de l'ouverture du fichier dans 'read_picture(...)'\n");
		return NULL;
	}

	int width, height;

	FSCANF(file, "height: %d", &height);
	FSCANF(file, ", width: %d\n", &width);
	FSCANF(file, "format: RGBA\n");

	picture* pic = create_picture(pos_x, pos_y, width, height);

	int indice = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			FSCANF(file, "%f,%f,%f,%f,", pic->pixels + indice, pic->pixels + indice + 1, pic->pixels + indice + 2, pic->pixels + indice + 3);

			indice += 4;
		}
		FSCANF(file, "\n");
	}
	fclose(file);

	return pic;
}


picture* create_picture(int pos_x, int pos_y, int width, int height) {

	picture* pic = (picture*) malloc (sizeof(picture));

	pic->pixels = (float*) malloc(4*height*width*sizeof(float));
	pic->width = width;
	pic->height = height;
	pic->pos_x = pos_x;
	pic->pos_y = pos_y;

	// On initialise l'image comme blanche transparente (donc grise en apparence) :
	for (int i = 0; i < 4 * height * width; i+= 4) {
		pic->pixels[ i ] = 1.0;
		pic->pixels[i+1] = 1.0;
		pic->pixels[i+2] = 1.0;
		pic->pixels[i+3] = 0.8;
	}

	return pic;
}

void destroy_picture(picture* pic) {

	free(pic->pixels);
	free(pic);

	return;
}

void shift_picture(picture* pic, int dx, int dy) {

	pic->pos_x += dx;
	pic->pos_y += dy;

	return;
}

void pic_to_picture(picture* screen, picture* pic, alpha_blending_mode MODE) {

	float* pic_pix = pic->pixels;

	int indice_pic    = 0;
	int indice_pixels = 0;  // pour les pixels qui seront affichés sur l'écran

	// On repère le cadre de l'écran correspondant à l'image :
	int start_x = pic->pos_x >= 0  ?  pic->pos_x  :  0 ;
	int start_y = pic->pos_y >= 0  ?  pic->pos_y  :  0 ;
	int end_x = pic->pos_x + pic->width  - 1 < screen->width  ? pic->pos_x + pic->width  - 1 : screen->width  - 1 ;
	int end_y = pic->pos_y + pic->height - 1 < screen->height ? pic->pos_y + pic->height - 1 : screen->height - 1;

	for (int y = start_y; y <= end_y; y += 1) {

		indice_pixels = 4 * (start_x + y * screen->width);

		for (int x = start_x; x <= end_x; x++) {

			set_pixel ( screen->pixels + indice_pixels,
				        *(pic_pix+indice_pic),   *(pic_pix+indice_pic+1),
				        *(pic_pix+indice_pic+2), *(pic_pix+indice_pic+3), MODE );

			indice_pixels += 4;
			indice_pic += 4;
		}
	}

	return;
}


picture* create_background (int width, int height, int param) {

	picture* background = create_picture(0, 0, width, height);

	float* pix = background->pixels;

	float half_width  = ((float) width  - 1) / 2;
	float half_height = ((float) height - 1) / 2;
	float half_diagonal = (float)sqrt( (float)( half_width*half_width + half_height*half_height) );

	float x2, y2;
	int indice = 0.0;

	switch (param) {

	case 0 :  //  Fond rouge sur les bords et noir au centre

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				x2 = x - half_width;
				y2 = y - half_height;

				*(pix + indice    ) = 1.0;
				*(pix + indice + 1) = 0.0;
				*(pix + indice + 2) = 0.0;
				*(pix + indice + 3) = sqrt( x2*x2 + y2*y2 ) / half_diagonal;

				indice += 4;
			}
		}
		break;

	case 1 :  //  Fond blanc sur les bords et cyan au centre

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				x2 = x - half_width;
				y2 = y - half_height;
				
				*(pix + indice    ) = sqrt( x2*x2 + y2*y2 ) / half_diagonal;
				*(pix + indice + 1) = 1.0;
				*(pix + indice + 2) = 1.0;
				*(pix + indice + 3) = 1.0;

				indice += 4;
			}
		}
		break;

	case 2 :  //  Fond uniforme gris foncé

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				*(pix + indice    ) = 0.4;
				*(pix + indice + 1) = 0.4;
				*(pix + indice + 2) = 0.4;
				*(pix + indice + 3) = 1.0;

				indice += 4;
			}
		}
		break;

	case 3 :  //  Fond uniforme gris clair

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				*(pix + indice    ) = 0.8;  // Avec 0.7 c'est bien aussi
				*(pix + indice + 1) = 0.8;
				*(pix + indice + 2) = 0.8;
				*(pix + indice + 3) = 1.0;

				indice += 4;
			}
		}
		break;

	default :
		fprintf(stderr, "Attention : create_background (...) ne traite pas le cas ou 'param' vaut : %d.\n", param);
		break;
	}

	return background;
}


picture* scalar1D_to_picture (scalar_1D* field, int size, direction DIR, scalar_type MODE) {
	
	picture* pic;
	int position;
	
	switch (DIR) {

	case RIGHT :
		
		pic = create_picture(100, 100, field->length, size);
		for (int j = 0; j < size; j++) {
			position = 4 * j * field->length;
			for (int i = 0; i < field->length; i++) {
				float_to_pixel(pic->pixels + position + 4*i, field->data[i], MODE);
			}
		}
		break;

	case LEFT:

		pic = create_picture(100, 100, field->length, size);
		for (int j = 0; j < size; j++) {
			position = 4 * j * field->length;
			for (int i = 0; i < field->length; i++) {
				float_to_pixel(pic->pixels + position + 4*i, field->data[field->length-1-i], MODE);
			}
		}
		break;

	case TOP:

		pic = create_picture(100, 100, size, field->length);
		for (int i = 0; i < field->length; i++) {

			position = size * 4 * i;
			float_to_pixel(pic->pixels + position, field->data[i], MODE);

			for (int j = 1; j < size; j++) {
				pic->pixels[position+4*j  ] = pic->pixels[position];
				pic->pixels[position+4*j+1] = pic->pixels[position+1];
				pic->pixels[position+4*j+2] = pic->pixels[position+2];
				pic->pixels[position+4*j+3] = pic->pixels[position+3];
			}
		}
		break;
		
	case BOTTOM:

		pic = create_picture(100, 100, size, field->length);
		for (int i = 0; i < field->length; i++) {

			position = size * 4 * i;
			float_to_pixel(pic->pixels + position, field->data[field->length-1-i], MODE);

			for (int j = 1; j < size; j++) {
				pic->pixels[position + 4 * j] = pic->pixels[position];
				pic->pixels[position + 4 * j + 1] = pic->pixels[position + 1];
				pic->pixels[position + 4 * j + 2] = pic->pixels[position + 2];
				pic->pixels[position + 4 * j + 3] = pic->pixels[position + 3];
			}
		}
		break;

	default :
		pic = NULL;
		// Sans cette ligne, on a une erreur : 'Arret de CL.exe avec le code 2'
		// En effet, on pourrait renvoyer une valeur qui n'a pas été initialisée (pour la variable 'pic')
		break;
	}
	
	return pic;
}

picture* scalar2D_to_picture(scalar_2D* field, scalar_type MODE) {

	picture* pic = create_picture(100, 100, field->width, field->height);

	int position;

	for (int i = 0; i < field->height; i++) {
		position = 4 * i * field->width;

		for (int j = 0; j < field->width; j++) {
			float_to_pixel(pic->pixels + position + 4*j, field->data[ i*field->width + j ], MODE);
		}
	}

	return pic;
}


