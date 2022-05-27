# PROGRAMMATION PARALLÈLE - TP NOTÉ

#### Université de Strasbourg - Licence Informatique S6 - 6 avril 2022

- Ce TP noté est à réaliser individuellement : les communications avec des tiers sont interdites sous peine de nullité du travail.
- Tous les documents sont autorisés.
- Le sujet comprend deux questions.
- Durée totale : 1h30.


------


## Question 1 : OpenMP (10 points)

> **Consignes** : travaillez sur le code source `'stencil1d.c'` à partir des questions posées. Justifiez vos réponses en écrivant votre raisonnement dans le cadre prévu à cet effet dans ce fichier markdown `'tp_note_2022.md'`.
> Les justifications sont indispensables pour obtenir les points de la question.
> Vous déposerez les fichiers source et le fichier `'tp_note_2022.md'` sur moodle sans changer leurs noms.


On cherche à paralléliser avec OpenMP le calcul de type *stencil* suivante. La fonction prend en entrée un vecteur `a`, et effectue pour chacun des éléments `a[i]` du vecteur un calcul qui dépend des éléments voisins `a[i-1]`, `a[i+1]` et de lui même (ligne 8).
Le résultat du calcul pour `a[i]` est stocké en `r[i]`.  
On recommence ensuite le calcul après avoir échangé les vecteurs `r` et `a` (ligne 11)  qui fait que le vecteur `r` précédemment calculé devienne `a` sur lequel on réitère la phase de calcul.
Après avoir itéré `STEPS` fois, la fonction calcule une somme de contrôle très simple consistant à sommer les parties entières du vecteur résultat (lignes 14-15).


```c
1      int stencil1D_reference(double a[N], double r[N]) {
2        r[0]   = a[0];
3        r[N-1] = a[N-1];
4        int  checksum = 0;

5        // compute 1D stencil
6        for (size_t step=0; step < STEPS; step++) {
7           for (size_t i = 1; i < N-1; i++) {
8              r[i] = f(a[i-1], a[i], a[i+1]);
9           }
10           // after each step, make r the new input by exchanging a and r
11           swap(&a, &r);
12        } // all steps done

13        // compute checksum on values in the final step
14        for (size_t i=0; i < N; i++) {
15              checksum += (int)r[i];
16        }
17        return checksum;
18      }
```

### Question 1.1 (3pts)

Parallélisez avec *openmp* la boucle de calcul des lignes 7 à 9. Dans cette question vous pouvez crééer une région parallèle autour des lignes 7 à 9. Justifiez vos choix de directive.
```
Pour cette première question, la directive à utiliser est simplement "parallel for". En effet, comme on ne modifie pas le tableau a, il n'y a aucune dépendences entre les différents tour de boucle.
```

### Question 1.2 (3pts)

Dans la question précédente, on recréé une région parallèle à chaque itération de la boucle (for step =...). Il est plus efficace de ne créer la région parallèle qu'une fois, en la faisant démarrer avant la ligne 5.

Que faut il modifer dans votre code précédent pour que le code parallélisé reste correct. Jutifiez.
```c
// Écrire ici la parallélisation avec une région parallèle créée une seule fois.
// Commentez en une ou deux lignes pour justifier vos choix.
.
.
.
.
.
.
```

### Question 1.3 (2pts)

Parallélisez le calcul de la somme de contrôle. Inclure ci-dessous tout l'extrait de code pertinent et justifiez en commentaires.
```c
  #pragma omp parallel for reduction(+:checksum)
  for (size_t i=0; i < N; i++) {
	  checksum += (int)r[i];
  }

Ici, la boucle est une boucle classiquement parallélisée avec OpenMP, à la différence que l'on veut faire la somme totale de checksum. Donc la directive reduction permet, à la fin de la région paralléle de faire la somme de chaque checksum local de chaque thread. 
```

### Question 1.4 (2pts)

Donnez les meilleures performances obtenues et commentez brièvement le résultat en disant s'il est conforme ou non à vos attentes.

```
Reference time : 8.96998 s
Kernel time -- : 1.54709 s
Speedup ------ : 5.79797
Efficiency --- : 1.93266
OK results :-)   [checksum=29875928]

On voit ici que j'ai un speedup de 5.7 (Temps de référence/nouveau temps). Ce qui est très bon, puisque j'ai 6 processeur sur ma machine, donc le meilleur speedup théoriquement ateignable est 6.
J'ai une efficacité de 1.9, (speedup/nbproc). Ce qui est bien aussi, puisque la meilleure efficacité théoriquement ateignable est 1.
```

-----

## Question 2 : MPI (10 points)

> **Consignes** : travaillez sur le code source `'reduce.c'`. Copiez le code de votre fonction dans le cadre prévu à cet effet dans ce fichier markdown `'tp_note_2022.md'`.
> Vous déposerez les fichiers source et le fichier `'tp_note_2022.md'` sur moodle sans changer leurs noms.

Le but de cet exercice est de réécrire la fonction de réduction de MPI sans utiliser de communication collective. Vous écrirez une fonction dont la déclaration est :

```c
void ma_reduction(long *val, long *res, MPI_Op op)
{
  // Doit effectuer une réduction sur le processus root=0, dans le communicateur
  // MPI_COMM_WORLD, de tous les entiers longs 'val' dans le résultat 'res',
  // en appliquant l'opération de réduction 'op', c'est à dire l'équivalent de :
  // MPI_Reduce(val, res, 1, MPI_LONG, op, 0, MPI_COMM_WORLD);

}
```

À l'intérieur de cette fonction, nous vous demandons de n'utiliser aucune primitive MPI de communication collective, les seuls appels MPI autorisés sont les communications point-à-point (send et recv, bloquants ou non). N'utilisez de fonction non-bloquante que si nécessaire.

Le fonctionnement général de la fonction `ma_reduction` doit être le même que celui de `MPI_Reduce` : tous les processus appellent cette fonction, et au retour le résultat `(res)` doit contenir le résultat de l'opération de réduction pour le processus de rang 0. Pour les autres processus son contenu est indéfini.
Pour résumer, l'appel `ma_reduction(x, y, z);` doit être équivalent à l'appel `MPI_Reduce(x, y, 1, MPI_LONG, z, 0, MPI_COMM_WORLD);`

Pour réaliser ceci, vous transmettrez directement les nombres à accumuler au processus de rang 0 depuis l'ensemble de tous les processus. Le processus de rang 0 effectuera l'accumulation dans le résultat au fur et à mesure des réceptions de messages.

Exemple d'utilisation du programme de test :
```sh
$ mpicc -Wall reduce.c
$ mpirun -n 2 a.out
Nb processus : 2

Test 1 : résultat = 69, résultat MPI = 69
Test 2 : résultat = 1311, résultat MPI = 1311
OK results :-)
$ mpirun -n 20 --oversubscribe a.out
Nb processus : 20

Test 1 : résultat = 100, résultat MPI = 100
Test 2 : résultat = 2584799008153272320, résultat MPI = 2584799008153272320
OK results :-)
```

------

**Votre réponse (code de la fonction uniquement, incluant éventuellement des commentaires) :**

```c
void ma_reduction(long *val, long *res, MPI_Op op)
{
  int rank, size;
  long r = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Status s;
  
  //Si le processus est le rang 0
  if(rank == 0){
    *res += *val;
    // On attend n-1 messages
    for(int i=0; i<size-1; i++){
      long data;
      MPI_Recv(&data, sizeof(long), MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &s);
      if(op == MPI_PROD){
        r = r * data;
      }
      else if(op == MPI_MAX){
        if(data > r){
          r = data;
        }
      }
    }
    *res = r;
  }
  //Sinon on envoie notre nombre
  else{
    MPI_Ssend(val, sizeof(long), MPI_LONG, 0, 0, MPI_COMM_WORLD);
  }
  return;
}
```

**Compléments/explications :**

*Vous pouvez, ici, ajouter toute information que vous jugerez pertinente (exemple : explications complémentaires, démonstration du bon fonctionnement dans différents cas, tests complémentaires, mesures de performance, etc.)*
```
Si le rank = 0, alors ma fonction attent size-1 message et multiplie (ou calcule le max) tout les résultats pour calculer le résultats final.
Si le rank n'est pas zéro, on envoie son résultat au rank = 0.
```

------
*Fin du Sujet*