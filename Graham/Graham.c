#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "liste_point.h"
#include "point.h"

#define N 10
#define MAX_COORDINATES 100
#define SCENARIO 78738

int main ()
{   struct point T[N];
    struct liste_point L;
    FILE* f;
    int i;
    struct point cour, prec;

    srand48 (SCENARIO);
/* 
 * On crée N points. Le point A est en (0,0).
 * On les enregistre dans "points.dat"
 */
    init_point (&T[0], 0, 0, 'A');
    for (i = 1; i < N; i++)
        init_point (&T[i], drand48 () * MAX_COORDINATES, 
                           drand48 () * MAX_COORDINATES, 'A' + i);
    f = fopen ("points.dat", "w");
    assert (f != NULL);
    for (i = 0; i < N; i++)
        fprintf (f, "%f %f %c\n", T[i].x, T[i].y, T[i].ident);
    fclose (f);
/* 
 * On trie T [1 .. N-1] par angle croissant.
 * On s'assure qu'il n'y a pas deux points alignés.
 */
    qsort (T+1, N-1, sizeof (struct point), &compare_points);
    for (i = 1; i < N-1; i++)
        assert (compare_points (&T[i], &T[i+1]) != 0);
/*
 * À FAIRE : BOUCLE PRINCIPALE DE L'ALGORITHME DE GRAHAM.
 *           UTILISER L POUR LA PILE DE POINTS.
 */

    init_liste_point(&L);
    ajouter_en_tete_liste_point(&L,&T[0]);
    ajouter_en_tete_liste_point(&L,&T[1]);
    
    i = 2;
    while(i<N)
    {
        cour = *(L.tete->value);
        prec = *(L.tete->next->value);
        if(tourne_a_gauche(&prec,&cour,&T[i]))
        {
            ajouter_en_tete_liste_point(&L,&T[i]);
            i=i+1;
        }else
        {
            extraire_tete_liste_point(&cour,&L);
        }
    }



    f = fopen ("enveloppe.dat", "w");
    assert (f != NULL);
    fprintf (f, "%f %f\n", T[0].x, T[0].y);
/*
 * À FAIRE : IMPRIMER TOUS LES POINTS DE L DANS "enveloppe.dat"
 */

    
    while(L.tete == (struct maillon_point*)0)
    {
        fprintf(f,"%f %f\n",cour.x, cour.y);
        extraire_tete_liste_point(&cour,&L);
    }

    fclose (f);
/* 
 * plot [-10:100][-10:100] "points.dat" with labels, "enveloppe.dat" with lines
 */
    clear_liste_point (&L);
    return 0;
}
