
#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include "GL/freeglut.h"
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "base/graphic.h"
#include "base/graphic_elem.h"
#include "base/complex.h"
#include "base/input.h"

#include "simulation.h"


// Fonctions :

void myGlutInit ();
void display ();


// Variables globales :

int window_width  = 1280;  // 1920;  // 1100;
int window_height = 720;   // 1080;  // 800;

picture* screen = NULL;  // 'screen->pixels' est un tableau de 4*width*height cases, représente l'image totale de la fenêtre en [Rouge, Vert, Bleu, Alpha]

picture* background = NULL;

graphic_set* set = NULL;
graphic_set* alpha_blend_set = NULL;  // graphic_set pour les textes (pour 'alpha_blending_mode', utilise OVER plutôt que REPLACE)

scalar_2D* field_2D = NULL;

////  Complex 1D et simulation  ////

particle* particle1 = NULL;
int is_created_simulation = 0;
complex_1D_graph* complex_graph1 = NULL;
complex_1D_graph* complex_graph2 = NULL;
complex_1D_graph* complex_graph3 = NULL;
simulation* simul = NULL;
complex_1D* delta = NULL;  // = derivative * dt
complex_1D* fourier_transform = NULL;

////  Grid  ////

scalar_2D* field_for_input = NULL;
input_grid_for_scalar2D* input_grid = NULL;
graphic_set* input_grid_graphic_set = NULL;
int is_created_grid = 0;


          ////////////////////////////////////////////////


void refresh_simulation_display() {

	if (complex_graph1) {
		remove_graphic_elem(set, complex_graph1);
		destroy_complex_1D_graph(complex_graph1);

		complex_graph1 = create_complex_1D_graph(particle1->last_wave_functions[0],
												 150, window_height/2 + 50, 100,
												 0, MODULE_WITH_ARG);
			
		add_graphic_elem(set, complex_graph1, COMPLEX_1D_GRAPH);
	}

	if (complex_graph2) {
		remove_graphic_elem(set, complex_graph2);
		destroy_complex_1D_graph(complex_graph2);

		complex_graph2 = create_complex_1D_graph( delta,  // au lieu de : particle1->last_wave_functions[0],
												 150, window_height/2 - 150, 100,
												 0, REAL);
		
		add_graphic_elem(set, complex_graph2, COMPLEX_1D_GRAPH);
	}

	if (complex_graph3) {
		remove_graphic_elem(set, complex_graph3);
		destroy_complex_1D_graph(complex_graph3);

		complex_graph3 = create_complex_1D_graph(fourier_transform,
												 150, window_height/2 - 350, 100,
												 0, MODULE_WITH_ARG);

		add_graphic_elem(set, complex_graph3, COMPLEX_1D_GRAPH);
	}

	return;
}


void keyboard (unsigned char key, int, int) {  // <=> void keybord (unsigned char key, int x, int y)
	
	glutPostRedisplay();  // Indique la fenêtre comme étant à rafraichir

	picture* argument_disk;

	switch (key) {

		case 27:
			glutDestroyWindow(glutGetWindow());
			return;

		case 'r' :

			if (is_created_grid == 0) {

				field_for_input = create_scalar2D(16, 8);

				input_grid = create_scalar2D_input_grid(field_for_input, 200, 200, 20);

				input_grid_graphic_set = create_set();

				add_grid_to_graphic_set(input_grid_graphic_set, input_grid);
				add_graphic_elem(set, input_grid_graphic_set, GRAPHIC_SET);

				is_created_grid = 1;
			}
			else {

				// Nom du fichier :
				char* file_path = (char*) malloc (256*sizeof(char));
				const char* path = "D:\\Fichiers\\Pictures\\alphabet_flat\\";
				int pos = 0;
				while (path[pos] != '\0' && pos <= 250) {
					file_path[pos] = path[pos];
					pos ++;
				}
				file_path[255] = '\0';
				
				fprintf(stdout, "Saisir caractere : ");
				char space;

            #ifdef _WIN32
                fscanf_s(stdin, "%c", file_path + pos);
                fscanf_s(stdin, "%c", &space);
            #else
				fscanf(stdin, "%c", file_path + pos);
				fscanf(stdin, "%c", &space);
            #endif

				// file_path[pos]   = ' ';
				file_path[pos+1] = '.';
				file_path[pos+2] = 'p';
				file_path[pos+3] = 'i';
				file_path[pos+4] = 'c';
				file_path[pos+5] = '\0';

				// création et sauvegarde de l'image :
				
				picture* pic = scalar2D_to_picture(field_for_input, BLACK);

				save_picture(file_path, pic, 0);

				// Remise à zéro de 'field' :
				for (int i = 0; i < field_for_input->width * field_for_input->height; i++) {
					field_for_input->data[i] = 0.0;
				}

				// free :
				destroy_picture(pic);
				free(file_path);
			}

			break;

		case 't' : 
		
				{  // permet de ne pas avoir de problème avec le fait que l'on déclare des variables dans un "case 't' : ... break;"

				fprintf(stderr, "ATTENTION : sur Linux, il faut créer le fichier ' .pic' (caractere ESPACE), en effet un fichier nommé ainsi ne peut pas être copié sur Windows.");

				//
				char chars_to_display[] = "abcdefghijklmnopqrstuvwxyz ceci est un message, veuillez ne pas y preter attention.";

				// on copie le principal de l'adresse :
				char* file_path = (char*) malloc (256 * sizeof(char));
				const char* path = "D:\\Fichiers\\Pictures\\alphabet_flat\\z.pic";
				int pos = 0;
				while (path[pos] != '\0' && pos <= 250) {
					file_path[pos] = path[pos];
					pos++;
				}
				file_path[pos] = '\0';

				// on repère la position du caractère à changer :
				pos = 0;
				while (file_path[pos] != 'z') {
					pos++;
				}

				// on affiche les divers caractères voulus :

				for (int i = 0; i < 83; i++) {

					file_path[pos] = chars_to_display[i];

					picture* pic = read_picture(file_path, 100 + i * 8, 100);

					add_graphic_elem(alpha_blend_set, pic, PICTURE);

				}
			
			} break;

		case 'd':
			if (is_created_simulation) {
				destroy_complex_1D(delta);
				delta = wave_func_derivative(simul, particle1);
				filter_order_1_rigorous(&delta, 0.005);
				for (int i = 0; i < particle1->last_wave_functions[0]->length; i++) {
					delta->re[i] *= 10 * simul->sim_params->dt;
					delta->im[i] *= 10 * simul->sim_params->dt;
				}
				simulation_next_step(simul);
				fft(particle1->last_wave_functions[0], fourier_transform, 0);
				refresh_simulation_display();
			}
			break;

		case 'w':
			if (is_created_simulation) {
                filter_attenuating_high_freq ( particle1->last_wave_functions[0], fourier_transform);
				refresh_simulation_display();
			}
			break;

		case 'x':
			if (is_created_simulation) {
                filter_cutting_high_freq(particle1->last_wave_functions[0], fourier_transform, 0.4);
				refresh_simulation_display();
			}
			break;

		case 'c':
			if (is_created_simulation) {
                filter_order_1( &(particle1->last_wave_functions[0]), 0.015 );
				fft(particle1->last_wave_functions[0], fourier_transform, 0);
				refresh_simulation_display();
			}
			break;

		case 'v':
			if (is_created_simulation) {
                filter_order_1_rigorous( &(particle1->last_wave_functions[0]), 0.01 );
				fft(particle1->last_wave_functions[0], fourier_transform, 0);
				refresh_simulation_display();
			}
			break;

		case 'b':
			if (is_created_simulation) {
                filter_2( &(particle1->last_wave_functions[0]), 0.5 );
				fft(particle1->last_wave_functions[0], fourier_transform, 0);
				refresh_simulation_display();
			}
			break;

		case 'f':  // FFT :
			if (is_created_simulation) {
				fft(particle1->last_wave_functions[0], fourier_transform, 0);
				refresh_simulation_display();
			}
			break;

		case 'g':  // IFFT :
			if (is_created_simulation) {
				fft(fourier_transform, particle1->last_wave_functions[0], 1);
				refresh_simulation_display();
			}
			break;

		case 's':
			
			if (!is_created_simulation) {

				argument_disk = create_argument_reference(window_width - 151, window_height - 151, 101);
				add_graphic_elem(alpha_blend_set, argument_disk, PICTURE);

				simul = create_simulation(1, 0.01, 0.001, 512);

				particle1 = create_particle(1.0, -1.0, simul->sim_params->dx, simul->field_V->length);
				add_particle(simul, particle1);

				for (int i = 0; i < NB_LAST_WAVE_FUNC; i++) {
					make_centered_gaussian(particle1->last_wave_functions[i], 0.1);
					normalize_module_square(particle1->last_wave_functions[i]);
				}

				delta = wave_func_derivative(simul, particle1);

				for (int i = 0; i < particle1->last_wave_functions[0]->length; i++) {
					delta->re[i] *= 10*simul->sim_params->dt;
					delta->im[i] *= 10 * simul->sim_params->dt;
				}

				fourier_transform = create_complex_1D(simul->field_V->length, 1.0 / simul->sim_params->dx);
				fft(particle1->last_wave_functions[0], fourier_transform, 0);

                //for (int i = 0; i < 512; i++) {
                //    fourier_transform->re[i] = 0.1;
                //    fourier_transform->im[i] = 0.0;
                //}
                //fft(fourier_transform, particle1->wave_function, 1);

				complex_graph1 = create_complex_1D_graph(particle1->last_wave_functions[0],
														150, window_height/2 + 50, 100,
														1, REAL);

				complex_graph2 = create_complex_1D_graph( delta,  // au lieu de : particle1->last_wave_functions[0],
														150, window_height/2 - 150, 100,
														0, MODULE_WITH_ARG);

				complex_graph3 = create_complex_1D_graph(fourier_transform,
														150, window_height/2 - 350, 100,
														0, MODULE_WITH_ARG);

				add_graphic_elem(set, complex_graph1, COMPLEX_1D_GRAPH);
				add_graphic_elem(set, complex_graph2, COMPLEX_1D_GRAPH);
				add_graphic_elem(set, complex_graph3, COMPLEX_1D_GRAPH);

				is_created_simulation = 1;
			}
			break;

		default:
			break;
	}
    
	return;
}


int x_mouse;  // pour (x_mouse, y_mouse) : (0;0) est en BAS à gauche
int y_mouse;
int buttonState = 0;

void mouse(int button, int state, int x, int y) {    // Attention : (0;0) en HAUT à gauche

	glutPostRedisplay();  // Indique la fenêtre comme étant à rafraichir

	if (state == GLUT_DOWN) {

		buttonState |= 1 << button;
	}
	else if (state == GLUT_UP) {

		buttonState = 0;
	}
	
	x_mouse = x;
	y_mouse = window_height - 1 - y;

	// input_grid :
	if (is_created_grid && button == 0 && state == GLUT_DOWN) {

		remove_graphic_elem(set, input_grid_graphic_set);
		destroy_set(input_grid_graphic_set);
		input_grid_graphic_set = create_set();

		check_for_input(input_grid, x_mouse, y_mouse);

		add_grid_to_graphic_set(input_grid_graphic_set, input_grid);
		add_graphic_elem(set, input_grid_graphic_set, GRAPHIC_SET);
	}

	return;
}

void motion (int x, int y) {

	glutPostRedisplay();

	int dx, dy;
	dx = x - x_mouse;
	dy = (window_height - 1 - y) - y_mouse;

	/* affichage network :
	if (is_created_net) {

		x_net += dx;
		y_net += dy;

		set_node_x_y (net, x_net, y_net);

		shift_graphic_set(network_graphic_set, dx, dy);
		shift_graphic_set(graphic_set_for_selection, dx, dy);
	}*/

	x_mouse = x;
	y_mouse = window_height - 1 - y;
}

void display(void) {

		//// Du coup, c'est un affichage point par point par le CPU, qui serait donc a priori lent

			// Sinon, il y a les textures et Frame Buffers (voir site Open Classroom)
			// Voir  "cudaBindTextureToArray( ... )" pour la création rapide de texture avec CUDA (utilisé dans "BicubicTexture" de CUDA Samples)

			// Aussi : voir comment est fait l'affichage dans la simulation de fluide (CUDA Samples) (utilise le GPU a priori) :   glBufferData(..., particles)   cudaGraphicsGLRegisterBuffer(...)
	
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);

	set_to_picture (screen, set, REPLACE);
	set_to_picture (screen, alpha_blend_set, OVER);

	float* pixels = screen->pixels;
	float r, g, b, alpha;
	int indice;

	for (int x=0; x < window_width; x++) {
		for (int y=0; y < window_height; y++) {

			indice = 4*( x + y*window_width );
			alpha = *(pixels + indice + 3);

			r = *(pixels + indice)     * alpha;
			g = *(pixels + indice + 1) * alpha;
			b = *(pixels + indice + 2) * alpha;
			glColor3f(r, g, b);
			glVertex2f(x, y);
		}
	}

	glEnd();
	glFlush();
}


int main(int argc, char **argv) {


	glutInit(&argc, argv);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Fenêtre 1");
	myGlutInit();

	srand(123456);
	// srand(time(NULL));

	screen = create_picture(0, 0, window_width, window_height);

	set = create_set();
	alpha_blend_set = create_set();

	background = create_background (window_width, window_height, 3);
	add_graphic_elem(set, background, PICTURE);


	
	//add_graphic_elem(set, create_square(700, 40, 100, 100, 1.0, 1.0, 0.0, 1.0),        SQUARE);
	//add_graphic_elem(set, create_empty_square(700, 200, 100, 100, 1.0, 1.0, 0.0, 1.0), EMPTY_SQUARE);
	//add_graphic_elem(set, create_picture(100, 50, 200, 100), PICTURE);
	//add_graphic_elem(set, create_curved_link(50, 500, 500, 200, NULL, NULL), VERTEX_SET);
	//add_graphic_elem(set, create_square(100, 100, 100, 100, 1.0, 1.0, 1.0, 1.0), SQUARE);
	//add_graphic_elem(set, create_curved_link(100, 100, 199, 199, NULL, NULL), VERTEX_SET);
	

	// save_picture( (char*) "F:\\Fichiers\\Pictures\\test.pic", create_picture(0, 0, 10, 4), 0);

	/*
	FILE* f = fopen("D:\\a", "w");
	int a = 512355;
	fwrite(&a, sizeof(int), 1, f);
	fclose(f);
	int r;
	f = fopen("D:\\a", "r");
	fread(&r, sizeof(int), 1, f);
	fclose(f);
	printf("%d\n", r);
	*/

	glutMainLoop();

	return 0;
}


////////////////////////////////////////////////

void cleanup() {

	destroy_picture  (screen);

	remove_graphic_elem  (set, background);
	destroy_picture  (background);

	if (is_created_simulation)
		destroy_simulation(simul);
	if (complex_graph1)
		destroy_complex_1D_graph(complex_graph1);
	if (complex_graph2)
		destroy_complex_1D_graph(complex_graph2);
}

void myGlutInit() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, window_width, 0.0, window_height);
	glMatrixMode(GL_MODELVIEW);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	// glutIdleFunc(myidle);  // appelée dès qu'il n'y a pas d'autres actions à effectuer
	glutCloseFunc(cleanup);
}
