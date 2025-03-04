#include "Entete.h"

#ifdef _WIN32
#pragma comment (lib,"GeneticDLL.lib")
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (GeneticDLL.dll et GeneticDLL.lib) doivent se trouver dans le r�pertoire courant du projet pour une ex�cution � l'aide du compilateur. 
//Indiquer les arguments du programme dans les propri�t�s du projet - d�bogage - arguments.
//Sinon, utiliser le r�pertoire execution.
//NB: le projet actuel doit etre compile dans le meme mode (DEBUG ou RELEASE) que les fichiers de DLL - par defaut en RELEASE

//*****************************************************************************************
// Prototype/description des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION: Lecture du Fichier probleme et initialiation de la structure Problem //NB: Construction pour chaque ville de listes pr�d�cesseurs/successeurs
extern "C" _declspec(dllimport) void LectureProbleme(std::string FileName, TProblem& unProb, TAlgo& unAlgo);

//DESCRIPTION: Affichage a l'ecran permettant de voir si les donnees du fichier (instance) probleme ont ete lues correctement
extern "C" _declspec(dllimport) void AfficherProbleme(TProblem unProb);

//DESCRIPTION: Evaluation de la fonction objectif (distance totale) d'une solution + Validation de la solution + MAJ du compteur d'�valuation
//				Validation de la solution: verification des contraintes de preseance + la presence de chaque ville une et une seule fois. Si invalide: fonction objectif = INT_MAX
extern "C" _declspec(dllimport) void EvaluerSolution(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Generation d'une population initiale de solutions aleatoires valides, incluant leur evaluation et validation
extern "C" _declspec(dllimport) void CreerPopulationInitialeAleatoireValide(std::vector<TSolution>& unePop, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Creation d'une solution aleatoire valide (sequence aleatoire de villes en respectant preseances), incluant son evaluation et sa validation
extern "C" _declspec(dllimport) void CreerSolutionAleatoire(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Affichage a l'ecran d'une solution : sequence, fonction objectif, etat de validite et l'effort necessaire pour l'obtenir
extern "C" _declspec(dllimport) void AfficherUneSolution(const TSolution uneSol, TProblem unProb);

//DESCRIPTION: Affichage des solutions de la population recue en parametre
extern "C" _declspec(dllimport) void AfficherPopulation(std::vector<TSolution> unePop, int Iter, TProblem unProb);

//DESCRIPTION: Tri croissant (probleme de minimisation) des individus dans la population entre Debut et Fin-1 INCLUSIVEMENT 
extern "C" _declspec(dllimport) void TrierPopulation(std::vector<TSolution>& unePop, int Debut, int Fin);

//DESCRIPTION: Mutation (modification aleatoire) d'une solution. La solution mutuee est evaluee avant d'etre retournee.
//- Si solution valide en entr�e: une mutation valide est produite et retournee - swap adjacent
//- Si solution NON valide en entr�e : une mutation(sans assurer la validite) est produite et retournee - swap aleatoire
extern "C" _declspec(dllimport) void Mutation(TSolution& Mutant, TProblem unProb, TAlgo& unAlgo);

//DESCRIPTION: Selection d'un individu par tournoi (taille 2)
extern "C" _declspec(dllimport) int Selection(std::vector<TSolution> unePop, int _Taille, TProblem unProb);

//DESCRIPTION: Affichage a l'ecran des resultats de l'algorithme evolutionnaire: solution finale (avec etat de validite), nombre d'evaluations et parametres de l'algorithme
extern "C" _declspec(dllimport) void AfficherResultats(TSolution uneSol, TProblem unProb, TAlgo unAlgo);

//DESCRIPTION: Affichage dans un fichier texte des resultats de l'algorithme evolutionnaire: solution finale (avec etat de validite), nombre d'evaluations et parametres de l'algorithme
extern "C" _declspec(dllimport) void AfficherResultatsFichier(TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName);

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement
extern "C" _declspec(dllimport) void LibererMemoireFinPgm(std::vector<TSolution>& unePop, std::vector<TSolution>& unePopEnfant, TSolution& uneBest, TProblem& unProb, TAlgo unAlgo);
#else
#include "SOP.h"
#endif

//*****************************************************************************************
// Prototype des fonctions locales 
//*****************************************************************************************
TSolution Croisement(TSolution Parent1, TSolution Parent2, TProblem unProb, TAlgo &unAlgo);
int Remplacement(std::vector<TSolution> &Parents, std::vector<TSolution> Enfants, TProblem unProb, TAlgo unAlgo);

int CalculSolutionsOriginales(vector<TSolution>& pop);

// Ajouter les résultats dans un fichier CSV pour une meilleure facilité d'analyse avec un programme externe
void AjouterResultatsFichierCSV(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName, int NombreSolutionsUniques);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char* Param[])
{
	TProblem LeProb;					//**Definition de l'instance de probleme
	TAlgo LAlgo;						//**Definition des parametres de l'algorithme
	std::vector<TSolution> Pop;			//**Population composee de TaillePop solutions 
	std::vector<TSolution> PopEnfant;	//**Population d'enfant composee de (TaillePopEnfant = TaillePop * ProbCr) solutions
	TSolution Best;						//**Meilleure solution depuis le debut de l'algorithme

	int Pere, Mere;						//**Indices de solution des parents
	int i;
	double Alea;
	string NomFichier;
	string FichierSortie;
	bool Verbose;

	//**Lecture des param�tres
	NomFichier.assign(Param[1]);
	LAlgo.TaillePop			= atoi(Param[2]);
	LAlgo.ProbCr			= atof(Param[3]);
	LAlgo.ProbMut			= atof(Param[4]);
	LAlgo.NB_EVAL_MAX		= atoi(Param[5]);
	LAlgo.TaillePopEnfant	= (int)ceil(LAlgo.TaillePop * LAlgo.ProbCr);
	LAlgo.Gen = 0;

	if (NbParam > 6) {
		FichierSortie.assign(Param[6]);
	}
	else {
		FichierSortie = "resultats.csv";
	}

	if (NbParam > 7) {
		Verbose = atoi(Param[7]);
	}
	else {
		Verbose = true;
	}

	srand(GetTickCount()); //**Precise un germe pour le generateur aleatoire (horloge en millisecondes)

	//**D�finition de la dimension des tableaux
	Pop.resize(LAlgo.TaillePop);				//**Le tableau utilise les indices de 0 � TaillePop-1.
	PopEnfant.resize(LAlgo.TaillePopEnfant);	//**Le tableau utilise les indices de 0 � TaillePopEnfant-1

	//**Lecture du fichier de donnees
	LectureProbleme(NomFichier, LeProb, LAlgo);
	//AfficherProbleme(LeProb);

	//**Initialisation de la population initiale NB: Initialisation de la population entraine des evaluation de solutions.
	//**CptEval est donc = TaillePop au retour de la fonction.
	CreerPopulationInitialeAleatoireValide(Pop, LeProb, LAlgo);
	//AfficherPopulation(Pop, LAlgo.Gen, LeProb);

	//**Tri de la population
	TrierPopulation(Pop, 0, LAlgo.TaillePop);
	//AfficherPopulation(Pop, LAlgo.Gen, LeProb);
	
	//**Initialisation de de la meilleure solution au debut de l'algorithme
	Best = Pop[0];
	LAlgo.FctObjSolDepart = Pop[0].FctObj;
	int unique = CalculSolutionsOriginales(Pop);

	if (Verbose) {
		std::cout << "(Pop Ini) Best OBJ: " << Best.FctObj << "    Nb uniques: " << unique << std::endl;
		(!Best.Valide) ? std::cout << "***NON Valide\n" : std::cout << endl;
	}

	//**Boucle principale de l'algorithme evolutionnaire
	do 
	{
		LAlgo.Gen++;
		//**Selection, croisement et mutation
		for (i=0; i<LAlgo.TaillePopEnfant; i++)
		{
			//**SELECTION de deux parents (tournoi de taille = 2)
			Pere = Selection(Pop, LAlgo.TaillePop, LeProb);
			do
			{
				Mere = Selection(Pop, LAlgo.TaillePop, LeProb);
			} while (Mere == Pere); //Validation: Pere et Mere ne doivent pas avoir le m�me # de solution
						
			//**CROISEMENT entre les deux parents. Cr�ation de 1 enfant.
			PopEnfant[i] = Croisement(Pop[Pere], Pop[Mere], LeProb, LAlgo);

			//V�rification pour ne pas perdre une meilleure solution connue: si mutation OU remplacement non elistite
			if (PopEnfant[i].Valide && (PopEnfant[i].FctObj < Best.FctObj))	Best = PopEnfant[i];

			//AfficherUneSolution(Pop[Pere], LeProb);
			//AfficherUneSolution(Pop[Mere], LeProb);
			//AfficherUneSolution(PopEnfant[i], LeProb);

			//**MUTATION d'une solution
			Alea = double(rand()) / double(RAND_MAX);
			if (Alea < LAlgo.ProbMut)
			{
				Mutation(PopEnfant[i], LeProb, LAlgo);
			}
		}
		//AfficherPopulation(Pop, LAlgo.Gen, LeProb);
		//AfficherPopulation(PopEnfant, LAlgo.Gen, LeProb);
		
		//**REMPLACEMENT de la population pour la prochaine g�n�ration
		unique = Remplacement(Pop, PopEnfant, LeProb, LAlgo);
		TrierPopulation(Pop, 0, LAlgo.TaillePop);
		
		//**Conservation de la meilleure solution
		if (Pop[0].Valide && (Pop[0].FctObj < Best.FctObj)) Best = Pop[0];
		
		if (Verbose) {
			std::cout << "(Gen#" << setw(3) << LAlgo.Gen << ") Best OBJ: " << Best.FctObj << "    Nb uniques: " << unique << std::endl;
			(!Best.Valide) ? std::cout << "***NON Valide\n": std::cout << endl;
		}
	}while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX);			//**NE PAS ENLEVER

	if (Verbose) {
		AfficherPopulation(Pop, LAlgo.Gen, LeProb);
		AfficherResultats (Best, LeProb, LAlgo);			//**NE PAS ENLEVER
	}

	AfficherResultatsFichier (Best, LeProb, LAlgo, "Resultats.txt");
	AjouterResultatsFichierCSV(Best, LeProb, LAlgo, FichierSortie, unique);
	
	LibererMemoireFinPgm(Pop, PopEnfant, Best, LeProb, LAlgo);
	//system("PAUSE");
	
	return 0;
}

//***************************************************************************************************************
//**Fonction qui r�alise le CROISEMENT (�change de genes) entre deux parents. Retourne l'enfant produit et evalue.
//***************************************************************************************************************
//**A D�FINIR PAR L'�TUDIANT*************************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAM�TRES AINSI QUE LE RETOUR DE LA FONCTION
//*************************************************************************************************************** 
TSolution Croisement(TSolution Parent1, TSolution Parent2, TProblem unProb, TAlgo &unAlgo)
{
	//**INDICE: Le sous-programme rand() g�n�re al�atoirement un nombre entier entre 0 et RAND_MAX inclusivement.
	//**Pour tirer un nombre al�atoire entier entre 0 et MAX-1 inclusivement, il suffit d'utiliser l'instruction suivante : NombreAleatoire = rand() % MAX;
	
	TSolution Enfant;
	Enfant.Seq.reserve(unProb.NbVilles);

	// Pige du point de coupure, n'importe ou entre la 2e ville et l'avant-dernière
	int coupure = 2 + (rand() % (unProb.NbVilles - 4));

	// On copie la première sous-séquence du parent 1
	for (int i = 0; i < coupure; i++) {
		Enfant.Seq.push_back(Parent1.Seq[i]);
	}

	// On itère sur le parent 2 et on ajoute les villes qui ne sont pas dans la
	// sous-séquence, dans l'ordre d'apparition pour le parent 2.
	for (int i = 0; i < unProb.NbVilles; i++) {
		// On cherche si la ville 'i' est dans la sous-séquence du parent 1
		auto pos = std::find(Parent1.Seq.begin(), Parent1.Seq.begin() + coupure, Parent2.Seq[i]);

		// Si l'élément du parent 2 n'est pas dans la sous-séquence, on l'ajoute à l'enfant
		if (pos == Parent1.Seq.begin() + coupure) {
			Enfant.Seq.push_back(Parent2.Seq[i]);
		}
	}

	//**NE PAS ENLEVER
	EvaluerSolution(Enfant, unProb, unAlgo);
	//AfficherUneSolution(Enfant, unProb);
	return (Enfant);
}

/*
TSolution Croisement(TSolution Parent1, TSolution Parent2, TProblem unProb, TAlgo &unAlgo)
{
	//**INDICE: Le sous-programme rand() g�n�re al�atoirement un nombre entier entre 0 et RAND_MAX inclusivement.
	//**Pour tirer un nombre al�atoire entier entre 0 et MAX-1 inclusivement, il suffit d'utiliser l'instruction suivante : NombreAleatoire = rand() % MAX;
	
	TSolution Enfant;
	Enfant.Seq.reserve(unProb.NbVilles);

	// Pige de deux points de coupure qui ne peuvent pas être à la même place
	int coupure1, coupure2;

	do {
		coupure1 = 2 + (rand() % (unProb.NbVilles - 4));
		coupure2 = 2 + (rand() % (unProb.NbVilles - 4));
	} while (coupure1 == coupure2);

	if (coupure1 > coupure2) {
		int tmp_coupure = coupure2;
		coupure2 = coupure1;
		coupure1 = tmp_coupure;
	}

	// On copie la première sous-séquence du parent 1
	for (int i = 0; i < coupure1; i++) {
		Enfant.Seq.push_back(Parent1.Seq[i]);
	}

	// On itère sur le parent 2 et on ajoute les villes qui ne sont pas dans la
	// sous-séquence, dans l'ordre d'apparition pour le parent 2.
	for (int i = 0; i < coupure2; i++) {
		// On cherche si la ville 'i' est dans la sous-séquence du parent 1
		auto pos = std::find(Parent1.Seq.begin(), Parent1.Seq.begin() + coupure1, Parent2.Seq[i]);

		// Si l'élément du parent 2 n'est pas dans la sous-séquence, on l'ajoute à l'enfant
		if (pos == Parent1.Seq.begin() + coupure1) {
			Enfant.Seq.push_back(Parent2.Seq[i]);
		}
	}

	// On refait la même chose avec la fin de la séquence du parent 1
	for (int i = coupure1; i < unProb.NbVilles; i++) {
		// On cherche si la ville 'i' est dans la sous-séquence du parent 2
		auto pos = std::find(Parent2.Seq.begin() + coupure1, Parent2.Seq.begin() + coupure2, Parent2.Seq[i]);

		// Si l'élément du parent 1 n'est pas dans la sous-séquence, on l'ajoute à l'enfant
		if (pos == Parent2.Seq.begin() + coupure2) {
			Enfant.Seq.push_back(Parent1.Seq[i]);
		}
	}

	//**NE PAS ENLEVER
	EvaluerSolution(Enfant, unProb, unAlgo);
	//AfficherUneSolution(Enfant, unProb);
	return (Enfant);
}
*/
//*********************************************************************************************************
//Realise le REMPLACEMENT de la population pour la prochaine generation. Cette fonction prend les TaillePop 
//solutions de la population "Parents" et les TaillePopEnfant solutions de la population "Enfants" et
//determine les TaillePop solutions a retenir pour la prochaine generation.  Les solutions retenues doivent 
//�tre placees dans la population "Parents" (qui est retournee). NB: Le tri de la population est pr�vue au  
//retour dans la fonction principale.
//*********************************************************************************************************
//**A DEFINIR PAR L'ETUDIANT*******************************************************************************
//**NB: IL FAUT RESPECTER LA DEFINITION DES PARAMETRES
//********************************************************************************************************* 
int Remplacement(std::vector<TSolution> &Parents, std::vector<TSolution> Enfants, TProblem unProb, TAlgo unAlgo)
{
	// Remplacement sans doublon
	std::vector<TSolution> Temporaire;
	Temporaire.reserve(unAlgo.TaillePop + unAlgo.TaillePopEnfant);

	// Ajout des parents, excluant les doublons
	for (int i = 0; i < unAlgo.TaillePop; i++) {
		TSolution& TmpSol = Parents[i];

		auto pos = std::find_if(
			Temporaire.begin(),
			Temporaire.end(),
			[&TmpSol](TSolution& sol) {
				if (sol.FctObj == TmpSol.FctObj) {
					return (sol.Seq == TmpSol.Seq);
				}
				else {
					return false;
				}
			}
		);

		if (pos == Temporaire.end()) {
			// Aucun n'élément de Temporaire n'a la même séquence
			Temporaire.push_back(TmpSol);
		}
	}

	// Ajout des enfants, excluant les doublons
	for (int i = 0; i < unAlgo.TaillePopEnfant; i++) {
		TSolution& TmpSol = Enfants[i];

		auto pos = std::find_if(
			Temporaire.begin(),
			Temporaire.end(),
			[&TmpSol](TSolution& sol) {
				if (sol.FctObj == TmpSol.FctObj) {
					return (sol.Seq == TmpSol.Seq);
				}
				else {
					return false;
				}
			}
		);

		if (pos == Temporaire.end()) {
			// Aucun n'élément de Temporaire n'a la même fonction séquence
			Temporaire.push_back(TmpSol);
		}
	}

	// On trie le vecteur temporaire
	TrierPopulation(Temporaire, 0, Temporaire.size());

	// On enregistre le nombre de solutions originales dans temporaire
	int NombreSolutionsOriginales = Temporaire.size();

	// Si le temporaire a plus d'éléments que la population, on garde les meilleurs
	if (NombreSolutionsOriginales >= unAlgo.TaillePop) {
		Temporaire.resize(unAlgo.TaillePop);
	}
	// Si le temporaire est plus petit que la population, on ajoute des enfants aléatoirement en double
	else {
		for (int i = NombreSolutionsOriginales; i < unAlgo.TaillePop; i++) {
			TSolution Alea = Temporaire[rand() % NombreSolutionsOriginales];
			Temporaire.push_back(Alea);
		}
	}

	// On remplace les parents par la population temporaires
	Parents = Temporaire;

	// On renvoie le nombre de solutions originales
	return NombreSolutionsOriginales;
}

int CalculSolutionsOriginales(vector<TSolution>& pop) {
	int count = 0;
	for (int i = 0; i < pop.size(); i++) {
		TSolution& TmpSol = pop[i];

		auto pos = std::find_if(
			pop.begin(),
			pop.begin() + i,
			[&TmpSol](TSolution& sol) {
				if (sol.FctObj == TmpSol.FctObj) {
					return (sol.Seq == TmpSol.Seq);
				}
				else {
					return false;
				}
			}
		);

		if (pos == pop.begin() + i) {
			// Aucun n'élément de n'a la même fonction séquence
			count++;
		}
	}
	return count;
}

// Cette fonction crée un fichier CSV ou ajoute à ce fichier les informations de la solution finale trouvée
// Ces informations sont formatées en CSV de façon à faciliter l'analyse par un programme exerne
void AjouterResultatsFichierCSV(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName, int NombreSolutionsUniques) {
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
		FileStream << "Nom,NbVilles,TaillePop,TaillePopEnfant,ProbCr,ProbMut,Gen,NbSolUniques,TotalEval,MaxEval,FctObjDepart,FctObjFinale,Etat,EvalPourTrouver,Seq" << std::endl;
	}

	// Ajouter les informations sur une seule ligne
	FileStream << unProb.Nom << ",";
	FileStream << unProb.NbVilles << ",";
	FileStream << unAlgo.TaillePop << ",";
	FileStream << unAlgo.TaillePopEnfant << ",";
	FileStream << unAlgo.ProbCr << ",";
	FileStream << unAlgo.ProbMut << ",";
	FileStream << unAlgo.Gen << ",";
	FileStream << NombreSolutionsUniques << ",";
	FileStream << unAlgo.CptEval << ",";
	FileStream << unAlgo.NB_EVAL_MAX << ",";
	FileStream << unAlgo.FctObjSolDepart << ",";
	FileStream << uneSol.FctObj << ",";
	FileStream << uneSol.Valide << ",";
	FileStream << uneSol.NbEvaltoGet << ",";

	for (int i = 0; i < unProb.NbVilles - 1; i++) {
		FileStream << uneSol.Seq[i] << "-";
	}
	FileStream << uneSol.Seq[unProb.NbVilles - 1] << std::endl;

	// Fermer le fichier
	FileStream.close();
}