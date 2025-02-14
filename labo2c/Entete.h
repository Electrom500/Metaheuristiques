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
using namespace std;

enum TProblemType { MIN, MAX };					//** Definition du sens de l'optimisation du probleme

struct TProblem									//**Definition du probleme:
{
	std::string Nom;							//**Nom du fichier de donnees
	int N;										//**Taille du probleme: Nombre de tâches
	std::vector<int> Temps;						//**Temps de réalisation pour effectuer une tâche. NB: Tableau de 0 à N-1
	std::vector<int> Poids;						//**Poids (importance) de la tâche. NB: Tableau de 0 à N-1
	std::vector<int> DateDue;					//**Date due (échéance) de la tâche. NB: Tableau de 0 à N-1
	TProblemType Type = MIN;					//**Type du probleme (e.g.Maximisation ou Minimisation)
};

struct TSolution								//**Definition d'une solution: 
{
	std::vector <int> Seq;				//**Indique la séquence de réalisation des tâches. NB: Tableau de 0 a N-1
	long FctObj = INT_MAX;				//**Valeur de la fonction obj: Sommation des retards pondérés
	bool Valide = false;				//**Etat de validite de la solution (presence de chacune des taches (sans doublons) + longueur de la solution + domaine admissible des variables)
	int NbEvaltoGet;					//**Nombre d'evalutions de solutions necessaires pour trouver cette solution... pour estimer convergence
	std::vector<int> Fin;				//**Temps de terminaison d'une tâche dans la séquence
	std::vector<int> Retard;			//**Indique le nombre d'unités de temps qu'une tâche est réalisée en retard: Max {0, Fin-DateDue}
	std::vector<int> wRetard;			//**Calcul le retard pondéré pour une tâche (Retard*Poids)
};

struct TAlgo							//Définition de la Recherche avec Tabous:
{
	int		LngListeTabous;				//**Longueur de la liste des tabous													//***NEW***
	
	int		CptEval=0;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	int		TailleVoisinage;			//**Nombre de solutions voisines générées à chaque itération
	long	FctObjSolDepart;			//**Valeur de la fonction objectif de la solution au départ de l'algorithme
};

#endif