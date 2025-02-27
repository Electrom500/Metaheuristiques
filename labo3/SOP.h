#ifndef __LIBRAIRIE_H_
#define __LIBRAIRIE_H_

#include "Entete.h"

//*****************************************************************************************
// Prototype/description des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION: Lecture du Fichier probleme et initialiation de la structure Problem //NB: Construction pour chaque ville de listes pr�d�cesseurs/successeurs
void LectureProbleme(std::string FileName, TProblem& unProb, TAlgo& unAlgo);

//DESCRIPTION: Affichage a l'ecran permettant de voir si les donnees du fichier (instance) probleme ont ete lues correctement
void StreamProbleme(TProblem unProb, std::ostream& output);
void AfficherProbleme(TProblem unProb);

//DESCRIPTION: Evaluation de la fonction objectif (distance totale) d'une solution + Validation de la solution + MAJ du compteur d'�valuation
//				Validation de la solution: verification des contraintes de preseance + la presence de chaque ville une et une seule fois. Si invalide: fonction objectif = INT_MAX
void EvaluerSolution(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Generation d'une population initiale de solutions aleatoires valides, incluant leur evaluation et validation
void CreerPopulationInitialeAleatoireValide(std::vector<TSolution>& unePop, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Creation d'une solution aleatoire valide (sequence aleatoire de villes en respectant preseances), incluant son evaluation et sa validation
void CreerSolutionAleatoire(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Affichage a l'ecran d'une solution : sequence, fonction objectif, etat de validite et l'effort necessaire pour l'obtenir
void StreamUneSolution(const TSolution uneSol, TProblem unProb, std::ostream& output);
void AfficherUneSolution(const TSolution uneSol, TProblem unProb);

//DESCRIPTION: Affichage des solutions de la population recue en parametre
void StreamPopulation(std::vector<TSolution> unePop, int Iter, TProblem unProb, std::ostream& output);
void AfficherPopulation(std::vector<TSolution> unePop, int Iter, TProblem unProb);

//DESCRIPTION: Tri croissant (probleme de minimisation) des individus dans la population entre Debut et Fin-1 INCLUSIVEMENT 
void TrierPopulation(std::vector<TSolution>& unePop, int Debut, int Fin);

//DESCRIPTION: Mutation (modification aleatoire) d'une solution. La solution mutuee est evaluee avant d'etre retournee.
//- Si solution valide en entr�e: une mutation valide est produite et retournee - swap adjacent
//- Si solution NON valide en entr�e : une mutation(sans assurer la validite) est produite et retournee - swap aleatoire
void Mutation(TSolution& Mutant, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Selection d'un individu par tournoi (taille 2)
int Selection(std::vector<TSolution> unePop, int _Taille, TProblem unProb);

//DESCRIPTION: Affichage a l'ecran des resultats de l'algorithme evolutionnaire: solution finale (avec etat de validite), nombre d'evaluations et parametres de l'algorithme
void StreamResultats(TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::ostream& output);
void AfficherResultats(TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier texte des resultats de l'algorithme evolutionnaire: solution finale (avec etat de validite), nombre d'evaluations et parametres de l'algorithme
void AfficherResultatsFichier(TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement
void LibererMemoireFinPgm(std::vector<TSolution>& unePop, std::vector<TSolution>& unePopEnfant, TSolution& uneBest, TProblem& unProb, TAlgo unAlgo);

// Pour remplacer la fonction Windows
unsigned int GetTickCount();

#endif