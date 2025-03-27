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
#include <limits>
#include <functional>
#include <algorithm>
using namespace std;

enum eProb { ALPINE, SPHERE, SCHWEFEL, MAXSAT };		//Ne pas modifier

//### STRUCTURE SERVANT UNIQUEMENT AU PROBLEME DE MAXSAT --- NE SERA PAS UTILISE DANS LE CAS DES FONCTIONS CONTINUES
struct tClause
{
	int NbVar;								//**Nombre de variables (litteraux) dans la clause
	int Poids;								//**Poids de la clause
	std::vector <int> Litt;					//**Litteraux: Vecteur qui indique le numero de la variable (0 a NbVar-1) ainsi que son signe (+ ou -)
};

struct tProblem								//**Definition du probleme:
{
	eProb	Nom;							//**Nom de la fonction ou du probleme a traiter
	int		D;								//**Nbre de Variables (dimensions)
	double	Xmin;							//**Domaine des variables: valeur minimale 
	double	Xmax;							//**Domaine des variables: valeur maximale

	//### PARTIE DE LA DEFINITION DU PROBLEME SERVANT UNIQUEMENT AU PROBLEME DE MAXSAT --- NE SERA PAS UTILISE DANS LE CAS DES FONCTIONS CONTINUES
	std::string NomFic;						//**Nom du fichier de donnees: instance MaxSat
	int NbClause;							//**Nbre de clauses indiques dans le fichier
	std::vector <tClause> Clause;			//**Definition des NbClause (Longueur, poids, details). NB: Tableaux de 0 a NbClause-1.
};

struct tPosition							//**Definition de la position d'une particule (solution): 
{
	std::vector<double>		X;				//**Position actuelle de la particule pour chacune des dimensions: definie en reelle mais dans le cas du MaxSat, sera utilise comme vecteur de booleen
	double					FctObj;			//**Valeur de la fonction objectif
	int NbEvaltoGet;						//**Nombre d'evalutions de solutions necessaires pour trouver cette solution... pour estimer convergence
};

struct tParticule							//**Definition d'une particule: 
{
	tPosition				Pos;			//**Position actuelle de la particule
	std::vector<double>		V;				//**Vitesse actuelle de la particule pour chacune des dimensions
	std::vector<tParticule*> Info;			//**Liste des informatrices de la particule
	tPosition				BestPos;		//**Meilleure position de la particule (experience)
};

struct tAlgo
{
	int		TailleEssaim;					//**Taille de l'essaim (nombre de particules) 
	double	C1;								//**Coefficient de confiance en soi
	double	C2;								//**Coefficient de confiance en son experience
	double	C3;								//**Coefficient de confiance en sa meilleure informatrice
	int		NbInfo;							//**Nombre d'informatrices pour chaque particule
	double	Vmax;							//**Vitesse maximale														//### Pour MAXSAT
	int		Iter;							//**Compteur du nombre d'iterations				
	double	Precision;						//**Niveau de precision souhaite pour les problemes a variables continues	//### Pour fonctions continues
	int		CptEval;						//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;					//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	double	FctObjSolDepart;				//**Valeur de la fonction objectif de la meilleure solution au depart de l'agorithme
};

#endif