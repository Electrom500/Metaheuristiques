#ifndef __ENTETE_H_
#define __ENTETE_H_

#include <windows.h> 
#include <cstdio>
#include <cstdlib> 
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>  
#include <cmath>
#include <vector>
#include <functional>
#include <algorithm>
using namespace std;

//Ne pas modifier TProblem
enum TProblemType { MIN, MAX };					//** Definition du sens de l'optimisation du probleme

struct TProblem									//**Definition du probleme:
{
	std::string Nom;							//**Nom du fichier de donnees
	int NbVilles;								//**Taille du probleme: Nombre de villes (incluant les villes de depart et d'arrivee qui sont fixes)
	std::vector<std::vector <int> > Distance;	//**Distance entre chaque paire de villes. NB: Tableau de 0 a NbVilles-1.  Indice 0 utilise
	std::vector<std::vector <int> > Pred;		//**Pour chaque ville: Liste des villes qui doivent d'abord etre visitees
												//NB: Premiere dimension du Tableau de 0 a NbVilles-1. Deuxieme dimension sera variable selon le nombre de predecesseurs
	std::vector<std::vector <int> > Succ;		//**Pour chaque ville: Liste des villes qui doivent etre visites apres (successeur)
												//NB: Premiere dimension du Tableau de 0 a NbVilles-1. Deuxieme dimension sera variable selon le nombre de successeurs
	TProblemType Type = MIN;					//**Type du probleme (e.g.Maximisation ou Minimisation)
};

//Ne pas modifier TSolution
struct TSolution						//**Definition d'une solution: 
{
	std::vector <int> Seq;				//**Sequence dans laquelle les villes sont visitees. NB: Tableau de 0 a NbVilles-1. NB: Positions 0 et NbVilles-1 sont fixes
	long FctObj = INT_MAX;				//**Valeur de la fonction obj: Distance totale
	bool Valide=false;					//**Etat de validité de la solution: respect des contraintes de precedence + si chaque ville est presente une et une seule fois. Verifie lorsque la solution est evaluee
	int NbEvaltoGet;					//**Nombre d'evalutions de solutions necessaires pour trouver cette solution... pour estimer convergence
};

//Ne pas modifier TAlgo
struct TAlgo
{
	int		TaillePop;					//**Taille de la population (nombre d'individus). NB: valeur minimale = 2
	int		TaillePopEnfant;			//**Taille de la populationEnfant (nombre d'enfants = TaillePop * ProbCr)
	double	ProbCr;						//**Probabilite de croisement [0%,100%]
	double	ProbMut;					//**Probabilite de mutation [0%,100%] 
	int		Gen;						//**Compteur du nombre de generations (iterations)

	int		CptEval=0;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	long	FctObjSolDepart;			//**Valeur de la fonction objectif de la meilleure solution lors de la 1ere generation (iteration)
};
#endif