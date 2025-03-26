#include "Entete.h"
#pragma comment (lib,"PsoDLL.lib")
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Le fichier de probleme (.txt) et les fichiers de la DLL (PsoDLL.dll et PsoDLL.lib) doivent se trouver dans le repertoire courant du projet pour une execution a l'aide du compilateur.
//Indiquer les arguments du programme dans les proprietes du projet - débogage - arguments.
//Sinon, utiliser le répertoire execution.

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION: Affichage des solutions de l'essaim a une iteration
extern "C" _declspec(dllimport) void AfficherEssaim(const std::vector<tParticule> unEssaim, int Iter, tProblem unProb);

//DESCRIPTION: Affichage d'une solution dans un fichier: ses variables (detail position), sa fonction objectif, son etat de validite et l'effort necessaire pour l'obtenir
//3e parametre (booleen) permet d'afficher ou non le detail de la position (solution)
extern "C" _declspec(dllimport) void AfficherUneSolution(const tPosition Pos, tProblem unProb, bool AffichePos);

//DESCRIPTION: Affichage d'une solution dans un fichier: ses variables (position), sa fonction objectif, son etat de validite et l'effort necessaire pour l'obtenir
extern "C" _declspec(dllimport) void AfficherUneSolutionFichier(const tPosition Pos, tProblem unProb, ofstream &Fichier);

//DESCRIPTION: Affichage des resultats finaux du PSO
extern "C" _declspec(dllimport) void AfficherResultats(tPosition uneBest, tProblem unProb, tAlgo unPSO);

//DESCRIPTION: Affichage des resultats finaux du PSO dans un fichier texte
extern "C" _declspec(dllimport) void AfficherResultatsFichier(tPosition uneBest, tProblem unProb, tAlgo unPSO, std::string FileName);

//DESCRIPTION: Liberation de la memoire allouee dynamiquement
extern "C" _declspec(dllimport) void LibererMemoireFinPgm(std::vector<tParticule> &unEssaim, tProblem unProb, tAlgo unPSO);

//### POUR MAXSAT ###
//DESCRIPTION: Lecture du Fichier probleme MAXSAT et initialiation de la structure Problem
extern "C" _declspec(dllimport) void LectureProblemeMAXSAT(std::string FileName, tProblem &unProb, tAlgo &unPSO);

//DESCRIPTION: Fonction d'affichage a l'ecran permettant de voir si les donnees du fichier problème ont ete lues correctement
extern "C" _declspec(dllimport) void AfficherProblemeMAXSAT(tProblem unProb);

//*****************************************************************************************
// Prototype des fonctions
//*****************************************************************************************
void InitialisationIntervalleVariable(tProblem &unProb);
void EvaluationPosition(tPosition &Pos, tProblem unProb, tAlgo &unPSO);
tPosition InitialisationEssaim(std::vector<tParticule> &unEssaim, tProblem unProb, tAlgo &unPSO);
void InitialisationPositionEtVitesseAleatoire(tParticule &Particule, tProblem unProb);
void InitialisationInformatrices(std::vector<tParticule> &unEssaim, tAlgo &unPSO);
void DeplacerEssaim(std::vector<tParticule> &unEssaim, tProblem unProb, tAlgo &unPSO, tPosition &Meilleure);
void DeplacerUneParticule(tParticule &Particule, tProblem unProb, tAlgo &unPSO);
tParticule* TrouverMeilleureInformatrice(tParticule &Particule, tAlgo &unPSO);
double AleaDouble(double a, double b);
//### Pour MAXSAT
double EvaluerSolutionMAXSAT(tPosition Pos, tProblem unProb);

//******************************************************************************************
// Fonction main
//*****************************************************************************************
int main(int NbParam, char *Param[])
{
	tProblem LeProb;						//**Definition de l'instance de probleme
	tAlgo LePSO;							//**Definition des parametres du PSO
	std::vector<tParticule> Essaim;			//**Ensemble de solutions 
	tPosition Best;							//**Meilleure solution depuis le début de l'algorithme
	
	string NomFichier;						//### Pour MAXSAT
	
	//**Lecture des parametres
	LePSO.TailleEssaim	= atoi(Param[1]);
	LePSO.C1			= atof(Param[2]);
	LePSO.C2			= atof(Param[3]);
	LePSO.C3			= atof(Param[4]);
	LePSO.NB_EVAL_MAX	= atoi(Param[5]);
	//NomFichier.assign(Param[6]);								//### Pour MAXSAT : doit etre active pour MAXSAT
	LePSO.Precision		= 0.00001;								//### pour MAXSAT : peut etre retire pour MAXSAT
	LePSO.Iter			= 0;
	LePSO.CptEval		= 0;
		
	srand(GetTickCount()); //**Precise un germe pour le generateur aleatoire (horloge en millisecondes)
	cout.setf(ios::fixed|ios::showpoint);
	
	//**Spécifications du problème a traiter
	LeProb.Nom = SCHWEFEL;										//**Specifie le probleme traite
	InitialisationIntervalleVariable(LeProb);
	
	//**Lecture du fichier de MAXSAT
	//LectureProblemeMAXSAT(NomFichier, LeProb, LePSO);			//### Pour MAXSAT
	//AfficherProblemeMAXSAT(LeProb);							//### Pour MAXSAT
		
	//**Dimension du tableaux de l'essaim selon le nombre de particules
	Best = InitialisationEssaim(Essaim, LeProb, LePSO);
	InitialisationInformatrices(Essaim, LePSO);
	

	//AfficherEssaim(Essaim, LePSO.Iter, LeProb);
	cout << "Iter# " << setw(3) << LePSO.Iter; AfficherUneSolution(Best, LeProb, true);

	//**Boucle principale du PSO
	while (Best.FctObj > LePSO.Precision && LePSO.CptEval < LePSO.NB_EVAL_MAX) 	//### POUR MAXSAT : NE PAS ENLEVER LA CONDITION SUR LE NOMBRE D'EVALUATIONS
	{																			//### POUR MAXSAT : ENLEVER CONDITION D'ARRET SUR PRECISION						
		LePSO.Iter++;
		DeplacerEssaim(Essaim, LeProb, LePSO, Best);  
		//AfficherEssaim(Essaim, LePSO.Iter, LeProb);
		cout << "Iter# " << setw(3) << LePSO.Iter;	AfficherUneSolution(Best, LeProb, false);
	};

	AfficherResultats (Best, LeProb, LePSO);									//**NE PAS ENLEVER
	AfficherResultatsFichier (Best, LeProb, LePSO, "Resultats.txt");
	LibererMemoireFinPgm(Essaim, LeProb, LePSO);

	//system("PAUSE");
	return 0;
}

//-----------------------------------------------------------------------
//DESCRIPTION: Determine l'intervalle de recherche selon le problème
void InitialisationIntervalleVariable(tProblem &unProb)
{
	switch(unProb.Nom)
	{
		case ALPINE:	unProb.Xmin = -10.0;	unProb.Xmax = 10.0;		unProb.D = 5; break;
		case SPHERE:	unProb.Xmin = -5.12;	unProb.Xmax = 5.12;		unProb.D = 10; break;
		case SCHWEFEL:	unProb.Xmin = -500.0;	unProb.Xmax = 500.0;	unProb.D = 5; break; //***** MODIF EquipeH01
		default:		unProb.Xmin = 0.0;		unProb.Xmax = 0.0;		unProb.D = 0; break; 
	}
}

//-----------------------------------------------------------------------
//DESCRIPTION: Evalue la position de la particule selon le probleme
void EvaluationPosition(tPosition &Pos, tProblem unProb, tAlgo &unPSO)
{
	double valeur = 0.0;
	int d;

	switch (unProb.Nom)
	{
	case ALPINE: //Alpine: Min 0 en (0,0 ... 0)
		for (d = 0; d < unProb.D; d++) valeur = valeur + fabs(Pos.X[d] * (sin(Pos.X[d]) + 0.1 * Pos.X[d]));
		break;
	case SPHERE: //Sphere: Min 0 en (0,0 ... 0)
		for (d = 0; d < unProb.D; d++)
		{
			valeur += pow(Pos.X[d], 2);
		}
		break;
	case SCHWEFEL: //***** MODIF EquipeH01
		valeur = 418.9829 * unProb.D;
		for (d = 0; d < unProb.D; d++) valeur -= Pos.X[d] * sin(sqrt(fabs(Pos.X[d])));
		break;
	case MAXSAT:																			//### MAXSAT
		valeur = EvaluerSolutionMAXSAT(Pos, unProb);
		break;
	default: valeur = FLT_MAX;
	}
	Pos.FctObj = valeur;
	
	//Mise a jour du compteur d'evaluation 
	unPSO.CptEval++;
	Pos.NbEvaltoGet = unPSO.CptEval;
}

//-----------------------------------------------------------------------
//DESCRIPTION: Dimension des vecteurs de l'essaim, initialisation des particules et retourne la meilleure solution rencontree
tPosition InitialisationEssaim(std::vector<tParticule> &unEssaim, tProblem unProb, tAlgo &unPSO)
{
	int i;
	tPosition Meilleure;

	unEssaim.resize(unPSO.TailleEssaim);		//**Le tableau utilise les indices de 0 a Taille-1.
	
	for (i=0; i<unPSO.TailleEssaim; i++)
	{
		//Creation + initialisation + evaluation des particules de l'essaim 
		unEssaim[i].Pos.X.resize(unProb.D);
		unEssaim[i].V.resize(unProb.D);
		InitialisationPositionEtVitesseAleatoire(unEssaim[i], unProb);
		EvaluationPosition(unEssaim[i].Pos, unProb, unPSO);
		
		//Initialisation de la meilleure position de la particule a sa position initiale
		unEssaim[i].BestPos.X.resize(unProb.D);
		unEssaim[i].BestPos = unEssaim[i].Pos;
		
		//Conservation de la meilleure solution
		if (i == 0)	
			Meilleure = unEssaim[i].Pos;
		else
			if (unEssaim[i].Pos.FctObj < Meilleure.FctObj)
				Meilleure = unEssaim[i].Pos;
	}	
	unPSO.FctObjSolDepart = Meilleure.FctObj;
	//Retourne la meilleure solution rencontree jusqu'a maintenant
	return(Meilleure);
}

//-----------------------------------------------------------------------
//DESCRIPTION: Initialise aleatoirement le vecteur de positions et de vitesses
void InitialisationPositionEtVitesseAleatoire(tParticule &Particule, tProblem unProb)
{
	int d;

	for(d=0; d<unProb.D; d++)
	{
		Particule.Pos.X[d] = AleaDouble(unProb.Xmin, unProb.Xmax);
		Particule.V[d] = AleaDouble(unProb.Xmin, unProb.Xmax);
	}
}

//-----------------------------------------------------------------------
//DESCRIPTION: Determine le groupement d'informatrices des particules (et le nombre d'informatrices)
void InitialisationInformatrices(std::vector<tParticule> &unEssaim, tAlgo &unPSO)
{
	int i;

	//***** A DEFINIR PAR L'ETUDIANT: nombre d'informatrices et configuration *****
	//Methode BIDON: une particule a comme informatrice seulement elle-meme (n'apporte aucune information supplementaire)
	
	//A DETERMINER: nombre d'informatrices pour une particule
	unPSO.NbInfo = 4;		
	
	//A DETERMINER: configuration des informatrices pour chaque particule
	for (i = 0; i < unPSO.TailleEssaim; i++)
	{
		//Dimension du vecteur d'informatrices
		unEssaim[i].Info.resize(unPSO.NbInfo);
		std::cout << i << "\t";

		for (int k = 0;k < unPSO.NbInfo+1;k++) {
			//std::cout << "test : " << k << endl;
			//std::cout << (k != unPSO.NbInfo / 2) << endl;
			if (k != unPSO.NbInfo / 2) {
				int indice = (i +k - unPSO.NbInfo / 2 + unPSO.TailleEssaim) % unPSO.TailleEssaim;
				std::cout << indice << " ";
				unEssaim[i].Info[k] = &unEssaim[indice];
				//On selectionne les plus proches voisins sur le cercle en excluant elle même
			}
			
		}
		std::cout << endl;
	}
}

//-----------------------------------------------------------------------
//DESCRIPTION: Pour chaque particule, deplacement/mise a jour (vitesse et position) 
void DeplacerEssaim(std::vector<tParticule> &unEssaim, tProblem unProb, tAlgo &unPSO, tPosition &Meilleure)
{
	int i;
	//Deplacement de chaque particule----------------------
	for(i=0; i<unPSO.TailleEssaim; i++)
	{
		//Calcul de la vitesse et maj de sa position
		DeplacerUneParticule(unEssaim[i], unProb, unPSO);
		
		//Evaluation de la nouvelle position-----------------------------
		EvaluationPosition(unEssaim[i].Pos, unProb, unPSO);
		
		//Mise a jour de la meilleure position de la particule-----------
		if(unEssaim[i].Pos.FctObj <= unEssaim[i].BestPos.FctObj)
		{
			unEssaim[i].BestPos = unEssaim[i].Pos;
			//Memorisation du meilleur resultat atteint jusqu'ici------------
			if(unEssaim[i].BestPos.FctObj < Meilleure.FctObj)
				Meilleure = unEssaim[i].BestPos;
		}
	}
}

//-----------------------------------------------------------------------
//DESCRIPTION: Deplacement d'une particule - Calcul de la vitesse et maj de sa position
void DeplacerUneParticule(tParticule &Particule, tProblem unProb, tAlgo &unPSO)
{
	tParticule* MeilleureInfo;
	int d;

	//Meilleure informatrice de la particule-----------------------------
	MeilleureInfo = TrouverMeilleureInformatrice(Particule, unPSO);

	//Calcul de la nouvelle vitesse--------------------------------------
	for(d=0; d<unProb.D; d++)
		Particule.V[d] =	unPSO.C1 * Particule.V[d] + 
							AleaDouble(0,unPSO.C2) * (Particule.BestPos.X[d] - Particule.Pos.X[d]) + 
							AleaDouble(0,unPSO.C3) * (MeilleureInfo->BestPos.X[d] - Particule.Pos.X[d]);

	//Mise a jour de la nouvelle position--------------------------------
	for(d=0; d<unProb.D; d++)
		Particule.Pos.X[d] += Particule.V[d];

	//Confinement d'intervalle pour la valeur des positions--------------
	for(int d=0; d<unProb.D; d++)
	{
		if(Particule.Pos.X[d] < unProb.Xmin)
		{
			Particule.Pos.X[d] = unProb.Xmin;
			Particule.V[d] = 0; //Remise a zéro de la vitesse
		}
		if(Particule.Pos.X[d] > unProb.Xmax)
		{
			Particule.Pos.X[d] = unProb.Xmax;
			Particule.V[d] = 0; //Remise a zéro de la vitesse
		}
	}
}

//-----------------------------------------------------------------------
//DESCRIPTION: Trouve la meilleure informatrice d'une particule
tParticule* TrouverMeilleureInformatrice(tParticule &Particule, tAlgo &unPSO)
{
	int Rang;
	int k;
	double Valeur;

	Rang = 0;
	Valeur = Particule.Info[0]->BestPos.FctObj;
	for(k=1; k<unPSO.NbInfo; k++)
	{
		if(Particule.Info[k]->BestPos.FctObj < Valeur)
		{
			Valeur = Particule.Info[k]->BestPos.FctObj;
			Rang = k;
		}
	}
	return Particule.Info[Rang];
}

//**-----------------------------------------------------------------------
//**DESCRIPTION: Renvoie un double aleatoire entre a et b
double AleaDouble(double a, double b)
{
	double resultat = (double)rand()/RAND_MAX;
	return (resultat*(b-a) + a);
}

//### POUR MAXSAT
//Fonction d'evaluation de la fonction objectif pour une solution du problème de MAXSAT
double EvaluerSolutionMAXSAT(tPosition Pos, tProblem unProb)
{
	//***** A DEFINIR PAR L'ETUDIANT: Calcul de la somme des poids des clauses positives *****
	//Méthode BIDON: la fonction retourne 0 
	return 0;
}