#include "Entete.h"
#pragma comment (lib,"RecuitDLL.lib")  
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (RecuitDLL.dll et RecuitDLL.lib) doivent se trouver dans le repertoire courant du projet pour une execution a l'aide du compilateur. 
//Indiquer les arguments du programme dans les proprietes du projet - debogage - arguments.
//Sinon, utiliser le repertoire execution.
//NB: le projet actuel doit etre compile dans le meme mode (DEBUG ou RELEASE) que les fichiers de DLL - par defaut en RELEASE

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem& unProb, TAlgo& unAlgo);

//DESCRIPTION: Affichage a l'ecran permettant de voir si les donnees du fichier probleme ont ete lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme(TProblem unProb);

//DESCRIPTION: Affichage d'une solution a l'ecran (avec ou sans détails des calculs)
extern "C" _declspec(dllimport) void AfficherSolution(const TSolution uneSol, TProblem unProb, std::string Titre, bool AvecCalcul = false);

//DESCRIPTION:	Evaluation de la fonction objectif d'une solution et MAJ du compteur d'evaluations. La fonction objectif represente la somme des retards ponderes de la sequence.
//				NB: L'evaluation d'une solution inclue la validation de la solution (presence de chacune des taches (sans doublons) + longueur de la solution + domaine admissible des variables)
extern "C" _declspec(dllimport) void EvaluerSolution(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION:	Creation d'une sequence aleatoire des N taches et appel a l'evaluation de la fonction objectif. Allocation dynamique de memoire pour le detail de la solution
extern "C" _declspec(dllimport) void CreerSolutionAleatoire(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Affichage a l'ecran de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
extern "C" _declspec(dllimport) void AfficherResultats(const TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier (en append) de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
extern "C" _declspec(dllimport) void AfficherResultatsFichier(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement pour les differentes structures en parametre
extern "C" _declspec(dllimport) void	LibererMemoireFinPgm(TSolution uneCourante, TSolution uneNext, TSolution uneBest, TProblem unProb);

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************

//DESCRIPTION:	Creation d'une solution voisine a partir de la solution uneSol. Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//NB:uneSol ne doit pas etre modifiee (const)
TSolution GetSolutionVoisine (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo);

//DESCRIPTION:	Application du type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB:uneSol ne doit pas etre modifiee (const)
TSolution	AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Echange de deux taches selectionnees aleatoirement.
void EchangeDeuxTaches(TSolution& Voisin, TProblem unProb, TAlgo& unAlgo);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char *Param[])
{
	TSolution Courante;		//Solution active au cours des iterations
	TSolution Next;			//Solution voisine retenue a une iteration
	TSolution Best;			//Meilleure solution depuis le debut de l'algorithme	//Non utilisee pour le moment 
	TProblem LeProb;		//Definition de l'instance de probleme
	TAlgo LAlgo;			//Definition des parametres de l'agorithme
	string NomFichier;
		
	//**Lecture des parametres
	NomFichier.assign(Param[1]);
	LAlgo.TailleVoisinage = atoi(Param[2]);
	LAlgo.TemperatureInitiale = atof(Param[3]);
	LAlgo.Alpha = atof(Param[4]);
	LAlgo.NbPalier = atoi(Param[5]);
	LAlgo.NB_EVAL_MAX = atoi(Param[6]);
	
	srand(GetTickCount()); //**Precise un germe pour le generateur aleatoire (horloge en millisecondes)

	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	//AfficherProbleme(LeProb);
	
	//**Creation de la solution initiale 
	CreerSolutionAleatoire(Courante, LeProb, LAlgo);
	AfficherSolution(Courante, LeProb, "SOLUTION INITIALE: ", false);
	//**Enregistrement qualite solution de depart
	LAlgo.FctObjSolDepart = Courante.FctObj;
	
	do
	{
		Next = GetSolutionVoisine(Courante, LeProb, LAlgo);
		//AfficherSolution(Courante, LeProb, "COURANTE: ", false);
		//AfficherSolution(Next, LeProb, "NEXT: ", false);
		LAlgo.Delta = Next.FctObj - Courante.FctObj;
		if (LAlgo.Delta < 0)	//**deplacement amelioration (nous pourrions également considerer l'egalite)
		{
			Courante = Next;
			cout << "CPT_EVAL: " << LAlgo.CptEval << "\t\tNEW COURANTE/OBJ: " << Courante.FctObj << endl;
			//AfficherSolution(Courante, LeProb, "NOUVELLE COURANTE: ", false);
		}
	}while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX && Courante.FctObj!=0); //Critere d'arret
	
	AfficherResultats(Courante, LeProb, LAlgo);
	AfficherResultatsFichier(Courante, LeProb, LAlgo,"Resultats.txt");
	
	LibererMemoireFinPgm(Courante, Next, Best, LeProb);
	
	//system("PAUSE");

	return 0;
}

//DESCRIPTION: Creation d'une solution voisine a partir de la solution courante (uneSol) qui ne doit pas etre modifiee.
//Dans cette fonction, appel de la fonction AppliquerVoisinage() pour obtenir une solution voisine selon un TYPE DE VOISINAGE selectionne + Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//Ainsi, si la RÈGLE DE PIVOT necessite l'etude de plusieurs voisins (TailleVoisinage>1), la fonction "AppliquerVoisinage()" sera appelee plusieurs fois.
TSolution GetSolutionVoisine (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo)
{
	//Type (structure) de voisinage : 	Echange - Echange de 2 taches
	//Parcours dans le voisinage : 		Aleatoire: Selection aleatoire des 2 taches 
	//Regle de pivot : 					k-ImproveBEST (k étant donné en paramètre pour l'exécution du pgm)

	//k-Improve-Best
	TSolution unVoisin, unGagnant;
	int i;
	
	unGagnant = AppliquerVoisinage(uneSol, unProb, unAlgo); //Premier voisin

	for (i = 2; i <= unAlgo.TailleVoisinage; i++) //Permet de generer plusieurs solutions voisines
	{
		unVoisin = AppliquerVoisinage(uneSol, unProb, unAlgo);
		if (unVoisin.FctObj < unGagnant.FctObj) //Conservation du meilleur
			unGagnant = unVoisin;
		else //Choix aléatoire si unVoisin et un Gagnant sont de même qualité
			if (unVoisin.FctObj == unGagnant.FctObj)
			{
				if (rand() % 2 == 0)
					unGagnant = unVoisin;
			}
	}
	return (unGagnant);
}

//DESCRIPTION: Fonction appliquant le type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB: La solution courante (uneSol) ne doit pas etre modifiee (const)
TSolution AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo)
{
	//Type (structure) de voisinage : 	Echange - Echange de 2 taches selectionnees aleatoirement	
	TSolution Copie;

	//Utilisation d'une nouvelle TSolution pour ne pas modifier La solution courante (uneSol)
	Copie = uneSol;

	//Transformation de la solution Copie selon le type (structure) de voisinage selectionne : Echange
	EchangeDeuxTaches(Copie, unProb, unAlgo);

	//Le nouveau voisin doit etre evalue et retourne
	EvaluerSolution(Copie, unProb, unAlgo);
	return(Copie);
}

//DESCRIPTION: Echange de deux taches selectionnees aleatoirement																		
//A modifier si vous le souhaitez 
void EchangeDeuxTaches(TSolution& Voisin, TProblem unProb, TAlgo& unAlgo)
{
	int PosA, PosB;

	//Tirage aleatoire des 2 taches
	PosA = rand() % unProb.N;
	do
	{
		PosB = rand() % unProb.N;
	} while (PosA == PosB);
	//Verification pour ne pas perdre une evaluation

	//Echange des 2 taches
	swap(Voisin.Seq[PosA], Voisin.Seq[PosB]); //Echange de 2 elements dans un vecteur (fonction dans la biblio functional)
}