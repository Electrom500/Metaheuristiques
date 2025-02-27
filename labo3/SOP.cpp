#include "Entete.h"
#include "SOP.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>    // For std::copy
#include <iterator>     // For std::istream_iterator
#include <numeric>      // For std::iota
#include <random>
#include <chrono>

// Pour remplacer la fonction Windows GetTickCount
unsigned int GetTickCount() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

//DESCRIPTION:	Lecture du Fichier probleme et initialiation de la structure Problem
void LectureProbleme(std::string FileName, TProblem& unProb, TAlgo& unAlgo) {
    // Lecture du fichier de données
    std::ifstream File(FileName);
    std::vector<int> FileContent;

    std::copy(
        std::istream_iterator<int>(File),
        std::istream_iterator<int>(),
        std::back_inserter(FileContent)
    );
    
    File.close();

    // Écriture du problème
    unProb.Nom = FileName;
    unProb.NbVilles = FileContent[0];
    unProb.Distance.clear();
    unProb.Pred.clear();
    unProb.Succ.clear();

    // Redimensionnement des vecteurs
    unProb.Distance.resize(unProb.NbVilles);
    unProb.Pred.resize(unProb.NbVilles);
    unProb.Succ.resize(unProb.NbVilles);

    // Écriture de la matrice et des prédécesseurs/successeurs
    for (int i = 0; i < unProb.NbVilles; i++) {
        unProb.Distance[i].resize(unProb.NbVilles);

        for (int j = 0; j < unProb.NbVilles; j++) {
            int value = FileContent[1 + i * unProb.NbVilles + j];
            unProb.Distance[i][j] = value;
            
            if (value == -1) {
                // La ville i doit être visitée avant la ville j
                unProb.Pred[i].push_back(j);
                unProb.Succ[j].push_back(i);
            }
        }
    }

    // Écriture de l'algorithme
    unAlgo.CptEval = 0;
}

//DESCRIPTION: Fonction d'affichage a l'ecran permettant de voir si les donnees du fichier probleme ont ete lues correctement
void StreamProbleme(TProblem unProb, std::ostream& output) {
    // Entête
    std::string type = unProb.Type == 0 ? "MIN" : "MAX";
    output << "****************************************************" << std::endl;
    output << std::setw(10) << std::left << "NOM FICHIER: " << unProb.Nom << std::endl;
    output << std::setw(10) << std::left << "NB VILLLES: " << unProb.NbVilles << std::endl;
    output << std::setw(10) << std::left << "TYPE: " << type << std::endl;
    output << std::endl;

    // Matrice des distances
    output << "DISTANCES" << std::endl;
    for (int i = 0; i < unProb.NbVilles; i++) {
        for (int j = 0; j < unProb.NbVilles; j++) {
            output << std::setw(10) << std::right << unProb.Distance[i][j];
        }
        output << std::endl;
    }

    output << std::endl;

    // Prédécesseurs et successeurs
    output << "PREDECESSEURS" << std::endl;
    for (int i = 0; i < unProb.NbVilles; i++) {
        output << "    " << i << " : ";
        
        if (unProb.Pred[i].size() == 0) {
            output << "aucun";
        }

        for (int j = 0; j < unProb.Pred[i].size(); j ++) {
            output << unProb.Pred[i][j] << ", ";
        }
        output << std::endl;
    }
    output << std::endl;

    output << "SUCCESSEURS" << std::endl;
    for (int i = 0; i < unProb.NbVilles; i++) {
        output << "    " << i << " : ";
        
        if (unProb.Succ[i].size() == 0) {
            output << "aucun";
        }

        for (int j = 0; j < unProb.Succ[i].size(); j ++) {
            output << unProb.Succ[i][j] << ", ";
        }
        output << std::endl;
    }

    output << "****************************************************" << std::endl << std::endl;
}

void AfficherProbleme(TProblem unProb) {
    StreamProbleme(unProb, std::cout);
}

//DESCRIPTION: Generation d'une population initiale de solutions aleatoires valides, incluant leur evaluation et validation
void CreerPopulationInitialeAleatoireValide(std::vector<TSolution>& unePop, TProblem unProb, TAlgo& unAlgo) {
    
    // Itérer sur les individus à créer
    for (int i = 0; i < unePop.size(); i++) {
        // Création d'une solution
        TSolution uneSol;
        uneSol.Seq.clear();

        // Faire une copie du vecteur de préséance
        std::vector<std::vector<int>> TmpPred = unProb.Pred;

        // Itérer sur les villes à ajouter
        for (int j = 0; j < unProb.NbVilles; j++) {
            // Liste des villes pour lesquelles il n'y a plus de prédécesseurs
            std::vector<int> VillesAdmissibles;

            for (int k = 0; k < unProb.NbVilles; k++) {
                // Vérification si la ville a déjà été mise et si elle n'a plus de prédécesseurs
                auto pos = std::find(uneSol.Seq.begin(), uneSol.Seq.end(), k);
                
                if ((pos == uneSol.Seq.end()) && (TmpPred[k].size() == 0)) {
                    VillesAdmissibles.push_back(k);
                }
            }

            // Piger une ville aléatoirement parmi les villes admissibles
            int idx = rand() % VillesAdmissibles.size();
            int ville = VillesAdmissibles[idx];

            // Ajouter la ville correspondante
            uneSol.Seq.push_back(ville);

            // Retirer la ville sélectionnée des conditions de précéance
            for (int k = 0; k < unProb.NbVilles; k++) {
                auto pos = std::find(TmpPred[k].begin(), TmpPred[k].end(), ville);

                if (pos != TmpPred[k].end()) {
                    TmpPred[k].erase(pos);
                }
            }
        }

        // Évaluer la solution
        EvaluerSolution(uneSol, unProb, unAlgo);

        // Ajouter la solution à la population
        unePop[i] = uneSol;
    }
}

//DESCRIPTION: Evaluation de la fonction objectif (distance totale) d'une solution + Validation de la solution + MAJ du compteur d'�valuation
//				Validation de la solution: verification des contraintes de preseance + la presence de chaque ville une et une seule fois. Si invalide: fonction objectif = INT_MAX
void EvaluerSolution(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo) {
    // Vérification de la validité de la solution (longueur, toutes les tâches)
    bool validSize = int(uneSol.Seq.size()) == unProb.NbVilles;

    std::vector<int> SortedSeq = uneSol.Seq;
    std::vector<int> SortedVilles(unProb.NbVilles);

    std::iota(SortedVilles.begin(), SortedVilles.end(), 0);
    std::sort(SortedSeq.begin(), SortedSeq.end());
    
    bool correct_elements = std::equal(SortedSeq.begin(), SortedSeq.end(), SortedVilles.begin());

    // Vérification des conditions de précéance
    bool respect_pred = true;

    for (int i = 0; i < unProb.NbVilles; i++) {
        int ville = uneSol.Seq[i];

        for (int j = 0; j < unProb.Pred[ville].size(); j++) {
            int pred = unProb.Pred[ville][j];

            auto res = std::find(uneSol.Seq.begin(), uneSol.Seq.begin() + i, pred);

            if (res == uneSol.Seq.begin() + i) {
                respect_pred = false;
            }
        }
    }

    // Calcul de la fonction objectif
    uneSol.FctObj = 0;

    for (int i = 1; i < unProb.NbVilles; i++) {
        // Calcul de la distance avec la ville précédente
        int distance = unProb.Distance[uneSol.Seq[i - 1]][uneSol.Seq[i]];

        if (distance == -1) {
            respect_pred = false;
        }

        uneSol.FctObj += distance;
    }

    uneSol.Valide = validSize && correct_elements && respect_pred;

    // Si la solution est invalide, INT_MAX
    if (!uneSol.Valide) {
        uneSol.FctObj = INT_MAX;
    }

    // Nombre d'évaluations pour trouver la solution
    unAlgo.CptEval++;
    uneSol.NbEvaltoGet = unAlgo.CptEval;
}


//DESCRIPTION: Affichage a l'ecran d'une solution : sequence, fonction objectif, etat de validite et l'effort necessaire pour l'obtenir
void StreamUneSolution(const TSolution uneSol, TProblem unProb, std::ostream& output) {
    // Entête
    std::string etat = uneSol.Valide ? "VALIDE" : "INVALIDE";

    output << "OBJ: ";
    output << std::setw(16) << std::left << uneSol.FctObj;
    output << "Etat: ";
    output << std::setw(10) << std::left << etat;
    output << "#EVAL pour trouver: " << uneSol.NbEvaltoGet << std::endl;

    output << "SEQ: ";
    for (int i = 0; i < unProb.NbVilles - 1; i++) {
        output << uneSol.Seq[i] << "-";
    }
    output << uneSol.Seq[unProb.NbVilles - 1] << std::endl << std::endl;
}

void AfficherUneSolution(const TSolution uneSol, TProblem unProb) {
    // Envoyer la solution dans cout
    StreamUneSolution(uneSol, unProb, std::cout);
}

//DESCRIPTION: Affichage des solutions de la population recue en parametre
void StreamPopulation(std::vector<TSolution> unePop, int Iter, TProblem unProb, std::ostream& output) {
    output << "POPULATION POUR LA GENERATION " << Iter << std::endl;

    for (int i = 0; i < unePop.size(); i++) {
        StreamUneSolution(unePop[i], unProb, output);
    }
}
void AfficherPopulation(std::vector<TSolution> unePop, int Iter, TProblem unProb) {
    StreamPopulation(unePop, Iter, unProb, std::cout);
}

//DESCRIPTION: Tri croissant (probleme de minimisation) des individus dans la population entre Debut et Fin-1 INCLUSIVEMENT 
void TrierPopulation(std::vector<TSolution>& unePop, int Debut, int Fin) {
    if (unePop.begin() + Fin > unePop.end()) {
        Fin = unePop.end() - unePop.begin();
    }

    std::sort(
        unePop.begin() + Debut,
        unePop.begin() + Fin,
        [](TSolution& solA, TSolution& solB) {return solA.FctObj < solB.FctObj;}
    );
}

//DESCRIPTION: Selection d'un individu par tournoi (taille 2)
int Selection(std::vector<TSolution> unePop, int _Taille, TProblem unProb) {
    int idxA = rand() % _Taille;
    int idxB = rand() % (_Taille - 1);

    if (idxB == idxA) {
        idxB = _Taille - 1;
    }

    TSolution solA = unePop[idxA];
    TSolution solB = unePop[idxB];

    if (solA.FctObj >= solB.FctObj) {
        return idxA;
    }
    else {
        return idxB;
    }
}

//DESCRIPTION: Mutation (modification aleatoire) d'une solution. La solution mutuee est evaluee avant d'etre retournee.
//- Si solution valide en entr�e: une mutation valide est produite et retournee - swap adjacent
//- Si solution NON valide en entr�e : une mutation(sans assurer la validite) est produite et retournee - swap aleatoire
void Mutation(TSolution& Mutant, TProblem unProb, TAlgo& unAlgo) {
    // Solution valide
    if (Mutant.Valide) {
        // Swap adjacent valide
        bool mutation_valide;
        int swap;

        do {
            // Piger la position d'un swap, excluant la première et la dernière ville
            swap = 1 + rand() % (unProb.NbVilles - 3);

            // Vérification si le mutant serait valide
            // La ville 'swap' ne doit pas être préalable à la ville 'swap + 1'
            std::vector<int> pred = unProb.Pred[Mutant.Seq[swap + 1]];
            auto pos = std::find(pred.begin(), pred.end(), Mutant.Seq[swap]);

            if (pos == pred.end()) {
                mutation_valide = true;
            }
            else {
                mutation_valide = false;
            }

        } while (!mutation_valide);

        // Effectuer le swap
        int TmpVille = Mutant.Seq[swap];
        Mutant.Seq[swap] = Mutant.Seq[swap + 1];
        Mutant.Seq[swap + 1] = TmpVille;
    }
    else {
        // Swap aléatoire
        int idxA = rand() % (unProb.NbVilles - 2);
        int idxB = rand() % (unProb.NbVilles - 3);

        if (idxA == idxB) {
            idxB = unProb.NbVilles - 3;
        }

        // Effectuer le swap
        int TmpVille = Mutant.Seq[idxA];
        Mutant.Seq[idxA] = Mutant.Seq[idxB];
        Mutant.Seq[idxB] = TmpVille;
    }

    // Evaluer le mutant
    EvaluerSolution(Mutant, unProb, unAlgo);
}

//DESCRIPTION: Affichage a l'ecran des resultats de l'algorithme evolutionnaire: solution finale (avec etat de validite), nombre d'evaluations et parametres de l'algorithme
void StreamResultats(TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::ostream& output) {
    // 1re ligne
    output << "NOM(" << unProb.Nom << ")\t";
    output << "N(" << unProb.NbVilles << ")\t";
    output << "TOTAL EVAL(" << unAlgo.CptEval << "/" << unAlgo.NB_EVAL_MAX << ")\t";
    output << "SOL DEPART(" << unAlgo.FctObjSolDepart << ")" << std::endl;

    // Solution
    StreamUneSolution(uneSol, unProb, output);
}
void AfficherResultats(TSolution uneSol, TProblem unProb, TAlgo unAlgo) {
    StreamResultats(uneSol, unProb, unAlgo, std::cout);
}

//DESCRIPTION: Affichage dans un fichier texte des resultats de l'algorithme evolutionnaire: solution finale (avec etat de validite), nombre d'evaluations et parametres de l'algorithme
void AfficherResultatsFichier(TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName) {
    // Ouvrir le fichier
    std::ofstream File(FileName);

    StreamResultats(uneSol, unProb, unAlgo, File);

    // Close file
    File.close();
}

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement pour les differentes structures en parametre
void LibererMemoireFinPgm(std::vector<TSolution>& unePop, std::vector<TSolution>& unePopEnfant, TSolution& uneBest, TProblem& unProb, TAlgo unAlgo) {
    // Pas vraiment nécessaires car les vecteurs se clear automatiquement quand ils sont
    // suprimés...
}
