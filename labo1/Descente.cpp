#include "Entete.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#pragma comment (lib,"DescenteDLL.lib")  
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (DescenteDLL.dll et DescenteDLL.lib) doivent se trouver dans le repertoire courant du projet pour une execution a l'aide du compilateur. 
//Indiquer les arguments du programme dans les proprietes du projet - debogage - arguments.
//Sinon, utiliser le repertoire execution.
//NB: le projet actuel doit etre compile dans le meme mode (DEBUG ou RELEASE) que les fichiers de DLL - par defaut en RELEASE

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem& unProb, TAlgo& unAlgo);

//DESCRIPTION: Fonction d'affichage a l'ecran permettant de voir si les donnees du fichier probleme ont ete lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme(TProblem unProb);

//DESCRIPTION: Affichage d'une solution a l'ecran (avec ou sans d�tails des calculs)
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
#else
// Nous avons fait du "reverse engineering" pour recréer les différentes fonctions des
// DLL afin de faire fonctionner le programme sur Linux aussi.
#include "ProcesseurUnique.h"
#endif

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************

//DESCRIPTION:	Creation d'une solution voisine a partir de la solution uneSol. Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//NB:uneSol ne doit pas etre modifiee (const)
//La fonction a été modifiée pour prendre en compte la méthode de voisinage choisie
TSolution GetSolutionVoisine (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo, int MethodeVoisinage);

//DESCRIPTION:	Application du type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB:uneSol ne doit pas etre modifiee (const)
TSolution AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

TSolution AppliquerVoisinagePermutation(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

TSolution AppliquerVoisinageOriente(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

// Générateur d'entier aléatoire uniforme
unsigned GenererEntierUniforme(unsigned range);

// Ajouter les résultats dans un fichier CSV pour une meilleure facilité d'analyse avec un programme externe
void AjouterResultatsFichierCSV(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName, int MethodeVoisinage=0);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char *Param[])
{
	TSolution Courante;		//Solution active au cours des iterations
	TSolution Next;			//Solution voisine retenue a une iteration
	TSolution Best;			//Meilleure solution depuis le d�but de l'algorithme	//Non utilisee pour le moment 
	TProblem LeProb;		//Definition de l'instance de probleme
	TAlgo LAlgo;			//Definition des parametres de l'agorithme
	string NomFichier;

	// Arguments optionnels du programme
	int MethodeVoisinage = 0;
	int NombreIterations = 1;
	string FichierSortie = "ResultatsTab.csv";
		
	//**Lecture des parametres
	NomFichier.assign(Param[1]);
	LAlgo.TailleVoisinage = atoi(Param[2]);
	LAlgo.NB_EVAL_MAX= atoi(Param[3]);

	// Méthode de voisinage
	if (NbParam > 4) {
		MethodeVoisinage = atoi(Param[4]);
	}

	// Nombre d'itérations
	if (NbParam > 5) {
		NombreIterations = atoi(Param[5]);
	}

	// Nom du fichier de sortie
	if (NbParam > 6) {
		FichierSortie = Param[6];
	}

	// Precise un germe pour le generateur aleatoire
	srand((unsigned) time(NULL));
	
	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	//AfficherProbleme(LeProb);
	
	// Itération sur le nombre de solutions souhaitées (1 solution par défaut)
	for (int i = 0; i < NombreIterations; i++) {
		//**Creation de la solution initiale 
		CreerSolutionAleatoire(Courante, LeProb, LAlgo);
		LAlgo.CptEval = 0;
		// AfficherSolution(Courante, LeProb, "SOLUTION INITIALE: ", false);

		//**Enregistrement des infos concernant la meilleure solution
		LAlgo.FctObjSolDepart = Courante.FctObj;

		do
		{
			Next = GetSolutionVoisine(Courante, LeProb, LAlgo, MethodeVoisinage);
			//AfficherSolution(Courante, LeProb, "COURANTE: ", false);
			//AfficherSolution(Next, LeProb, "NEXT: ", false);
			if (Next.FctObj < Courante.FctObj)	//**amelioration
			{
				//cout << "CPT_EVAL: " << LAlgo.CptEval << "\t\tNEW COURANTE/OBJ: " << Next.FctObj << endl;
				//AfficherSolution(Next, LeProb, "NOUVELLE COURANTE: ", false);
				//Modification de la solution courante
				Courante = Next;
			}
		}while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX && Courante.FctObj!=0); //Critere d'arret (ne pas enlever/modifier)
		
		//AfficherResultats(Courante, LeProb, LAlgo);
		//AfficherResultatsFichier(Courante, LeProb, LAlgo,"Resultats.txt");
		AjouterResultatsFichierCSV(Courante, LeProb, LAlgo, FichierSortie, MethodeVoisinage);
		
		LibererMemoireFinPgm(Courante, Next, Best, LeProb);
	}
	
	//system("PAUSE");

	return 0;
}

//DESCRIPTION: Creation d'une solution voisine a partir de la solution courante (uneSol) qui ne doit pas etre modifiee.
//Dans cette fonction, appel de la fonction AppliquerVoisinage() pour obtenir une solution voisine selon un TYPE DE VOISINAGE selectionne + Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//Ainsi, si la ReGLE DE PIVOT necessite l'etude de plusieurs voisins (TailleVoisinage>1), la fonction "AppliquerVoisinage()" sera appelee plusieurs fois.
TSolution GetSolutionVoisine (const TSolution uneSol, TProblem unProb, TAlgo &unAlgo, int MethodeVoisinage)
{
	//Type (structure) de voisinage : Soit Insertion (méthodes 0 et 2), Permutation (méthode 1)
	//Parcours dans le voisinage : Aléatoire (méthodes 0 et 1) ou Partielle orienté (méthode 2)
	//Regle de pivot : 	k-improve best (on choisit la meilleure parmi k solutions voisines)

	// Création du voisin
	TSolution unVoisin;
	TSolution meilleurVoisin = uneSol;

	// Itération sur k voisins
	for (int i = 0; i < unAlgo.TailleVoisinage; i++) {
		// Utiliser la bonne fonction selon la méthode de voisinage choisie
		if (MethodeVoisinage == 0) {
			// Insertion aléatoire
			unVoisin = AppliquerVoisinage(uneSol, unProb, unAlgo);
		}
		else if (MethodeVoisinage == 1 ){
			// Permutation aléatoire
			unVoisin = AppliquerVoisinagePermutation(uneSol, unProb, unAlgo);
		}
		else {
			// Insertion orientée
			unVoisin = AppliquerVoisinageOriente(uneSol, unProb, unAlgo);
		}

		// Si le voisin est meilleur que le meilleur voisin jusqu'à présent, on remplace le meilleur voisin par le nouveau voisin
		if (unVoisin.FctObj < meilleurVoisin.FctObj) {
			meilleurVoisin = unVoisin;
		}
	}

	// On renvoie le meilleur voisin parmi les k voisins testés
	return meilleurVoisin;
}


//DESCRIPTION: Fonction appliquant le type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB: La solution courante (uneSol) ne doit pas etre modifiee (const)
TSolution AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo)
{
	// Type (structure) de voisinage : Insertion
	// On enlève un élément de la séquence et on l'insère ailleurs dans la séquence
	TSolution Copie;

	// Utilisation d'une nouvelle TSolution pour ne pas modifier La solution courante (uneSol)
	Copie = uneSol;

	// Transformation de la solution Copie selon le type (structure) de voisinage selectionne : insertion
	// Ici la solution Copie demeure identique a la solution uneSol

	// Piger une tâche parmis N à enlever et piger une nouvelle position parmis N-1 positions
	int AnciennePosition = GenererEntierUniforme(unProb.N);
	int NouvellePosition = GenererEntierUniforme(unProb.N-1);

	// On enlève l'élément à l'ancienne position
	int ValeurElement = Copie.Seq[AnciennePosition];
	Copie.Seq.erase(Copie.Seq.begin() + AnciennePosition);

	// Cas rare: Plutôt que d'insérer à la même place, on le met à la fin
	if (AnciennePosition == NouvellePosition) {
		// On ajoute notre élément à la fin du vecteur
		Copie.Seq.push_back(ValeurElement);
	}
	// Cas normal: On remet l'élément à la nouvelle position
	// en décalant le sous-ensemble du vecteur.
	else {
		// On insère notre élément à la nouvelle position
		Copie.Seq.insert(Copie.Seq.begin() + NouvellePosition, ValeurElement);
	}

	// Le nouveau voisin doit etre evalue et retourne
	EvaluerSolution(Copie, unProb, unAlgo);
	return(Copie);
}

TSolution AppliquerVoisinagePermutation(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo) {
	// Type (structure) de voisinage : Permutation
	TSolution Copie;

	// On permute deux éléments de la séquence
	Copie = uneSol;

	// Piger une tâche parmis N à enlever
	int AnciennePosition = GenererEntierUniforme(unProb.N);
	int NouvellePosition = GenererEntierUniforme(unProb.N-1);

	// On copie la valeur temporaire
	int ValeurElement = Copie.Seq[AnciennePosition];

	// Cas rare: Plutôt que d'échanger avec la même place, on échange avec le dernier
	if (AnciennePosition == NouvellePosition) {
		NouvellePosition = unProb.N-1;
	}

	// On swap notre élément avec la nouvelle position
	Copie.Seq[AnciennePosition] = Copie.Seq[NouvellePosition];
	Copie.Seq[NouvellePosition] = ValeurElement;

	//Le nouveau voisin doit etre evalue et retourne
	EvaluerSolution(Copie, unProb, unAlgo);
	return(Copie);
}

TSolution AppliquerVoisinageOriente(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo)
{
	// Type (structure) de voisinage : Insertion, partiellement orienté
	// On enlève un élément de la séquence et on l'insère ailleurs dans la séquence
	// On choisit l'élément à enlever uniquement parmi les éléments qui ont un retard
	TSolution Copie;

	// Utilisation d'une nouvelle TSolution pour ne pas modifier La solution courante (uneSol)
	Copie = uneSol;

	// Crée un vecteur qui contient les indices des éléments qui sont en retard
	std::vector<int> indicesRetard;

	for (int i = 0; i < unProb.N; i++) {
		if (Copie.Retard[i] > 0) {
			indicesRetard.push_back(i);
		}
	}

	// Piger un élément parmis ceux en retard
	int AnciennePosition = indicesRetard[GenererEntierUniforme(indicesRetard.size())];

	// Piger une position aléatoire ou on met l'élément
	int NouvellePosition = GenererEntierUniforme(unProb.N-1);

	// On enlève l'élément à l'ancienne position
	int ValeurElement = Copie.Seq[AnciennePosition];
	Copie.Seq.erase(Copie.Seq.begin() + AnciennePosition);

	// Cas rare: Plutôt que d'insérer à la même place, on le met à la fin
	if (AnciennePosition == NouvellePosition) {
		// On ajoute notre élément à la fin du vecteur
		Copie.Seq.push_back(ValeurElement);
	}
	// Cas normal: On remet l'élément à la nouvelle position
	// en décalant le sous-ensemble du vecteur.
	else {
		// On insère notre élément à la nouvelle position
		Copie.Seq.insert(Copie.Seq.begin() + NouvellePosition, ValeurElement);
	}

	//Le nouveau voisin doit etre evalue et retourne
	EvaluerSolution(Copie, unProb, unAlgo);
	return(Copie);
}


// Cette fonction crée un fichier CSV ou ajoute à ce fichier les informations de la solution finale trouvée
// Ces informations sont formatées en CSV de façon à faciliter l'analyse par un programme exerne
void AjouterResultatsFichierCSV(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName, int MethodeVoisinage) {
    // Vérifier si le fichier existe
	std::ifstream ReadFileStream;
	ReadFileStream.open(FileName);
	bool FileExists = ReadFileStream.good();
	ReadFileStream.close();
	
	// Ouvrir le fichier en mode "append"
    std::ofstream FileStream;
    FileStream.open(FileName, std::ios::app);

    // Si c'est la première fois qu'on ouvre le fichier, on écrit aussi l'entête
    if (!FileExists) {
        FileStream << "Nom,N,NbVoisins,MethodeVoisinage,TotalEval,MaxEval,FctObjDepart,FctObjFinale,Etat,EvalPourTrouver,Seq" << std::endl;
    }

    // Ajouter les informations sur une seule ligne
    FileStream << unProb.Nom << ",";
    FileStream << unProb.N << ",";
    FileStream << unAlgo.TailleVoisinage << ",";
    FileStream << MethodeVoisinage << ",";
    FileStream << unAlgo.CptEval << ",";
    FileStream << unAlgo.NB_EVAL_MAX << ",";
    FileStream << unAlgo.FctObjSolDepart << ",";
    FileStream << uneSol.FctObj << ",";
    FileStream << uneSol.Valide << ",";
    FileStream << uneSol.NbEvaltoGet << ",";

    for (int i = 0; i < unProb.N - 1; i++) {
        FileStream << uneSol.Seq[i] << "-";
    }
    FileStream << uneSol.Seq[unProb.N - 1] << std::endl;

    // Fermer le fichier
    FileStream.close();
}

// Méthode pour générer des nombres aléatoires entiers entre 0 et range-1
// Cette méthode est légèrement biaisée, car le range initial parmi lequel le nombre
// aléatoire initial est généré n'est pas nécessairement divisible par la variable "range"
// Pour avoir un générateur de nombres aléatoires no biaisé, il faudrait utiliser :
// - Un générateur moderne (ex: std::mt19937)
// - Une distribution d'entiers uniformes (std::uniform_int_distribution)

// Il existe aussi des fonctions permettant de contourner le biais, mais c'est en dehors
// du but de ce cours.
unsigned GenererEntierUniforme(unsigned range) {
	unsigned x;
	unsigned r;

	x = rand();
	r = x % range;

	return r;
}