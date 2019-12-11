

#include <math.h>

#include "util.h"
#include "complex.h"



complex_1D* create_complex_1D (int length, double dx) {

	complex_1D* field = (complex_1D*) malloc (sizeof(complex_1D));
	field->length = length;
	field->dx = dx;

	field->re = (double*) malloc (length * sizeof(double));
	field->im = (double*) malloc (length * sizeof(double));

	for (int i = 0; i < length; i++) {
		field->re[i] = 0.0;
		field->im[i] = 0.0;
	}
	return field;
}

void destroy_complex_1D (complex_1D* field) {
	free(field->re);
	free(field->im);
	free(field);
}

complex_2D* create_complex_2D (int height, int width, double dx, double dy) {

	complex_2D* field = (complex_2D*) malloc (sizeof(complex_2D));
	field->width = width;
	field->height = height;
	field->dx = dx;
	field->dy = dy;

	field->re = (double*) malloc (width * height * sizeof(double));
	field->im = (double*) malloc (width * height * sizeof(double));

	for (int i = 0; i < width * height; i++) {
		field->re[i] = 0.0;
		field->im[i] = 0.0;
	}
	return field;
}

void destroy_complex_2D (complex_2D* field) {
	free(field->re);
	free(field->im);
	free(field);
}


/***
		Utile et calcul :
***/


complex get_value (complex_1D* field, int index, behaviour_at_border MODE) {

	switch (MODE) {

	case ZERO:
		if (index < 0 || index >= field->length)
			return { 0.0, 0.0 };
		else
			return { field->re[index], field->im[index] };
		break;

	case CONSTANT:
		if (index < 0)
			return { field->re[0], field->im[0] };
		else if (index >= field->length)
			return { field->re[field->length - 1], field->im[field->length - 1] };
		else
			return { field->re[index], field->im[index] };
		break;

	case LOOP:
		if (index < 0) {
			while (index < 0)
				index += field->length;
			return { field->re[index], field->im[index] };
		}
		else if (index >= field->length)
			return { field->re[index % field->length], field->im[index % field->length] };
		else
			return { field->re[index], field->im[index] };
		break;

	case CONSTANT_DERIVATIVE:
		fprintf(stderr, "Avertissement dans 'get_value(...)' : MODE non pris en charge.\n");
		break;

	case CONSTANT_2ND_DERIVATIVE:
		fprintf(stderr, "Avertissement dans 'get_value(...)' : MODE non pris en charge.\n");
		break;

	default:
		fprintf(stderr, "Erreur dans 'get_value(...)' : MODE non reconnu.\n");
		break;
	}
	return { 0, 0 };
}

complex_1D* derivative_1D_field (complex_1D* field, int shift_direction, int quality, behaviour_at_border MODE) {

	//// SEUL le cas quality == 0 est pris en charge, ici

	complex_1D* derivative = create_complex_1D(field->length, field->dx);
	double dx = field->dx;

	complex values[5];  // l'usage de values[] dépend de quality.

		// Pour le cas 'quality == 0' : décalage vers la droite <=> shift_direction > 0
	int shift_start = shift_direction > 0 ? -1 : 0;
	int shift_next = shift_direction > 0 ? 0 : 1;

	switch (quality) {

	case 1 :  // ici : values[0] = f(x-h) et values[1] = f(x+h)

		for (int i = 0; i < field->length; i++) {
			values[0] = get_value(field, i + 1, MODE);
			values[1] = get_value(field, i - 1, MODE);
			derivative->re[i] = (values[1].re - values[0].re) / (2*dx);
			derivative->im[i] = (values[1].im - values[0].im) / (2*dx);
		}
		break;

	case 2:  // ici : values[0] = f(x-2*h) ; values[1] = f(x-h) ; values[2] = f(x+h) ; values[3] = f(x+2*h)
		double tmp1, tmp2;
		for (int i = 0; i < field->length; i++) {
			values[0] = get_value(field, i - 2, MODE);
			values[1] = get_value(field, i - 1, MODE);
			values[2] = get_value(field, i + 1, MODE);
			values[3] = get_value(field, i + 2, MODE);

			tmp1 = (values[3].re - values[0].re) / (4*dx);
			tmp2 = (values[2].re - values[1].re) / (2*dx);
			derivative->re[i] = -1.0/3 * tmp1 + 4.0/3 * tmp2;

			tmp1 = (values[3].im - values[0].im) / (4*dx);
			tmp2 = (values[2].im - values[1].im) / (2*dx);
			derivative->im[i] = -1.0/3 * tmp1 + 4.0/3 * tmp2;
		}
		break;

	case 3:  // ici : values[0] = f(x-4*h) ; values[1] = f(x-3*h) ; values[2] = f(x-2*h) ; values[3] = f(x-h) ; values[4] = f(x)
		
		for (int i = 0; i < field->length; i++) {
			values[0] = get_value(field, i-4, MODE);
			values[1] = get_value(field, i-3, MODE);
			values[2] = get_value(field, i-2, MODE);
			values[3] = get_value(field, i-1, MODE);
			values[4] = get_value(field, i, MODE);

			derivative->re[i] = 1.0 / dx * ( 1.0/4 * values[0].re - 4.0/3 * values[1].re + 3 * values[2].re - 4 * values[3].re + 25.0/12 * values[4].re);
			derivative->im[i] = 1.0 / dx * ( 1.0/4 * values[0].im - 4.0/3 * values[1].im + 3 * values[2].im - 4 * values[3].im + 25.0/12 * values[4].im);
			
		}
		break;

	case 4:  // ici : values[0] = f(x-4*h) ; values[1] = f(x-3*h) ; values[2] = f(x-2*h) ; values[3] = f(x-h) ; values[4] = f(x)

		for (int i = 0; i < field->length; i++) {
			values[0] = get_value(field, i - 4, MODE);
			values[1] = get_value(field, i - 3, MODE);
			values[2] = get_value(field, i - 2, MODE);
			values[3] = get_value(field, i - 1, MODE);
			values[4] = get_value(field, i, MODE);

			derivative->re[i] = 1; // Numerical Differentiation Formulas  ?
			derivative->im[i] = 1; // Numerical Differentiation Formulas  ?

		}
		break;

	default:  // cas quality == 0

		// ici : values[0] = f[x+shift_start] et values[1] = f(x+shift_next)
		//  <=>  values[0] = f(x-h/2)         et values[1] = f(x+h/2)

		for (int i = 0; i < field->length; i++) {
			values[0] = get_value(field, i + shift_start, MODE);
			values[1] = get_value(field, i + shift_next, MODE);
			derivative->re[i] = (values[1].re - values[0].re) / dx;
			derivative->im[i] = (values[1].im - values[0].im) / dx;
		}
		break;
	}
	return derivative;
}

complex_1D* laplacian_1D_field (complex_1D* field, int quality, behaviour_at_border MODE) {

	//// SEUL le cas quality == 0 est pris en charge, pour l'instant.

	complex_1D* first_derivative = derivative_1D_field (field,             1, quality, MODE);
	complex_1D* laplacian        = derivative_1D_field (first_derivative, -1, quality, MODE);

	destroy_complex_1D(first_derivative);
	return laplacian;
}

complex laplacian_1D (complex_1D* field, int index, behaviour_at_border MODE) {

	complex res;

	double dx = field->dx;

	if (index >= 1 && index <= field->length - 2) {

		// On obtient bien le bon résultat avec " / (dx*dx) " :
		res.re = ( field->re[index+1] - 2 * field->re[index] + field->re[index-1] ) / (dx*dx);
		res.im = ( field->im[index+1] - 2 * field->im[index] + field->im[index-1] ) / (dx*dx);
	}
	else if (index == 0 || index == field->length - 1) {

		complex previous = get_value(field, index - 1, MODE);
		complex next     = get_value(field, index + 1, MODE);

		// On obtient bien le bon résultat avec " / (dx*dx) " :
		res.re = (next.re - 2 * field->re[index] + previous.re) / (dx*dx);
		res.im = (next.im - 2 * field->im[index] + previous.im) / (dx*dx);
	}
	else {
		fprintf(stderr, "Erreur dans 'laplacian(...)' : index hors des limites\n");
		res = { 0.0, 0.0 };
	}

	return res;
}

void next_step_with_derivative (complex_1D** fields_tab, int size_fields_tab, complex_1D* derivative, double dt, int quality) {

    /*  DANS ce commentaire, on prend l'exemple size_fields_tab == 5, mais la fonction peut aussi marcher pour size_fields_tab != 5 (et >= 1)
		(cela dépend toutefois de quality).

        Calcule le nouveau champ complexe à partir de sa dérivée à l'instant considéré,

             à partir d'un tableau 'fields_tab' de 5 complex_1D* tels que :
                - fields_tab[0] est le champ à l'instant t-1
                - fields_tab[1] est le champ à l'instant t-2
                - fields_tab[2] est le champ à l'instant t-3
                - fields_tab[3] est le champ à l'instant t-4
                - fields_tab[4] est le champ à l'instant t-5

            et 'derivative' est la dérivée souhaitée du champ à l'instant courant.

            A la fin de l'exécution :
                - fields_tab[0] est le champ à l'instant t
                - fields_tab[1] est le champ à l'instant t-1
                - fields_tab[2] est le champ à l'instant t-2
                - fields_tab[3] est le champ à l'instant t-3
                - fields_tab[4] est le champ à l'instant t-4

            // ATTENTION : ici, on fait l'approximation : " f'(t) = ...f(t-5).....f(t-1) ",
                alors que la formule correcte est :  f'(t) = ...f(t-4).....f(t)
                (cela impliquerait de manipuler des formules propres à chaque équation différentielle étudiée).
    */

    // On calcule d'abord le nouveau champ (à l'instant t),
    // en le mettant à la place du champ 'fields_tab[4]' :

    complex_1D* new_field = create_complex_1D(fields_tab[0]->length, fields_tab[0]->dx);

    complex_1D* previous_field = fields_tab[0];

	complex values[5];

    switch (quality) {

    case 1:
		fprintf(stdout,"AZEAZE");
		for (int i = 0; i < new_field->length; i++) {
			  // MODE (mis à ZERO ci-dessous) est inutile ici.
			values[0] = get_value(fields_tab[0], i, ZERO);  // à t-4
			values[1] = get_value(fields_tab[0], i, ZERO);  // à t-3
			values[2] = get_value(fields_tab[0], i, ZERO);  // à t-2
			values[3] = get_value(fields_tab[0], i, ZERO);  // à t-1

			new_field->re[i] = 12.0 / 25 * (dt*derivative->re[i] - 1.0 / 4 * values[0].re + 4.0 / 3 * values[1].re - 3 * values[2].re + 4 * values[3].re);
			new_field->im[i] = 12.0 / 25 * (dt*derivative->im[i] - 1.0 / 4 * values[0].im + 4.0 / 3 * values[1].im - 3 * values[2].im + 4 * values[3].im);
		}
        break;

    default:

		for (int i = 0; i < new_field->length; i++) {

			new_field->re[i] = previous_field->re[i] + derivative->re[i] * dt;
			new_field->im[i] = previous_field->im[i] + derivative->im[i] * dt;
		}
        break;

    }

    // Ensuite, on met le nouveau champ en tête du tableau,
    // et on décale les autres en écrasant le plus ancien (fields_tab[4]) :

      // on supprime le plus ancien champ :
    destroy_complex_1D(fields_tab[size_fields_tab - 1]);

      // décalage des 4 premiers tableaux :
    for (int i = size_fields_tab - 1; i > 0; i -= 1)
        fields_tab[i] = fields_tab[i-1];

      // placement du nouveau champ en tête du tableau :
    fields_tab[0] = new_field;

    return;
}

double  complex_module  (double re, double im) {
	return sqrt(re*re + im*im);
}

double  complex_arg  (double re, double im) {

	if (re == 0.0 && im == 0.0)
		return 0.0;
	if (re == 0.0 && im > 0.0)
		return PI/2;
	if (re == 0.0 && im < 0.0)
		return 3*PI/2;

	double arg;

	if (re >= 0)
		arg = atan( im / re );
	else
		arg = PI + atan( im / re );

	if (arg < 0)
		arg += 2*PI;

	return arg;
}

double get_square_module_integral (complex_1D* field) {

	/*
		*** On peut se contenter d'additionner directement les valeurs ici, car :

			- on a une précision 'double'
			- les valeurs additionnées sont d'un ordre de grandeur proche
			- il y a moins de 1 000 000 de valeurs à additionner
				(même avec beaucoup plus, ça devrait passer,
				la mantisse des 'double' est de 53 bits (dont le 1 initial))

		*** aussi, on n'est pas obligé de calculer exactement les modules,

			ça marche aussi en additionnant les carrés des parties réelles entre elles
			et les carrés des parties imaginaires entre elles
			(en effet, puisque les modules sont mis au carré avant d'être additionnés,
			c'est comme s'il n'y avait pas la racine).
	*/

	double sum = 0.0;

	for (int i = 0; i < field->length; i++) {

		sum += field->re[i] * field->re[i];
		sum += field->im[i] * field->im[i];
	}
	// intégrale = sum / field->length * (x_length)  avec x_length = field->length * dx,
	// donc : intégrale = sum * dx;
	return sum * field->dx;
}

double  get_real_integral  (complex_1D* field) {
	double sum = 0.0;
	for (int i = 0; i < field->length; i++)
		sum += field->re[i];
	return sum * field->dx;
}

double  get_imaginary_integral  (complex_1D* field) {
	double sum = 0.0;
	for (int i = 0; i < field->length; i++)
		sum += field->im[i];
	return sum * field->dx;
}

void  normalize_module_square (complex_1D* field) {

	double coeff = sqrt( get_square_module_integral(field) );

	if (coeff <= 0) {
		fprintf(stdout, "Attention (dans normalize_module_square(field)) : intégrale <= 0, impossible de normaliser le carré du module.\n");
		return;
	}

	for (int i = 0; i < field->length; i++) {

		field->re[i] /= coeff;
		field->im[i] /= coeff;
	}
}

void  make_centered_gaussian(complex_1D* field, double sigma)  {

	double x;

	for (int i = 0; i < field->length; i++) {

		x = (i - (field->length - 1.0) / 2) * field->dx;

		field->re[i] = exp( - x*x / (2*sigma*sigma) ) / (sigma * sqrt(2*PI));
		field->im[i] = 0.0;
	}
}

//
//
// Pour FILTER_ORDER_1 :
//
//		OK - soit : Essayer avec meilleurs formules de dérivées

//		(pas fait)						(mais d'abord essayer de comprendre le pb ?)

//		OK - soit : Utiliser les trucs de l'Aide Mémoire de SIC : x(n) -> y(n+k) += alpha*k*exp(-alpha'*k)
//
//		OK - soit : faire la FFT du signal, couper les HF, et fait l'IFFT.
//
//

void filter_order_1(complex_1D** field, double tau) {
	/*
		Modifie " *field_pt " en le faisant pointer vers un nouveau 'complex_1D',
		qui correspond au résultat du filtrage de 'field'

		Pour le fonctionnement, voir schéma sur téléphone :
			la version mise en oeuvre est LA 3EME VERSION
			(celle avec "Enfin", et non PAS la version avec écrit "Choisi")
	*/
	complex_1D* new_field = create_complex_1D((*field)->length, (*field)->dx);

	if ((*field)->length > 0) {
		new_field->re[0] = (*field)->re[0];
		new_field->im[0] = (*field)->im[0];
	}
	if ((*field)->length > 1) {
		new_field->re[0] = (*field)->re[1];
		new_field->im[0] = (*field)->im[1];
	}
	for (int i = 1; i < (*field)->length - 1; i++) {

		new_field->re[i+1] = new_field->re[i-1] + 2 * new_field->dx / tau * ( (*field)->re[i] - new_field->re[i]);
		new_field->im[i+1] = new_field->im[i-1] + 2 * new_field->dx / tau * ( (*field)->im[i] - new_field->im[i]);

	}
	destroy_complex_1D(*field);
	*field = new_field;
}

void filter_order_1_rigorous(complex_1D** field, double tau) {

    //
    double percent = 0.999;
    double b = 1.0 / tau;
    double dx = (*field)->dx;

    int len = (*field)->length;

	complex_1D* new_field = create_complex_1D((*field)->length, (*field)->dx);
    for (int i = 0; i < len; i++) {
        new_field->re[i] = 0.0;
        new_field->im[i] = 0.0;
    }

    // on doit d'abord repérer le nombre de points 'nb_points' de la réponse impultionnelle au-delà duquel elle-ci devient négligeable.
    // Pour cela on calcule l'intégrale de la réponse impultionnelle et prend 'nb_points' tel que : intégrale entre 0 et nb_points = intégrale totale * [percent]  (ex: 95%, 99%,...).

    double integral = 0.0;
      // pour calculer l'intégrale totale, on calcule en réalité l'intégrale entre 0 et *field->length
    for (int k = 0; k < len; k++) {
        integral += (k+1)*exp(-b*(k+1)*dx);
    }

    int nb_points = 0;
    double integral_tmp = 0.0;

    while (nb_points < len && integral_tmp < integral * percent) {
        integral_tmp += (nb_points+1)*exp(-b*(nb_points+1)*dx);
        nb_points += 1;
    }
      // integral_tmp sera réutilisé plus bas

    // pour ne pas à refaire les calculs à chaque fois, on calcule à l'avance la réponse impultionnelle :
    double* signal = (double*) malloc (nb_points * sizeof(double));

    for (int k = 0; k < nb_points; k++) {
        signal[k] = (k+1)*exp(-b*(k+1)*dx) / integral_tmp;
    }

    // on effectue ensuite le filtrage : pour chaque valeur de '*field', on remplace le "pic" correspondant à cette valeur par la réponse impultionnelle du filtre :

      // on suppose que 'new_field' a été créé avec des valeurs nulles : 0.0

    int end;
    for (int i = 0; i < len; i++) {
        end = i + nb_points > len ? len - i : nb_points;
        for (int j = 0; j < end; j++) {
            new_field->re[i+j] += (*field)->re[i] * signal[j];
            new_field->im[i+j] += (*field)->im[i] * signal[j];
        }
    }

    // on fait en sorte que le signal conserve son intégrale :
    double coeff_real = get_real_integral(new_field) == 0 ? 1.0 : get_real_integral(*field) / get_real_integral(new_field);
    double coeff_imaginary = get_imaginary_integral(new_field) == 0 ? 1.0 : get_imaginary_integral(*field) / get_imaginary_integral(new_field);
    
    for (int i = 0; i < len; i++) {
        new_field->re[i] *= coeff_real;
        new_field->im[i] *= coeff_imaginary;
    }

    // fin :
	destroy_complex_1D(*field);
	*field = new_field;
}

void filter_2(complex_1D** field_pt, double coeff) {
	/*
		Correspond au cas de la 2è image sur le téléphone (avec le titre "Choisi")

		Fonctionnement :
			nouvelle dérivée = ancienne dérivée * (1-coeff) + dérivée moyenne * coeff
	*/
	complex_1D* field = *field_pt;
	complex_1D* new_field = create_complex_1D(field->length, field->dx);

	double dx = field->dx;
	new_field->re[0] = field->re[0];
	new_field->im[0] = field->im[0];

	double medium_gradient;
	double new_derivative;
	double central_value;

	int i = 1;
	while (i < field->length - 2) {

		// Re :
		medium_gradient = 0.5 * ( (field->re[i+1]-field->re[i-1])/(2*dx) + (field->re[i+2]-field->re[i])/(2*dx) );
		new_derivative = (1-coeff) * (field->re[i+1] - field->re[i])/dx + coeff * medium_gradient;

		central_value = ( field->re[i] + field->re[i+1] ) / 2;

		new_field->re[i]   = central_value - new_derivative * dx / 2;
		new_field->re[i+1] = central_value + new_derivative * dx / 2;

		// Im :
		medium_gradient = 0.5 * ( (field->im[i+1]-field->im[i-1])/(2*dx) + (field->im[i+2]-field->im[i])/(2*dx) );
		new_derivative = (1-coeff) * (field->im[i+1] - field->im[i])/dx + coeff * medium_gradient;

		central_value = ( field->im[i] + field->im[i+1] ) / 2;

		new_field->im[i]   = central_value - new_derivative * dx / 2;
		new_field->im[i+1] = central_value + new_derivative * dx / 2;

		i += 2;
	}
	while (i < field->length) {
		new_field->re[i] = field->re[i];
		new_field->im[i] = field->im[i];
		i++;
	}
	destroy_complex_1D(field);
	*field_pt = new_field;
}

void fft(complex_1D* source, complex_1D* target, int inversed) {

	int len = source->length;

	// on vérifie que 'source' et 'target' ont la même longueur
	if (target->length != len) {
		fprintf(stderr, "Erreur dans fft(...) : 'source' et 'target' n'ont pas la meme longueur.\n");
		return;
	}

	// on vérifie que la longueur est correcte (>= 1 et une puissance de 2)

	int exp = my_log2(len);

	if (len <= 1 || len != 1<<exp) {
		fprintf(stderr, "Erreur dans fft(...) : la longueur de 'source' doit être une puissance de 2 (c'est-a-dire en 2^N et avec N >= 1).\n");
		return;
	}

	// FFT ou IFFT :

	// on copie d'abord source dans target :
	for (int i = 0; i < len; i++) {
		target->re[i] = source->re[i];
		target->im[i] = source->im[i];
	}

	// permutations :

	  // pour échanger 2 valeurs :
	double re_tmp = 0.0;
	double im_tmp = 0.0;
	int indice;
	int indice_next;

	// pour le calcul d'indice_next :
	int indice_tmp;
	int offset;
	int step;

	for (indice = 1; indice < len - 1; indice++) {

		// Calcul de l'indice suivant (indice_next) :
		indice_tmp = indice;
		offset = 0;
		step = len / 2;

		for (int i = 0; i < exp - 1; i++) {

			if (indice_tmp % 2 == 0) {
				indice_tmp = indice_tmp / 2;
				// (offset += 0)
			}
			else {
				indice_tmp = (indice_tmp - 1) / 2;
				offset += step;
			}
			step /= 2;
		}
		indice_next = offset + indice_tmp;

		// échange des valeurs :
		if (indice_next > indice) {

			re_tmp = target->re[indice];
			im_tmp = target->im[indice];

			target->re[indice] = target->re[indice_next];
			target->im[indice] = target->im[indice_next];

			target->re[indice_next] = re_tmp;
			target->im[indice_next] = im_tmp;

		}

	}

	// Calculs sur les couples :
	int nb_groups = len / 2;
	int group_size = 1;
	int indice1, indice2;  // indice1 (resp. indice2) est l'indice du premier (resp. deuxième) élément du couple traité dans 'target'
	double re1, re2, im1, im2;
	double arg;  // correspond à 'arg' dans les 'e^(i*arg)' qui multipleront les éléments des couples

	int sign = inversed == 0 ? -1 : 1;  // selon que l'on fasse la transformée inverse ou non
	
	for (int i = 0; i < exp; i++) {  // chaque itération correspond à l'application de la FFT à un "étage" de récursion (dans la description récursive de la FFT)

		for (int i_group = 0; i_group < nb_groups; i_group++) {  // chaque itération correspond au traitement d'un groupe de couples (voir illustration faite sur Paint)

			for (int i_couple = 0; i_couple < group_size; i_couple++) {  // chaque itération correspond au traitement d'1 couple

				indice1 = i_group * 2 * group_size + i_couple;
				indice2 = indice1 + group_size;

				// nb_groups coïncide avec N/2, où N est le nombre de points dans les listes à "l'étage de récursion" qui est en train d'être traité.
				// Donc : - 2*PI / (N) = - 2*PI*i_group / (2*nb_groups)
				//					   = - PI*i_group / nb_groups
				arg = sign * PI * i_couple / group_size;

				re1 = target->re[indice1];
				im1 = target->im[indice1];
				re2 = target->re[indice2];
				im2 = target->im[indice2];

				target->re[indice1] = re1 + cos(arg) * re2  - sin(arg) * im2 ;
				target->im[indice1] = im1 + sin(arg) * re2  + cos(arg) * im2 ;
				target->re[indice2] = re1 - (cos(arg) * re2 - sin(arg) * im2);
				target->im[indice2] = im1 - (sin(arg) * re2 + cos(arg) * im2);

				if (inversed == 0) {
					target->re[indice1] /= 2;
					target->im[indice1] /= 2;
					target->re[indice2] /= 2;
					target->im[indice2] /= 2;
				}

			}
		}
		nb_groups  /= 2;
		group_size *= 2;
	}

	/* Remarque : ici, on traite le cas de l'IFFT à la ligne : " int sign = inversed == 0 ? -1 : 1; "
		pour ensuite calculer " arg = sign * PI * i_couple / group_size; ".

		On aurait aussi pu faire l'IFFT en faisant :

			if (inversed != 0) {
				for (int i = 0; i < len; i++) {
	    			target->im[i] *= -1;
			}

		AVANT les permutations ET APRES les calculs sur les couples.

	*/

	return;
}

void filter_cutting_high_freq (complex_1D* field, complex_1D* fourier_transform, float percent_cut) {

	fft(field, fourier_transform, 0);

	// Attention, il ne faut pas couper la fin de 'target_for_fft' car elle correspond aux fréquences négatives
	// (par périodicité), et inclut les basses fréquences négatives.

	int highest_frequency_index = field->length / 2;
	int nb_cut_frequencies = (int) round( fourier_transform->length * percent_cut );

	for (int i = highest_frequency_index - nb_cut_frequencies / 2; i < highest_frequency_index + (nb_cut_frequencies+1) / 2; i++) {
		fourier_transform->re[i] = 0.0;
		fourier_transform->im[i] = 0.0;
	}

	fft(fourier_transform, field, 1);
}

void filter_attenuating_high_freq (complex_1D* field, complex_1D* fourier_transform) {

	fft(field, fourier_transform, 0);

	// Attention, il ne faut pas atténuer la fin de 'target_for_fft' car elle correspond aux fréquences négatives
	// (par périodicité), et inclut les basses fréquences négatives.

    int len = field->length;

	//int highest_frequency_index = len / 2;
	//int nb_cut_frequencies = round( fourier_transform->length * percent_attenuated );
    //int start = highest_frequency_index - nb_cut_frequencies / 2;
    //int end = highest_frequency_index + (nb_cut_frequencies+1) / 2;
    double distance; // représente la "distance" entre le point traité et le plus court des bords à partir desquels on ne fait pas de traitement.
    int nb = len/100;

	for (int i = 0; i < len; i++) {
	//for (int i = start; i < end; i++) {
        //distance = i-start-1 > end-i ? i-start-1 : end-i;
		//fourier_transform->re[i] *= 1.0 / distance;
		//fourier_transform->im[i] *= 1.0 / distance;
        distance = i <= field->length / 2 ? i : len-i-1;
		fourier_transform->re[i] *= exp( - nb * distance / (len/2) );
		fourier_transform->im[i] *= exp( - nb *  distance / (len/2) );
	}

	fft(fourier_transform, field, 1);
} 


/***
		Affichage :
***/

void  arg_to_pix (pixel* pix, double real, double imaginary) {

	if (real == 0 && imaginary == 0) {
		set_pixel(pix, 0.0, 0.0, 0.0, 1.0);
		return;
	}

	double arg = complex_arg (real, imaginary);
	double r = 0.0;
	double g = 0.0;
	double b = 0.0;

	if (arg < PI/2) {
		r = 1.0 - arg / (PI/2);
		g =       arg / (PI/2);
	}
	else if (arg < PI) {
		g = 1.0 - (arg - PI/2) / (PI/2);
		b = (arg - PI / 2) / (PI / 2);
	}
	else if (arg < 3*PI/2) {
		b = 1.0;
		r = (arg-PI) / (PI/2);
	}
	else {
		b = 1.0 - (arg - 3*PI/2) / (PI/2);
		r = 1.0;
	}
	set_pixel(pix, r, g, b, 1.0);
}


complex_1D_graph*  create_complex_1D_graph (complex_1D* field,
											int x_0, int y_0, int y_1,
											int line_width, complex_1D_graph_mode MODE) {

	complex_1D_graph* graph = (complex_1D_graph*) malloc (sizeof(complex_1D_graph));

	graph->x_0 = x_0;
	graph->y_0 = y_0;

	graph->size = field->length;
	graph->line_width = line_width;

	graph->y = (int*) malloc (field->length * sizeof(int));
	graph->pixels = (pixel*) malloc (field->length * sizeof(pixel));

	pixel* pix = create_pixel (0.0,0.0,0.0,1.0);
	int y;

	if (MODE == REAL)
		set_pixel(pix, 1.0, 0.0, 0.0, 1.0);
	if (MODE == IMAGINARY)
		set_pixel(pix, 0.0, 1.0, 0.0, 1.0);

	for (int i = 0; i < field->length; i++) {

		switch (MODE) {

		case MODULE_WITH_ARG :
			arg_to_pix (pix, field->re[i], field->im[i]);
			graph->y[i] = complex_module(field->re[i], field->im[i]) * y_1;
			break;

		case MODULE_NO_ARG :
			graph->y[i] = complex_module(field->re[i], field->im[i]) * y_1;
			break;

		case REAL :
			graph->y[i] = field->re[i] * y_1;
			break;

		case IMAGINARY :
			graph->y[i] = field->im[i] * y_1;
			break;

		default:
			fprintf(stderr, "Erreur dans create_complex_1D_graph() : MODE non-reconnu");
			destroy_complex_1D_graph(graph);
			return NULL;
			break;

		}

		set_pixel ( graph->pixels + i, pix );

	}
	return graph;
}

void  destroy_complex_1D_graph(complex_1D_graph* graph) {
	free(graph->y);
	free(graph->pixels);
	free(graph);
}

void  complex_1D_graph_to_picture(picture* pic, complex_1D_graph* graph) {

	pixel* pix = NULL;

	int i_start = graph->x_0 >= 0 ? 0 : 0 - graph->x_0;
	int i_end   = graph->x_0 + graph->size <= pic->width ? graph->size - 1 : pic->width - graph->x_0 - 1;

	int y_start;
	int y_end;

	int x;
	int index;  // index des pixels de l'image 'picture'

	int local_line_width;

	for (int i = i_start; i <= i_end; i++) {

		x = graph->x_0 + i;
		pix = graph->pixels + i;

		if (graph->line_width <= 0) {

			y_start = graph->y[i] >= 0 ? graph->y_0  : graph->y_0 + graph->y[i];
			y_end   = graph->y[i] >= 0 ? graph->y_0 + graph->y[i] : graph->y_0 ;
		}
		else {

			y_start = graph->y_0 + graph->y[i] - (graph->line_width-1) / 2;
			y_end   = graph->y_0 + graph->y[i] - (graph->line_width-1) / 2 + graph->line_width - 1;

			// pour que la ligne ne soit pas discontinue,
			// s'il y a trop d'écartement entre les points :
			modify_Y_start_end_to_link_points(&y_start, &y_end, graph, i);

		}

		if (y_start < 0)
			y_start = 0;

		if (y_end >= pic->height)
			y_end = pic->height - 1;

		for (int y = y_start; y <= y_end; y++) {

			index = 4 * ( y * pic->width + x );
			set_pixel(pic->pixels + index, pix->r, pix->g, pix->b, pix->a, REPLACE);

		}

	}
	return;
}

// Fonction utile pour complex_1D_graph_to_picture(...)
void modify_Y_start_end_to_link_points (int* y_start, int* y_end, complex_1D_graph* graph, int i) {

	if (i > 0 && i < graph->size - 1) {

		// si on est sur un maximum local :
		if (graph->y[i] > graph->y[i-1] && graph->y[i] > graph->y[i+1]) {
			*y_start -= max( (graph->y[i] - graph->y[i-1] ) / 2, (graph->y[i] - graph->y[i+1] - 1) / 2 );
			return;
		}

		// si on est sur un minimum local :
		else if (graph->y[i] < graph->y[i-1] && graph->y[i] < graph->y[i+1]) {
			*y_end   += max( (graph->y[i-1] - graph->y[i] ) / 2, (graph->y[i+1] - graph->y[i] - 1) / 2 );
			return;
		}

	}

	// si la valeur courante est comprise entre les deux valeurs adjascentes :
	// (ou sur une extrémité du graphe)
	if (i > 0) {
		if (graph->y[i] > graph->y[i-1])
			*y_start -= ( graph->y[i] - graph->y[i-1] ) / 2;  // pas de '-1' ici (car le point équidistant des 2 valeurs successives sera traité lors du traitement du point suivant)
		if (graph->y[i] < graph->y[i-1])
			*y_end   += ( graph->y[i-1] - graph->y[i] ) / 2;  // pas de '-1' ici
	}
	if (i < graph->size - 1) {
		fprintf(stdout, "a");
		if (graph->y[i+1] > graph->y[i])
			*y_end    += (graph->y[i+1] - graph->y[i] - 1) / 2;  // il faut '-1' ici
		if (graph->y[i+1] < graph->y[i])
			* y_start -= (graph->y[i+1] - graph->y[i] - 1) / 2;  // il faut '-1' ici
	}
	return;
}


picture* create_argument_reference (int x_0, int y_0, int width) {

	picture* pic = create_picture(x_0, y_0, width, width);

	double half_width = width / 2;

	pixel* pix = create_pixel(0.0, 0.0, 0.0, 1.0);
	int index = 0;

	for (int y = 0; y < width; y++) {

		for (int x = 0; x < width; x++) {

			if ( complex_module(x - half_width, y - half_width) <= half_width ) {

				arg_to_pix (pix, x - half_width, y - half_width);
				set_pixel (pic->pixels + index, pix->r, pix->g, pix->b, pix->a, REPLACE);

			} else {
				set_pixel (pic->pixels + index, 0.0, 0.0, 0.0, 0.0, REPLACE);
			}

			index += 4;
		}

	}

	return pic;
}


