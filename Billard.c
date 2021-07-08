#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>




//FONCTION ALÉATOIRE ENTRE 2 POINTS
double random_ab(double a, double b){
  return (rand()*(b-a)/(double)RAND_MAX)+a;
  
}


//écrit dans un fichier fich les coordonnées d'un cercle de rayon r centré en 0
void circle(FILE * fich, double r){
	
	double x;
	double y1,y2;

for(x=-r; x<=r; x+=0.0001){
	
	y1 = sqrt((pow(r,2))-pow(x,2));
	y2 = -sqrt((pow(r,2))-pow(x,2));
	fprintf(fich, "%f %f %f\n", x, y1,y2);
}
}


//RENVOIE UN ANGLE AU HASARD, POUR REPRÉSENTER COUP DANS LA BOULE
double angle_random(){

  return random_ab(-M_PI,M_PI);
}

//TROUVE RAYON POINT TANGENT
double r_point_tangente(double r, double angle_coup){
	
	return r*sin(angle_coup);
}

//TROUVE ANGLE DU POINT TANGENT
double angle_point_tangente(double r, double r_prime, double angle_boule){

	double tmp = acos(r_prime/r);
	
	return angle_boule - tmp;
}

//TROUVE ANGLE D'INCIDENCE
double angle_incidence(double rayon_billard, double rayon_prime){
	
	return asin(rayon_prime/rayon_billard);
}


//TROUVE EQUATION DE LA TRAJECTOIRE DE LA BOULE DE BILLARD POUR UN ANGLE DONNE
double trajectoire(double r, double phi, double theta){

	return fabs(r/(cos(theta-phi)));
}

//TROUVE L'ANGLE DU REBOND
double angle_rebond(double po, double phi){
	
	return -acos(po) + phi;
}





 int main(int argc, char const *argv[]) {


	
	//fichier pour position des rebonds
  FILE * fich = fopen("Billard.txt", "w");
	//fichier pour le cercle représentant le billard
  FILE * cercleBillard = fopen("cercleBillard.txt","w");
  
	//fichier pour le cercle de la caustique
  FILE * cercleCaustique = fopen("cercleCaustique.txt","w");
  
	//pipe pour tracer sur gnuplot à l'éxécution
  FILE * gnuplotPipe = popen("gnuplot -persistent", "w");
  	

   double rayon_billard = 1;
   int nb_rebonds_MAX;
   
   //compteur de rebond
   int nb_rebonds = 0;
	
	
	//ANGLE ET RAYON DE LA BOULE AU DÉPART
	double rayon_boule=0.5;
	double angle_boule= M_PI/4;;


	//ANGLE DU COUP
	double angle_coup;
	
	
	//ANGLE POUR DÉTERMINER CAUSTIQUE
	double angle_caus;
	
	//PRÉCISION POUR COMPARER ANGLE
	double eps = 0.005;
	
	
	//ANGLE ET RAYON DU POINT D'INTERSECTION ENTRE DROITE RADIALE ET DROITE DU COUP
	double r_prime;
	double teta_prime;

	
	//ANGLE INCIDENCE
   double angle_inc;
   
   //NOMBRE DE POINTS DE REBONDS
   int nb_pts_caust;


	//ANGLE QUI RETIENT LA BONNE VALEUR DU COUP
	double angle_coup_bon;

	//nombre de rebonds maximum
   printf("Nombre de rebonds max ? \n\n");
   scanf("%d", &nb_rebonds_MAX);
   
   //nombre de points de rebonds différents
   printf("\nNombre de rebonds différents : ? \n\n");
   scanf("%d", &nb_pts_caust);
   
   if(nb_pts_caust>=nb_rebonds_MAX){
	   printf("\nErreur, nombre de rebonds différents supérieur à nombre de rebonds max.\n");
	   return 0;
   }
   
   //variation de l'angle de tir
   for(angle_coup=0; angle_coup<2*M_PI; angle_coup += 0.005){
	   
	   //calcul de r0 pour cet angle de coup
	   r_prime= r_point_tangente(rayon_boule, angle_coup);
	   //calcul du premier phi
	   teta_prime = angle_point_tangente(rayon_boule, r_prime, angle_boule);	
	   //calcul de l'angle d'incidence pour cet angle de coup
        angle_inc = angle_incidence(rayon_billard, r_prime);
		
		
		while(nb_rebonds < nb_rebonds_MAX ){
	
			//nouveau point de rebond
			angle_boule = angle_rebond(r_prime, teta_prime);
		
			//nouveau phi
			teta_prime = angle_point_tangente(rayon_billard, r_prime, angle_boule);
		
			//on travaille modulo 2pi, donc on rajoute ou enlève 2*pi quand on dépasse
			if(angle_boule < -2*M_PI) angle_boule += 2*M_PI;
			if(angle_boule > 2*M_PI) angle_boule -= 2*M_PI;
			if(teta_prime < -2*M_PI) teta_prime += 2*M_PI;
			if(teta_prime > 2*M_PI) teta_prime -= 2*M_PI;
	

			nb_rebonds++;
			
			//on mémorise la position du premier rebond
			if(nb_rebonds == 1) angle_caus = angle_boule;
		
			//On vérifie si le rebond courant est à la même position que le premier
			if(angle_coup != 0 && nb_rebonds == nb_pts_caust+1 && angle_boule >= angle_caus - eps && angle_boule <= angle_caus + eps){
				angle_coup_bon = angle_coup;
			}
	
	}
	nb_rebonds=0;
}


//ON RECALCULE LES REBONDS POUR LE BON ANGLE DE COUP, ET ON LES ENREGISTRE DANS UN FICHIER POUR PLOTER
		printf("Pour ro = %f, teta = pi/4, le bon angle de coup pour avoir une trajectoire à %d rebonds est %f\n\n", rayon_boule, nb_pts_caust, angle_coup_bon);
	  
	    r_prime= r_point_tangente(rayon_boule, angle_coup_bon);
	    teta_prime = angle_point_tangente(rayon_boule, r_prime, angle_boule);	
        angle_inc = angle_incidence(rayon_billard, r_prime);
		printf("\nAngle d'incidence : %f\n\n", angle_inc);

	while(nb_rebonds < nb_rebonds_MAX){
		
        	angle_boule = angle_rebond(r_prime, teta_prime);
	
			fprintf(fich,"%12f \t %.12f\n", cos(angle_boule), sin(angle_boule));
			teta_prime = angle_point_tangente(rayon_billard, r_prime, angle_boule);
			
			if(angle_boule < -2*M_PI) angle_boule += 2*M_PI;
			if(angle_boule > 2*M_PI) angle_boule -= 2*M_PI;
			if(teta_prime < -2*M_PI) teta_prime += 2*M_PI;
			if(teta_prime > 2*M_PI) teta_prime -= 2*M_PI;
	

			nb_rebonds++;
		}
		
	//trace les cercles du billard, et la caustique
	circle(cercleBillard, rayon_billard);	
	circle(cercleCaustique, fabs(r_prime));
		
		//commande gnuplot
	fprintf(gnuplotPipe, "plot 'Billard.txt' with line notitle, 'cercleBillard.txt' lt rgb \"green\" with line notitle, '' using 1:3 lt rgb \"green\" with line notitle, 'cercleCaustique.txt' using 1:2  lt rgb \"red\" with line notitle, '' using 1:3 lt rgb \"red\" with line notitle\nset xlabel 'x'\nset ylabel 'y'\nset size square\nreplot\n");
	fflush(gnuplotPipe);
	
   return 0;
 }
