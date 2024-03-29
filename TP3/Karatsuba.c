#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

/*
 * Représentation élémentaire d'un polynôme. 
 * On pourrait, au minimum, rajouter le degré !
 */

struct poly
{   double* T;     /* le tableau des coefficients */
    bool dynamic;  /* indique si T a été alloué dynamiquement */
    int n;         /* la taille du tableau */
};

/*
 * Constructeur. Le tableau T est alloué dynamiquement 
 */
void init_poly (struct poly* A, int n)
{
    A->T = (double*)malloc (n * sizeof (double));
    assert (A->T != (double*)0);
    A->n = n;
    A->dynamic = true;
}

/*
 * Autre constructeur. Le tableau T est passé en paramètre.
 * Il n'est donc pas alloué dynamiquement 
 */
void init2_poly (struct poly* A, double* T, int n)
{
    A->T = T;
    A->n = n;
    A->dynamic = false;
}

/*
 * Destructeur.
 */
void clear_poly (struct poly* A)
{
    if (A->dynamic)
        free (A->T);
}

/*
 * L'impression d'un polynôme est plus technique qu'on ne le croit :-)
 * On suppose que les doubles sont en fait des entiers
 */
void print_poly (struct poly* A)
{   long c;
    int i;
    bool b;

    b = false;
    for (i = A->n - 1; i >= 0; i--)
    {   if (A->T[i] != 0.0)
        {   if (A->T[i] > 0) 
            {   if (b) printf (" + "); 
            } else 
            {   if (b) printf (" - "); else printf ("-");
            }
            b = true;
            if (A->T[i] > 0) c = (long)A->T[i]; else c = (long) - A->T[i];
            if (i == 0)
                printf ("%ld", c);
            else if (i == 1)
            {   if (c != 1) printf ("%ld*x", c); else printf ("x");
            } else
            {   if (c != 1) printf ("%ld*x^%d", c, i); else printf ("x^%d", i);
            }
        }
    }
    printf ("\n ");
}

/*
 * R = A + B
 */
void add_poly (struct poly* R, struct poly* A, struct poly* B, int* fn)
{   int i;
    i = 0;
    while (i < A->n && i < B->n)
    {   R->T[i] = A->T[i] + B->T[i];
        i += 1;
        *fn += 1;
    }
    while (i < A->n)
    {   R->T[i] = A->T[i];
        i += 1;
    }
    while (i < B->n)
    {   R->T[i] = B->T[i];
        i += 1;
    }
    while (i < R->n)
    {   R->T[i] = 0.0;
        i += 1;
    }
}

/*
 * R = A - B
 */
void sub_poly (struct poly* R, struct poly* A, struct poly* B, int* fn)
{   int i;
    i = 0;
    while (i < A->n && i < B->n)
    {   R->T[i] = A->T[i] - B->T[i];
        i += 1;
        *fn += 1;
    }
    while (i < A->n)
    {   R->T[i] = A->T[i];
        i += 1;
    }
    while (i < B->n)
    {   R->T[i] = - B->T[i];
        i += 1;
        *fn += 1;
    }
    while (i < R->n)
    {   R->T[i] = 0.0;
        i += 1;
    }
}

/*
 * R = A * B par la méthode élémentaire
 */
void mul_poly (struct poly* R, struct poly* A, struct poly* B, int* fn)
{   int a, b;
    for (a = 0; a < R->n; a++)
        R->T[a] = 0.0;
    for (a = 0; a < A->n; a++)
        for (b = 0; b < B->n; b++)
        {
            R->T[a+b] = R->T[a+b] + A->T[a] * B->T[b];
            *fn += 2;
        }
}

/*
 * R = A * B par la méthode de Karatsuba
 */
void Karatsuba (struct poly* R, struct poly* A, struct poly* B, int* fn)
{   struct poly R0, R1, R2, A0, A1, B0, B1, tmpA, tmpB;
    int i, p, q;
/*
 * L'algorithme élémentaire est utilisé pour les cas de base
 */
    if (A->n == 1 || B->n == 1)
        mul_poly (R, A, B,fn);
    else
    {
        if (A->n < B->n)
            p = A->n/2;
        else
            p = B->n/2;
/*
 * Découper les polynômes A et B en deux se fait en temps constant 
 */
        init2_poly (&A0, A->T, p);
        init2_poly (&A1, A->T + p, A->n - p);
        init2_poly (&B0, B->T, p);
        init2_poly (&B1, B->T + p, B->n - p);
/*
 * Les polynômes R0 et R2 sont des sous-tableaux de R.
 * On évite ainsi deux recopies de tableaux.
 */
        init2_poly (&R0, R->T, 2*p-1);
        init2_poly (&R2, R->T + 2*p, A->n + B->n - 1 - 2*p);
        Karatsuba (&R0, &A0, &B0, fn);
        Karatsuba (&R2, &A1, &B1, fn);
/*
 * À ce stade, R = R0 + x^(2*p)*R2. On calcule maintenant R1.
 */
        if (A0.n > A1.n) q = A0.n; else q = A1.n;
        init_poly (&tmpA, q);
        add_poly (&tmpA, &A0, &A1,fn);
        if (B0.n > B1.n) q = B0.n; else q = B1.n;
        init_poly (&tmpB, q);
        add_poly (&tmpB, &B0, &B1,fn);
        q = tmpA.n + tmpB.n - 1;
        if (q < R0.n) q = R0.n;
        if (q < R2.n) q = R2.n;
        init_poly (&R1, q);
        Karatsuba (&R1, &tmpA, &tmpB,fn);
        sub_poly (&R1, &R1, &R0,fn);
        sub_poly (&R1, &R1, &R2,fn);
/*
 * R = R + x^p*R1
 */
        R->T[2*p-1] = 0;
        for (i = 0; i < R1.n; i++)
        {
            R->T[p+i] = R->T[p+i] + R1.T[i];
            *fn += 1;
        }

        clear_poly (&A0);
        clear_poly (&A1);
        clear_poly (&B0);
        clear_poly (&B1);
        clear_poly (&R0);
        clear_poly (&R1);
        clear_poly (&R2);
        clear_poly (&tmpA);
        clear_poly (&tmpB);
    }
}

int main ()
{   struct poly A, B, R;
    int i, n, N, fn;

    srand48 ((long)421);

    N = 1000;              /* taille des polynômes */
    init_poly (&A, N);
    init_poly (&B, N);
    init_poly (&R, 2*N-1);
    for (i = 0; i < N; i++)
    {   A.T [i] = floor (10.0 * drand48 () - 5.0);
        B.T [i] = floor (10.0 * drand48 () - 5.0);
    }
    printf("n   f(n)\n");
    for (n = 1; n < N; n++)
    {   A.n = n;
        B.n = n;
        fn = 0;
//         mul_poly (&R, &A, &B,&fn);
//         print_poly (&R);
//         R.fn = 0;
        Karatsuba (&R, &A, &B,&fn);
//         print_poly (&R);
        printf("%d  %d\n",n,fn);
    }
//     printf ("\n\nMultiplication de polynômes aléatoires de taille croissante\n");
//     printf ("par la méthode élémentaire, puis par celle de Karatsuba.\n");
//     printf ("Si le programme est correct, on doit voir des polynômes de\n");
//     printf ("taille de plus en plus grande, affichés deux fois chacun.\n\n");
//     printf ("Lors du TP, vous pouvez supprimer ces affichages.\n\n");
}
