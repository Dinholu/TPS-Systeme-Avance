# Projet_Systeme_Avance

## :notebook_with_decorative_cover: Sommaire

- [Minishell](#minishell)
- [Description](#description)
- [Fonctionnalités](#fonctionnalités)
- [Instructions de compilation](#instructions-de-compilation)
- [Utilisation](#utilisation)
    - [Commandes classiques](#commandes-classiques)
    - [Variables d'environnement](#variables-denvironnement)
    - [Gestion des alias](#gestion-des-alias)
- [Documentation Doxygen](#documentation-doxygen)
- [Aperçu technique](#aperçu-technique)
    - [Composants principaux](#composants-principaux)
    - [Logique globale](#logique-globale)
    - [Gestion des inclusions](#gestion-des-inclusions)
- [Mode Batch](#mode-batch)
- [Structure des répertoires](#structure-des-répertoires)
- [Dépendances](#dépendances)
- [Bugs connus](#bugs-connus)
- [Améliorations futures](#améliorations-futures)

---

## Minishell

Ceci est le repository du projet de système avancé de l'année 2024-2025. Il s'agit d'un projet de type shell nommé Minishell. Ce projet a été réalisé par [**@Vladimir**](https://github.com/Vladimir9595) et [**@Alizée**](https://github.com/Dinholu).

## Description

Le projet **Minishell** est un shell minimaliste développé en langage C. L'objectif principal est de reproduire une interface de type shell capable de :

- Lire et exécuter des commandes simples,
- Gérer les erreurs,
- Manipuler les variables d'environnement,
- Gérer les signaux,
- Implémenter des commandes internes (built-ins).

## Fonctionnalités

- Exécution de commandes simples
- Gestion des opérateurs logiques (&&, ||)
- Support des redirections (>, <, >>)
- Gestion des pipes (|)
- Variables d'environnement
- Commandes en arrière-plan (&)
- Historique des commandes
- Commandes internes (cd, pwd, echo, env, alias)
- Mode batch permettant de passer une commande directement en argument

## Instructions de compilation

Afin d'utiliser ce shell, il suffit de cloner le repository et de compiler le programme avec les commandes suivantes :

```bash
cd Project/Minishell_Proj/
mkdir build/ && cd build/
cmake ../Minishelln/
make
./minishell
```

## Utilisation

Pour utiliser le Minishell, il suffit de compiler le projet et d'exécuter l'exécutable généré.

### Commandes classiques

Voici quelques exemples d'utilisation du Minishell :

```bash
ls && pwd
```

Cette commande affiche la liste des fichiers du répertoire courant, suivie du chemin absolu si `ls` a réussi.

```bash
false || who
```

Cette commande affiche les utilisateurs connectés, car `false` échoue et l'opérateur `||` force l'exécution de `who`.

```bash
ls -l | grep "Makefile"
```

Cette commande affiche uniquement les lignes contenant "Makefile" parmi la liste des fichiers détaillés.

### Variables d'environnement

Le shell permet de définir et d'utiliser des variables d'environnement :

```bash
ti=toto
echo $ti
```

La commande ci-dessus affiche `toto`.

### Gestion des alias

Le Minishell prend en charge la gestion des alias :

```bash
alias ll="ls -al"
ll
```

Cette commande affiche la liste des fichiers en mode détaillé avec les fichiers cachés.

## Documentation Doxygen

Afin de générer la documentation Doxygen, il suffit de se rendre dans le répertoire `Project/Minishell_Proj/` et de lancer la commande suivante :

```bash
doxygen Doxyfile
```

Ensuite pour accéder à la documentation, il suffit d'ouvrir le fichier `Project/Minishell_Proj/docs/html/index.html` dans un navigateur web.

---

## Aperçu technique

### Composants principaux

Les différentes parties de l'application sont organisées en plusieurs modules principaux :

1. **Parser (parser.c)** :
   - Analyse les chaînes de commandes saisies par l'utilisateur.
   - Découpe les commandes en fonction des opérateurs de contrôle (pipes, redirections, etc.).
   - Fournit les fonctions `parse_command`, `split_pipes`, et `parse_control_operators`.

2. **Exécuteur (executor.c)** :
   - Gère l'exécution des commandes.
   - Prend en charge les redirections, les pipes et les opérateurs logiques.
   - Fonctionnalités principales : `execute_command`, `handle_redirections`, et `execute_piped_commands`.

3. **Built-ins (builtins.c)** :
   - Implémente les commandes internes comme `cd`, `pwd`, `env`, `alias`, et `exit`.
   - Gère la manipulation des variables d'environnement et des alias.

4. **Historique (history.c)** :
   - Gère l'enregistrement et l'affichage des commandes entrées par l'utilisateur.

5. **Main (main.c)** :
   - Point d'entrée principal du programme.
   - Initialise les variables globales et lance la boucle principale du shell.

6. **Typedef (typedef.h)** :
   - Définit les structures de données utilisées dans le projet.

### Logique globale

La logique du projet repose sur les étapes suivantes :

1. **Lecture de la commande utilisateur** :
   - Récupération de la saisie utilisateur via `readline`.

2. **Parsing** :
   - Analyse et segmentation de la commande en fonction des opérateurs logiques, redirections et pipes.
   - Stockage des informations sous forme de structures telles que `CommandNode`.

3. **Exécution** :
   - Vérification si la commande correspond à un built-in.
   - Gestion des redirections et exécution de la commande.
   - Gestion des opérateurs logiques (&&, ||) et de l'exécution en arrière-plan.

4. **Historique et alias** :
   - Enregistrement des commandes dans l'historique.
   - Gestion des alias pour simplifier les commandes utilisateur.

5. **Boucle principale** :
   - Répétition de ce cycle jusqu'à la saisie de la commande `exit`.

### Gestion des inclusions

Le projet est organisé avec des fichiers d'en-têtes qui regroupent les déclarations nécessaires pour chaque module. Voici les principaux fichiers d'en-têtes :

- `builtins.h` : Déclarations des commandes internes.
- `executor.h` : Déclarations des fonctions d'exécution des commandes.
- `history.h` : Gestion de l'historique des commandes.
- `parser.h` : Analyse des commandes utilisateur.
- `typedef.h` : Définitions des structures et constantes globales.

---

## Mode batch

Le mode batch permet de lancer le shell avec une commande en argument.
Le lancement du mode batch s'effectue une fois que l'executable a été crée avec la commande `make`.

Afin d'utiliser le mode batch, il suffit de lancer une commande comme par exemple :

```bash
./minishell -c "echo "titi" && echo "tata" && echo "toto""
```

Dans cet exemple, le shell va afficher les trois mots `titi`, `tata` et `toto` à la suite.

## Structure des répertoires

Le projet est organisé de la manière suivante :

```bash
Project/
├── Minishell_Proj/
│   ├── Minishelln/
│   │   ├── include/
│   │   │   ├── builtins.h
│   │   │   ├── executor.h
│   │   │   ├── history.h
│   │   │   ├── parser.h
│   │   │   └── typedef.h
│   │   ├── src/
│   │   │   ├── builtins.c
│   │   │   ├── executor.c
│   │   │   ├── history.c
│   │   │   ├── main.c
│   │   │   └── parser.c
│   │   ├── CMakeLists.txt
│   │   └── Makefile
│   └── Doxyfile
├── .gitignore
└── README.md
```

## Dépendances

Le projet dépend des outils suivants :

- **GCC** pour la compilation
- **CMake** pour la génération des builds
- **Readline** pour la gestion des entrées utilisateur

## Bugs connus

- Problèmes possibles avec certaines redirections complexes.
- Historique non sauvegardé entre les sessions.
- Alias non persistants.

## Améliorations futures

- Implémentation d'une persistance de l'historique et des alias.
- Gestion avancée des redirections multiples.
- Support des scripts shell complexes.
- Optimisation des performances de parsing et d'exécution.
