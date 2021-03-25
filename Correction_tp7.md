# Polytechnique Montréal

Département de génie informatique et génie logiciel

INF1900: Projet initial de système embarqué

# Grille de correction des programmes:

Identification:
+ Travail    : Mise en commun du code et formation de librairies
+ Section #  : 1
+ Équipe #   : 0320
+ Correcteur : Ahmad Faour

# CODE

| Points importants                                        | Poids        |
| -------------------------------------------------------- | ------------ |
| La qualités et le choix de vos portions de code choisies | (3/5.0)      |
| La qualités de vos modifications aux Makefiles           | (3/5.0)      |
| __Résultat partiel__                                     | __(6/10.0)__ |

# RAPPORT

| Points importants                                                           | Poids       |
| --------------------------------------------------------------------------- | ----------- |
| Explications cohérentes par rapport au code retenu pour former la librairie | (2/2.0)     |
| Explications cohérentes par rapport aux Makefiles modifiés                  | (2/2.0)     |
| Explications claires avec un bon niveau de détails                          | (2/2.0)     |
| Bon français                                                                | (1/1.0)     |
| __Résultat partiel__                                                        | __(7/7.0)__ |

# GÉNÉRALITÉS
## Bonne soumission de l'ensemble du code et du rapport selon le format demandé 

| Pénalité par erreur      | -0.5          |
| ------------------------ | ------------- |
| compilation sans erreurs |               |
| etc                      |               |
| __Résultat partiel__     | __(2.5/3.0)__ |


# Résultat

__Total des points: 15.5/20__

# Commentaires du correcteur:
## Commentaire global

## Commentaire par fichiers/Classes
### Button
* Dans la méthode `setSenseControl` il est faut que vous ne devez rien faire dans le `case InterruptSenseControl::LOW_LEVEL`. Vous devez le mettre à `0`.
* Les shifts bits de 0 ne serve à rien
* Il aura des erreurs si on essaie de changer le `InterruptSenseControl` dans la méthode `setSenseControl`
* Pourquoi avoir mis `pseudoPD2=2`?
* Dans les enums c'est des constantes donc `PSEUDO_PD2` et non `pseudoPD2`
* Valeur magique

### Can
* Les noms de classe commence par une lettre majuscule
* Beaucoup de commentaire

### PWM
* Le PWM n'extiste pas uniquement sur le timer 1.
* Séparer l'assignation des `OCR1A/B` et des autres registres de configuration.
* Les namespaces doit commencer par une lettre minuscule donc `pwm` au lieu de `PWM`
### Timer
* Beaucoup de commentaire
* l'utilisation d'un `enum Prescaler` serait meilleur, parce qu'en ce moment vous avez plein de valeur magique dans votre méthode `setPrescaler` et j'ai des millions de possibilité avec votre paramètre `unsigned int prescaler` xD
* Il n'a pas juste le Timer 1! Le Timer 0 et 2 sont très important à configurer aussi!
* Code dupliqué dans `timer::initializeTimer`. Ce code se retrouve dans votre `class Button`
  
### Uart
* Les bits shifts de zéro, ne sont pas nécessaire!
* Un peu trop de commentaire
* Les namespaces doit commencer par une lettre minuscule donc `uart` au lieu de `UART`
* Valeur magique
### Utils
* Le terme `Utils` n'est pas approprié pour ce qui est à l'intérieur de votre `namespace`: `Led`, `Pin`
* Vous auriez pu créer une classe pour gérer les `Led`
* Valeur magique
* Pourquoi vous utilisez `_BV()` juste ici et pas partout?
* Define global inutile `THRESHOLD` elle devrait être local
* Define inutilisé `JUMP`
* Pourquoi l'utilisation d'une variable local statique? `static bool val = false;`. Une raison de plus pour faire des classes!
* Commentaire inutile




## Makefile
* Votre code ne compile pas, vous n'avez pas spécifier le standard c++14 dans vos Makefiles
### Librairie
* Ne pas avoir mis le flags `std=c++14` pour que votre librairie compile
* Constante inutile `HEXROMTRG`, `HEXTRG`, `LDFLAGS`, `HEXFORMAT`
* il aurait était préférable de mettre un nom plus explicite de son utiliter que `MES_OPTIONS`
* Target inutile `install`
* Code inutile

### Exec
* Ne pas avoir mis le flags `std=c++14` pour que votre librairie compile

# Basé sur le commit suivant
```
commit 9b686fc6d13427f7a5bfb70c50485fbc296e4b71
Merge: d7b74ee ff0d303
Author: zakhn <zkhnissi@gmail.com>
Date:   Wed Mar 17 16:14:28 2021 -0400

    ajout du rapport
```

# Fichiers indésirables pertinents
Aucun

# Tous les fichiers indésirables
Aucun

# Sorties de `make` dans les sous-répertoires

## Sortie de `make` dans `codecommun/tp/tp7/lib`
```
make: *** correction_tp7/0320/codecommun/tp/tp7/lib: No such file or directory.  Stop.

```

## Sortie de `make` dans `codecommun/tp/tp7/exec`
```
make: *** correction_tp7/0320/codecommun/tp/tp7/exec: No such file or directory.  Stop.

```
