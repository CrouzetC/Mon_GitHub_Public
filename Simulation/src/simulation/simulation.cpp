

#include "simulation.h"


particle* create_particle (double m, double charge, double dx, int length) {
	particle* p = (particle*) malloc (sizeof(particle));
	
	p->m = m;
	p->charge = charge;
	for (int i = 0; i < NB_LAST_WAVE_FUNC; i++)
		p->last_wave_functions[i] = create_complex_1D (length, dx);

	return p;
}

phys_constants* create_phys_constants() {
	phys_constants* constants = (phys_constants*) malloc (sizeof(phys_constants));

	constants->h_barre = 1.0;
	constants->eps_0   = 1.0;
	constants->eps_0_4pi = 4 * PI * constants->eps_0;

	return constants;
}

simul_params* create_simul_params (int dimension, double dxyz, double dt) {
	simul_params* params = (simul_params*) malloc (sizeof(simul_params));
	params->dimension = dimension;
	params->dx = dxyz;
	params->dy = dimension >= 2 ? dxyz : 0.0;
	params->dz = dimension >= 3 ? dxyz : 0.0;
	params->dt = dt;
	return params;
}

simulation* create_simulation (int dimension, double dxyz, double dt, int length) {
	simulation* simul = (simulation*) malloc (sizeof(simulation));

	simul->sim_params = create_simul_params(dimension, dxyz, dt);
	simul->constants  = create_phys_constants();
	simul->field_V    = create_scalar1D (length);
	simul->particles  = create_general_set();
	return simul;
}


void  destroy_particle (particle* p) {
	for (int i = 0; i < NB_LAST_WAVE_FUNC; i++)
		destroy_complex_1D (p->last_wave_functions[i]);
	free(p);
}

void  destroy_phys_constants (phys_constants* constants) {
	free(constants);
}

void  destroy_simul_params (simul_params* params) {
	free(params);
}

void  destroy_simulation (simulation* simul) {
	destroy_simul_params  (simul->sim_params);
	destroy_phys_constants(simul->constants);
	destroy_scalar1D      (simul->field_V);
	while (simul->particles->nb > 0) {
		destroy_particle((particle*)simul->particles->elems[0]);
		remove_elem(simul->particles, simul->particles->elems[0]);
	}
	destroy_set(simul->particles);
	free(simul);
}


void add_particle (simulation* simul, particle* p) {

	add_elem(simul->particles, p);
}

void simulation_next_step (simulation* simul) {

	for (int i = 0; i < simul->particles->nb; i++)
    {
		particle_next_step(simul, (particle*)simul->particles->elems[i]);
	}
}

void particle_next_step (simulation* simul, particle* p) {

	//
	int next_step_quality = 0;

	double dt = simul->sim_params->dt;

	complex_1D* derivative = wave_func_derivative(simul, p);

	// filter_order_1_rigorous(&derivative, 0.005);

	// On calcule la nouvelle fonction d'onde et on actualise 'p->last_wave_functions' :
	next_step_with_derivative(p->last_wave_functions, NB_LAST_WAVE_FUNC, derivative, dt, next_step_quality);
	
	destroy_complex_1D(derivative);

	return;
}

complex_1D* wave_func_derivative (simulation* simul, particle* p) {

	//
	int laplacian_quality = 0;

	complex_1D* current_wave_func = p->last_wave_functions[0];

	int length = current_wave_func->length;
	double dx = current_wave_func->dx;

	complex_1D* laplacian_field = laplacian_1D_field(current_wave_func, laplacian_quality, ZERO);

	// On calcule la dérivée de la fonction d'onde avec l'équation de Schrodinger :

	complex_1D* derivative = create_complex_1D(length, dx);

	complex delta = { 0.0, 0.0 };
	complex laplacian = { 0.0, 0.0 };
	complex wave_func = { 0.0, 0.0 };
	complex field_V_times_wave_func = { 0.0, 0.0 };

	double h_barre = simul->constants->h_barre;

	for (int i = 0; i < length; i++) {

		wave_func = { current_wave_func->re[i] , current_wave_func->im[i] };

		laplacian = { laplacian_field->re[i] , laplacian_field->im[i] };

		field_V_times_wave_func.re = simul->field_V->data[i] * wave_func.re;
		field_V_times_wave_func.im = simul->field_V->data[i] * wave_func.im;

		derivative->re[i] =  (-h_barre * h_barre / (2 * p->m) * laplacian.im + field_V_times_wave_func.im) / h_barre;
		derivative->im[i] = -(-h_barre * h_barre / (2 * p->m) * laplacian.re + field_V_times_wave_func.re) / h_barre;

	}

	destroy_complex_1D(laplacian_field);

	return derivative;
}


