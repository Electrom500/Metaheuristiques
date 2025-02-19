#include "Entete.h"
#include <deque>
#include <utility>
#include <tuple>

#ifdef _WIN32
#pragma comment (lib,"TabouDLL.lib")  
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (TabouDLL.dll et TabouDLL.lib) doivent se trouver dans le repertoire courant du projet pour une execution a l'aide du compilateur. 
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
#else
#include "ProcesseurUnique.h"
#endif

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************

//DESCRIPTION:	Creation d'une solution voisine a partir de la solution uneSol. Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//NB:uneSol ne doit pas etre modifiee (const)
TSolution GetSolutionVoisine(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo, std::deque<std::pair<int, int>>& listeTabous, int critereAspiration);

//DESCRIPTION:	Application du type de voisinage selectionne. La fonction retourne la solution voisine obtenue suite a l'application du type de voisinage.
//NB:uneSol ne doit pas etre modifiee (const)
std::pair<TSolution, std::pair<int, int>>	AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Vérifie que les indices selectionnés ne sont pas dans la liste des tabous ou respecte le critère d'aspiration 
bool EstAdmissible(const std::pair<int, int >  Tabou, const TSolution SolutionVoisine, const std::deque<std::pair<int, int>>& listeTabous,const int critereAspiration);

// Ajouter les résultats dans un fichier CSV pour une meilleure facilité d'analyse avec un programme externe
void AjouterResultatsFichierCSV(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

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
	string FichierSortie;
		
	//**Lecture des parametres
	NomFichier.assign(Param[1]);
	LAlgo.TailleVoisinage = atoi(Param[2]);
	LAlgo.LngListeTabous = atoi(Param[3]);
	LAlgo.NB_EVAL_MAX = atoi(Param[4]);

	if (NbParam > 5) {
		FichierSortie.assign(Param[5]);
	}
	else {
		FichierSortie = "resultats.csv";
	}

	srand(GetTickCount()); //**Precise un germe pour le generateur aleatoire (horloge en millisecondes)
	
	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	//AfficherProbleme(LeProb);

	//**Creation de la solution initiale 
	CreerSolutionAleatoire(Courante, LeProb, LAlgo);
	//AfficherSolution(Courante, LeProb, "SOLUTION INITIALE: ", false);
	//**Enregistrement qualite solution de depart
	LAlgo.FctObjSolDepart = Courante.FctObj;

	// Initialisation de la "meilleure solution � date"
	Best = Courante;

	// ListeTabous contiendra des paires des indices des taches précédement déplacés et leurs positions avant déplacement
	std::deque<std::pair<int, int>> listeTabous;


	for (int i = 0;i < LAlgo.LngListeTabous;i++) {
		// Initialisation avec des tabous invalides pour avoir une taille fixe plus tard (sans devoir vérifier la taille à chaque fois) 
		listeTabous.push_back(std::make_pair(-1, -1));
	}

	do
	{
		Next = GetSolutionVoisine(Courante, LeProb, LAlgo,listeTabous, Best.FctObj);
		//AfficherSolution(Courante, LeProb, "COURANTE: ", false);
		//AfficherSolution(Next, LeProb, "NEXT: ", false);
		if (Next.FctObj < Best.FctObj)	//**deplacement amelioration (nous pourrions également considerer l'egalite) 
		{
			Best = Next;
			//AfficherSolution(Courante, LeProb, "NOUVELLE COURANTE: ", false);
		}
		// Dans tout les cas on selectionne le meilleur voisin admissible comme prochaine courante
		Courante = Next;
		//cout << "CPT_EVAL: " << LAlgo.CptEval << "\t\tNEW COURANTE/OBJ: " << Next.FctObj << endl;
	} while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX && Courante.FctObj != 0); //Critere d'arret

	//AfficherResultats(Best, LeProb, LAlgo);
	//AfficherResultatsFichier(Best, LeProb, LAlgo, "Resultats.txt");
	AjouterResultatsFichierCSV(Best, LeProb, LAlgo, FichierSortie);

	LibererMemoireFinPgm(Courante, Next, Best, LeProb);

	//system("PAUSE");

	return 0;
}

//DESCRIPTION: Creation d'une solution voisine a partir de la solution courante (uneSol) qui ne doit pas etre modifiee.
//Dans cette fonction, appel de la fonction AppliquerVoisinage() pour obtenir une solution voisine selon un TYPE DE VOISINAGE selectionne + Definition la STRATEGIE D'ORIENTATION (Parcours/Regle de pivot).
//Ainsi, si la RÈGLE DE PIVOT necessite l'etude de plusieurs voisins (TailleVoisinage>1), la fonction "AppliquerVoisinage()" sera appelee plusieurs fois.
TSolution GetSolutionVoisine(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo, std::deque<std::pair<int, int>>& listeTabous, int critereAspiration)
{
	//Type (structure) de voisinage : 	Echange - Echange de 2 unités
	//Parcours dans le voisinage : 		Aleatoire: Selection aleatoire des 2 unités 
	//Regle de pivot : 					k-ImproveBEST (k étant donné en paramètre pour l'exécution du pgm)

	//k-Improve-Best
	TSolution unVoisin, unGagnant;
	int i;
	std::pair<int, int> tabouVoisin, tabouGagnant;
	
	std::tie(unGagnant, tabouGagnant) = AppliquerVoisinage(uneSol, unProb, unAlgo); //Premier voisin et Premier tabou

	bool GagnantAdmissible = EstAdmissible(tabouGagnant, unGagnant, listeTabous, critereAspiration);

	for (i = 2; i <= unAlgo.TailleVoisinage; i++) //Permet de generer plusieurs solutions voisines
	{

		std::tie(unVoisin, tabouVoisin) = AppliquerVoisinage(uneSol, unProb, unAlgo);

		bool VoisinAdmissible = EstAdmissible(tabouVoisin, unVoisin, listeTabous, critereAspiration);
		if (VoisinAdmissible) {
			if ((!GagnantAdmissible) || (unVoisin.FctObj < unGagnant.FctObj)) {
				unGagnant = unVoisin;
				GagnantAdmissible = VoisinAdmissible;
				tabouGagnant = tabouVoisin;
			}
		}
	}
	// Mise a jour de la liste des tabous
	listeTabous.push_back(tabouGagnant);
	listeTabous.pop_front();
	return (unGagnant);
}

std::pair<TSolution,std::pair<int,int>> AppliquerVoisinage(const TSolution uneSol, TProblem unProb, TAlgo& unAlgo)
{
	//Type (structure) de voisinage : 	A DETERMINER...	
	TSolution Copie;
	std::pair<int, int> Tabou;
	//Utilisation d'une nouvelle TSolution pour ne pas modifier La solution courante (uneSol)
	Copie = uneSol;

	//Transformation de la solution Copie selon le type (structure) de voisinage selectionne : echange, insertion, 2-opt, etc.
	//Ici la solution Copie demeure identique a la solution uneSol
	//.......... A COMPLETER  ou APPEL a une fonction que vous pouvez creer


	// Piger une tâche parmis N à enlever
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
	//Tabou = std::make_pair(ValeurElement, AnciennePosition);
	Tabou = std::make_pair(NouvellePosition, AnciennePosition);

	//Le nouveau voisin doit etre evalue et retourne
	EvaluerSolution(Copie, unProb, unAlgo);
	return(std::make_pair(Copie,Tabou));
}

bool EstAdmissible(const std::pair<int, int >  Tabou, const TSolution SolutionVoisine, const std::deque<std::pair<int, int>>& listeTabous, const int critereAspiration) {
	for (int i = 0;i < listeTabous.size();i++) {
		//if ((Tabou.first == listeTabous[i].first) && (std::abs(Tabou.second - listeTabous[i].second)<4 )) {
		if (Tabou == listeTabous[i]){
			if (SolutionVoisine.FctObj < critereAspiration) {
				//std::cout << "-----------------CRITERE D ASPIRATION-------------------------" << std::endl;
				return true;
			}
			return false;
		}
	} 
	return true;
}

// Cette fonction crée un fichier CSV ou ajoute à ce fichier les informations de la solution finale trouvée
// Ces informations sont formatées en CSV de façon à faciliter l'analyse par un programme exerne
void AjouterResultatsFichierCSV(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName) {
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
		FileStream << "Nom,N,NbVoisins,LngListeTabous,TotalEval,MaxEval,FctObjDepart,FctObjFinale,Etat,EvalPourTrouver,Seq" << std::endl;
	}

	// Ajouter les informations sur une seule ligne
	FileStream << unProb.Nom << ",";
	FileStream << unProb.N << ",";
	FileStream << unAlgo.TailleVoisinage << ",";
	FileStream << unAlgo.LngListeTabous << ",";
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