# TP2 - Réponses

## Matrice / Matrice

Evidement, le mode non parallélisé est plus long, et le temps ne varie pas en fonction du nombre de threads impliqués.

Le mode parallélisé avec ordonancement statique petrmet de réduire énormément le temps de calcul. Cependant, on peut constater une stagnation du speedup, (et donc une baisse de l'efficacité à partir de 6 threads) En effet ma machine de disposant que de 6 threads (3 coeurs), il est inutile de paraléllisé le programme plus que ça.

Le mode dynamic semble être un petit peu en dessous du static en terme d'efficacité. En effet, le mode dynamic n'est par pertinant ici, car toutes les boucles vont faire le même nombre d'information. Le mode dynamic va donc perdre du temps à demander à chaque fois les prochaines instructions. Ce temps ne sera pas perdu avec le mode static, puisque chaque thread reçoit le même nombre d'instructions à effectuer au début.
