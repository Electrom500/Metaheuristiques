#ifndef __LIBRAIRIE_H_
#define __LIBRAIRIE_H_

#include "Entete.h"

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
void LectureProbleme(std::string FileName, TProblem& unProb, TAlgo& unAlgo);

//DESCRIPTION: Fonction d'affichage a l'ecran permettant de voir si les donnees du fichier probleme ont ete lues correctement
void StreamProbleme(TProblem unProb, std::ostream& output);
void AfficherProbleme(TProblem unProb);

//DESCRIPTION: Affichage d'une solution a l'ecran (avec ou sans d�tails des calculs)
void StreamSolution(const TSolution uneSol, TProblem unProb, std::string Titre, bool AvecCalcul, std::ostream& output);
void AfficherSolution(const TSolution uneSol, TProblem unProb, std::string Titre, bool AvecCalcul = false);

//DESCRIPTION:	Evaluation de la fonction objectif d'une solution et MAJ du compteur d'evaluations. La fonction objectif represente la somme des retards ponderes de la sequence.
//				NB: L'evaluation d'une solution inclue la validation de la solution (presence de chacune des taches (sans doublons) + longueur de la solution + domaine admissible des variables)
void EvaluerSolution(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION:	Creation d'une sequence aleatoire des N taches et appel a l'evaluation de la fonction objectif. Allocation dynamique de memoire pour le detail de la solution
void CreerSolutionAleatoire(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Affichage a l'ecran de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
void StreamResultats(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, bool AvecCalcul, std::ostream& output);
void AfficherResultats(const TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier (en append) de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
void AfficherResultatsFichier(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement pour les differentes structures en parametre
void LibererMemoireFinPgm(TSolution uneCourante, TSolution uneNext, TSolution uneBest, TProblem unProb);

#endif