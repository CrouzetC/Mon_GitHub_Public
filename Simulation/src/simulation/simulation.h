
#pragma once

#include "base/complex.h"
#include "base/util.h"

#define NB_LAST_WAVE_FUNC 5

typedef struct _particle {

	double m;  // masse
	double charge;
	complex_1D* last_wave_functions[NB_LAST_WAVE_FUNC];  // *last_wave_functions[0] est la fonction d'onde actuelle
	
} particle;

typedef struct _phys_constants {

	double h_barre;    // constante de Plank r�duite
	double eps_0;      // permittitivit� �lectrique du vide
	double eps_0_4pi;  // = eps_0 * 4 * PI

} phys_constants;

typedef struct _simul_params {  // param�tres propres � une simulation

	int dimension;  // pour savoir si on est en dimension 1, 2 ou 3
	double dx;
	double dy;
	double dz;

	double dt;  // intervalle de temps

} simul_params;

typedef struct _simulation {  // param�tres propres � une simulation

	simul_params*  sim_params;
	phys_constants* constants;

	scalar_1D* field_V;  // champ d'�nergie potentielle

	general_set* particles;

} simulation;


//
//
//  !! Attention : il faudrait peut-�tre D'ABORD faire une simulation de diffusion,
//
//			pour v�rifier que le 'Laplacian' donne le bon r�sultat (pas de "/4")
//	==> ou juste calculer analytiquement la valeur attendue du Laplacian et comparer !
//


particle*     create_particle  (double m, double charge, double dx, int length);
phys_constants*  create_phys_constants ();
simul_params*    create_simul_params   (int dimension, double dxyz, double dt);
simulation*      create_simulation     (int dimension, double dxyz, double dt, int length);

void  destroy_particle        (particle* p);
void  destroy_phys_constants  (phys_constants* constants);
void  destroy_simul_params    (simul_params* params);
void  destroy_simulation      (simulation* simul);

void  add_particle (simulation* simul, particle* p);

void  simulation_next_step (simulation* simul);
void  particle_next_step   (simulation* simul, particle* p);
complex_1D* wave_func_derivative(simulation* simul, particle* p);

