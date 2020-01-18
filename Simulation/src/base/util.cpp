

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"


general_set* create_general_set () {

	general_set* set = (general_set*) malloc (sizeof(general_set));
	set->nb = 0;
	set->elems = NULL;
	return set;
}

void add_elem (general_set* set, void* elem) {

	if (set->nb == 0) {

		set->elems = (void**) malloc (sizeof(void*));
		set->elems[0] = elem;
		set->nb = 1;
	}
	else {

		set->elems = (void**) realloc ( set->elems, (set->nb + 1) * sizeof(void*) );
		set->elems[set->nb] = elem;
		set->nb += 1;
	}
	return;
}

void  remove_elem (general_set* set, void* elem) {

	int pos = 0;

	while ( set->elems[pos] != elem && pos < set->nb )
		pos++;

	if (pos == set->nb) {
		fprintf(stderr, "Attention : un élément passé à 'remove_elem(general_set* ...)' n'a pas été trouvé dans le 'set'.\n");
		return;
	}

	while (pos < set->nb - 1)
		set->elems[pos] = set->elems[pos+1];

	set->elems = (void**) realloc (set->elems, (set->nb - 1) * sizeof(void*));

	set->nb -= 1;
	return;
}

void destroy_set (general_set* set) {
	free(set->elems);
	free(set);
	return;
}


scalar_1D* create_scalar1D (int length) {

	scalar_1D* field = (scalar_1D*) malloc(sizeof(scalar_1D));

	field->length = length;
	field->data = (float*) malloc( length*sizeof(float) );

	for (int i = 0; i < length; i++) {
		field->data[i] = 0.0;
	}

	return field;
}

void destroy_scalar1D (scalar_1D* field) {

	free(field->data);
	free(field);

	return;
}

scalar_2D* create_scalar2D (int height, int width) {

	scalar_2D* field = (scalar_2D*) malloc(sizeof(scalar_2D));

	field->width = width;
	field->height = height;
	field->data = (float*) malloc(width * height * sizeof(float));

	for (int i = 0; i < width * height; i++) {
		field->data[i] = 0.0;
	}

	return field;
}

void destroy_scalar2D (scalar_2D* field) {

	free(field->data);
	free(field);

	return;
}


FILE* new_file (char* path, int number_at_end) {

	// On vérifie que l'adresse du fichier n'est pas trop longue :
	int k = 0;
	while (path[k] != '\0')
		k++;
	if (k > 1800) {
		fprintf(stderr, "Erreur : le nom du fichier à sauvegarder est trop long, la sauvegarde n'a pas été effectuée\n");
		return NULL;
	}

	//
	int max_length = 2050;

	if (max_length <= 60)
		fprintf(stderr, "%s\n", "Erreur dans 'new_file(path)'");

	char* file_path = (char*)malloc(max_length);
	int i = 0;
	while (path[i] != '\0' && i < max_length-50) {  // Attention, il faut que 'i' s'arrête bien avant 2048 car si le fichier existe déjà on devra rajouter un nombre au début
		file_path[i] = path[i];
		i++;
	}
	file_path[i] = '\0';

	FILE* file;
#ifdef _WIN32
	fopen_s(&file, file_path, "r");
#else
    file = fopen(file_path, "r");
#endif

	// Tant qu'un fichier du même nom existe, on change le nom de celui-ci jusqu'à pouvoir créer le fichier :

	int puissance_10 = 3;
	int max = 1;
	for (int j = 0; j < puissance_10; j++)
		max *= 10;

	// Si on doit changer le nom, on décale la fin de l'adresse (le nom du fichier) pour insérer un nombre :

	int position = i;

	char limiter = number_at_end == 1 ? '.' : '\\';
	 
	if (file != NULL) {

		for (int k = i; k > 0 && file_path[k] != limiter; k--) {
			position--;
		}
		if (number_at_end == 0)
			position++;

		for (int j = i-1; j >= position; j--) {
			file_path[j+ puissance_10 + 1] = file_path[j];
		}

		if (number_at_end) {
			file_path[position] = ' ';
			position++;
		} else {
			file_path[position + puissance_10] = ' ';
		}

		file_path[i + puissance_10 + 1] = '\0';
	}

	int j = 0;
	while (file != NULL && j < max) {
		fclose(file);

		char num = '0';
		int tmp1 = max / 10;
		int tmp2 = j;

		for (int num_position = 0; num_position < puissance_10; num_position++) {

			num = '0' + (char)(tmp2 / tmp1);
			tmp2 = tmp2 - (tmp2 / tmp1) * tmp1;
			tmp1 = tmp1 / 10;
			
			file_path[position + num_position] = num;
		}

#ifdef _WIN32
	    fopen_s(&file, file_path, "r");
#else
        file = fopen(file_path, "r");
#endif

		j += 1;
	}

	if (file != NULL)
		fclose(file);
	// NB : si les " 10 ^ puissance_10 " (100, 1000, 1000000...) noms testés étaient déjà pris, alors on remplace le dernier.

	// On crée le nouveau fichier :
#ifdef _WIN32
	fopen_s(&file, file_path, "w");
#else
    file = fopen(file_path, "w");
#endif
	if (file == NULL) {
		fprintf(stderr, "Erreur : echec de la creation du fichier, verifiez que l'adresse est valide (y compris que les eventuels dossiers existent)\n");
		return NULL;
	}

	free(file_path);

	return file;
}

float get_time_in_seconds() {
    float time = ((float)clock()) / CLOCKS_PER_SEC;
    return time;
}

int max (int a, int b) {
	return a > b ? a : b;
}

float get_rand_float() {
	return 1.0 * rand() / RAND_MAX;
}

double get_rand_double() {
	return ((double)1.0) * rand() / RAND_MAX;
}

int my_log2 (int a) {
	if (a <= 0) {
		fprintf(stderr, "Erreur dans my_log2(...) : paramètre <= 0.\n");
		return -1;
	}

	int exposant = 8*sizeof(int);

	while ((1 << exposant & a) == 0)
		exposant -= 1;

	return exposant;
}

void write_float(FILE* file, float f) {

	/* Méthode Fun :

	  // On pouvait utiliser : " unsigned char* c = reinterpret_cast<unsigned char*>(&f); "

	unsigned char c;
	unsigned char* bytes_tab = ((unsigned char*)(&f));  // tableau d'octets

	for (int i = 0; i < sizeof(float); i++) {

		c = bytes_tab[i];
		fprintf(file, "%c", c);
		printf("%c",c);
	}
	*/

	fwrite(&f, sizeof(float), 1, file);
}

float read_float(FILE* file) {

	float f;

	fread(&f, sizeof(float), 1, file);

	return f;
}





