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
	int N;										//**Taille du probleme: Nombre de t�ches
	std::vector<int> Temps;						//**Temps de r�alisation pour effectuer une t�che. NB: Tableau de 0 � N-1
	std::vector<int> Poids;						//**Poids (importance) de la t�che. NB: Tableau de 0 � N-1
	std::vector<int> DateDue;					//**Date due (�ch�ance) de la t�che. NB: Tableau de 0 � N-1
	TProblemType Type = MIN;					//**Type du probleme (e.g.Maximisation ou Minimisation)
};

struct TSolution						//**Definition d'une solution: 
{
	std::vector <int> Seq;				//**Indique la s�quence de r�alisation des t�ches. NB: Tableau de 0 a N-1
	long FctObj = INT_MAX;				//**Valeur de la fonction obj: Sommation des retards pond�r�s
	bool Valide = false;				//**Etat de validite de la solution (presence de chacune des taches (sans doublons) + longueur de la solution + domaine admissible des variables)
	int NbEvaltoGet;					//**Nombre d'evalutions de solutions necessaires pour trouver cette solution... pour estimer convergence
	std::vector<int> Fin;				//**Temps de terminaison d'une t�che dans la s�quence
	std::vector<int> Retard;			//**Indique le nombre d'unit�s de temps qu'une t�che est r�alis�e en retard: Max {0, Fin-DateDue}
	std::vector<int> wRetard;			//**Calcul le retard pond�r� pour une t�che (Retard*Poids)
};

struct TAlgo							//D�finition du Recuit Simul�:
{
	double	TemperatureCourante;		//Temperature courante du systeme																		//***NEW***
	double	TemperatureInitiale;		//Temperature initiale du systeme																		//***NEW***
	int		Delta;						//D�gradation courante entre la solution Courante et la solution Next									//***NEW***
	double	Alpha;						//Schema de "reduction de la temp�rature" : Temp(t+1) = Alpha * Temp(t)									//***NEW***
	int		NbPalier;					//Schema de "Duree de la temp�rature": Duree Temperature = NB_EVAL_MAX/NbPalier							//***NEW***

	int		CptEval=0;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	int		TailleVoisinage;			//**Nombre de solutions voisines generees a chaque iteration
	long	FctObjSolDepart;			//**Valeur de la fonction objectif de la solution au depart de l'algorithme
};

#endif