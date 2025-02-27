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
void Remplacement(std::vector<TSolution> &Parents, std::vector<TSolution> Enfants, TProblem unProb, TAlgo unAlgo);

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

	//**Lecture des param�tres
	NomFichier.assign(Param[1]);
	LAlgo.TaillePop			= atoi(Param[2]);
	LAlgo.ProbCr			= atof(Param[3]);
	LAlgo.ProbMut			= atof(Param[4]);
	LAlgo.NB_EVAL_MAX		= atoi(Param[5]);
	LAlgo.TaillePopEnfant	= (int)ceil(LAlgo.TaillePop * LAlgo.ProbCr);
	LAlgo.Gen = 0;

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
	std::cout << "(Pop Ini) Best OBJ: " << Best.FctObj; (!Best.Valide) ? std::cout << "***NON Valide\n" : std::cout << endl;

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
		Remplacement(Pop, PopEnfant, LeProb, LAlgo);
		TrierPopulation(Pop, 0, LAlgo.TaillePop);
		
		//**Conservation de la meilleure solution
		if (Pop[0].Valide && (Pop[0].FctObj < Best.FctObj)) Best = Pop[0];																					
		std::cout << "(Gen#" << setw(3) << LAlgo.Gen << ") Best OBJ: " << Best.FctObj; (!Best.Valide) ? std::cout << "***NON Valide\n": std::cout << endl;  
	}while (LAlgo.CptEval < LAlgo.NB_EVAL_MAX);			//**NE PAS ENLEVER

	AfficherResultats (Best, LeProb, LAlgo);			//**NE PAS ENLEVER
	AfficherResultatsFichier (Best, LeProb, LAlgo, "Resultats.txt");
	
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
	
	//Pour dimensionner le vecteur solution de Enfant
	//Enfant.Seq.resize(unProb.NbVilles);

	//METHODE BIDON: Recopie les genes du Parent1 dans Enfant
	Enfant = Parent1;

	//**NE PAS ENLEVER
	EvaluerSolution(Enfant, unProb, unAlgo);
	//AfficherUneSolution(Enfant, unProb);
	return (Enfant);
}

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
void Remplacement(std::vector<TSolution> &Parents, std::vector<TSolution> Enfants, TProblem unProb, TAlgo unAlgo)
{
	//METHODE ACTUELLE BIDON: La population Parent demeure inchang�e   -    a modifier

	//INFOS pour d�finir votre methode de remplacement...

	//**Declaration et dimension dynamique d'une population temporaire pour contenir tous les parents + les enfants
	//std::vector<TSolution> Temporaire;
	//Temporaire.resize(unAlgo.TaillePop + unAlgo.TaillePopEnfant);
	//**Pour trier toute la population temporaire, il suffit de faire l'appel suivant: 	TrierPopulation(Temporaire, 0, unAlgo.TaillePop+unAlgo.TaillePopEnfant);

	//**A LA FIN: Liberation de la population temporaire
	//int i;
	//for (i = 0; i < Temporaire.size(); i++)
	//	Temporaire[i].Seq.clear();
	//Temporaire.clear();
}
