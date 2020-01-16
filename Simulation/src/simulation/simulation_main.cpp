
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
												 0, MODULE_WITH_ARG);
		
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
														1, MODULE_WITH_ARG);

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


        // Filtrages et FFT :

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

	if (state == GLUT_DOWN)
    {
		buttonState |= 1 << button;
    }
    else if (state == GLUT_UP)
    {
		buttonState = 0;
	}
	
	x_mouse = x;
	y_mouse = window_height - 1 - y;

	return;
}

void motion (int x, int y) {

	glutPostRedisplay();

	int dx, dy;
	dx = x - x_mouse;
	dy = (window_height - 1 - y) - y_mouse;

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
