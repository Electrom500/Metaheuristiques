#include "Entete.h"

#include <cmath>

// Membres de l'�quipe H01 :
// - Damien Santerne
// - Maxime Hardy
// - Niels Kristen

// Sur windows, on load les DLL. Sur linux, on importe ProcesseurUnique.h
#ifdef _WIN32
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
#include "ProcesseurUnique.h"
#endif

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************

//DESCRIPTION:	Creation d'une solution voisine a partir de la solution uneSol. Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//NB:uneSol ne doit pas etre modifiee (const)
TSolution GetSolutionVoisine(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION:	Application du type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB:uneSol ne doit pas etre modifiee (const)
TSolution	AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Echange de deux taches selectionnees aleatoirement.
void EchangeDeuxTaches(TSolution& Voisin, TProblem unProb, TAlgo& unAlgo);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char* Param[])
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

	// Initialisation de la "meilleure solution � date"
	Best = Courante;

	// Initialisation de la temp�rature courante
	LAlgo.TemperatureCourante = LAlgo.TemperatureInitiale;

	// Variable sur le nombre d'�valuations par paliers
	int NbEvalPalier = LAlgo.NB_EVAL_MAX / LAlgo.NbPalier;

	// Variable pour la valeur � atteindre avant de changer de palier de temp�rature
	int ProchainPalier = NbEvalPalier;

	do
	{
		do
		{
			Next = GetSolutionVoisine(Courante, LeProb, LAlgo);
			//AfficherSolution(Courante, LeProb, "COURANTE: ", false);
			//AfficherSolution(Next, LeProb, "NEXT: ", false);
			LAlgo.Delta = Next.FctObj - Courante.FctObj;
			if (LAlgo.Delta < 0)	//**deplacement amelioration (nous pourrions �galement considerer l'egalite)
			{
				Courante = Next;
				// cout << "CPT_EVAL: " << LAlgo.CptEval << "\t\tNEW COURANTE/OBJ: " << Courante.FctObj << endl;
				//AfficherSolution(Courante, LeProb, "NOUVELLE COURANTE: ", false);

				// S'il y a am�lioration de la fonction objectif, il y a une possibilit� que la nouvelle solution soit la meilleure
				// On doit tester si elle est meilleure ou non
				if (Courante.FctObj < Best.FctObj)
				{
					Best = Courante;
				}
			}
			else
			{
				// Calcul du seuil d'acceptation de la solution
				double seuil = std::exp(-LAlgo.Delta / LAlgo.TemperatureCourante);

				// On tire un nombre al�atoire r�el entre 0 et 1
				double nombre_alea = rand();
				nombre_alea /= RAND_MAX;

				// Si le nombre pig� est inf�rieur au seuil, il prend la nouvelle solution m�me si elle d�grade la fonction objectif
				// Si le nombre pig� est sup�rieur ou �gal au seuil, il garde la solution courante sans la modifier
				if (nombre_alea < seuil)
				{
					Courante = Next;
					// cout << "CPT_EVAL: " << LAlgo.CptEval << "\t\tNEW COURANTE/OBJ: " << Courante.FctObj << "\tSEUIL: " << seuil << endl;
				}
			}
		} while (LAlgo.CptEval < ProchainPalier);

		// Modification de la temp�rature courante
		LAlgo.TemperatureCourante *= LAlgo.Alpha;
		cout << "T_COURANTE: " << LAlgo.TemperatureCourante << endl;

		// Modification du prochain palier
		ProchainPalier += NbEvalPalier;

	} while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX && Courante.FctObj != 0); //Critere d'arret

	AfficherResultats(Best, LeProb, LAlgo);
	AfficherResultatsFichier(Best, LeProb, LAlgo, "Resultats.txt");

	LibererMemoireFinPgm(Courante, Next, Best, LeProb);

	//system("PAUSE");

	return 0;
}

TSolution GetSolutionVoisine(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo)
{
	//Type (structure) de voisinage : Insertion
	//Parcours dans le voisinage : Aléatoire (méthodes 0 et 1) ou Partielle orienté (méthode 2)
	//Regle de pivot : 	k-improve best (on choisit la meilleure parmi k solutions voisines)

	// Création du voisin
	TSolution unVoisin;
	TSolution meilleurVoisin = uneSol;

	// Itération sur k voisins
	for (int i = 0; i < unAlgo.TailleVoisinage; i++) {

		// Utiliser la fonction de voisinage par Insertion
		unVoisin = AppliquerVoisinage(uneSol, unProb, unAlgo);
		
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
	int AnciennePosition = rand() % unProb.N;
	int NouvellePosition = rand() % (unProb.N - 1);

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
