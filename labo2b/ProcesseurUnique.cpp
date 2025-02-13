#include "Entete.h"
#include "ProcesseurUnique.h"

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
    unProb.N = FileContent[0];
    unProb.Temps.clear();
    unProb.Poids.clear();
    unProb.DateDue.clear();

    std::copy(
        FileContent.begin() + 1,
        FileContent.begin() + unProb.N + 1,
        std::back_inserter(unProb.Temps)
    );

    std::copy(
        FileContent.begin() + unProb.N + 1,
        FileContent.begin() + 2 * unProb.N + 1,
        std::back_inserter(unProb.Poids)
    );

    std::copy(
        FileContent.begin() + 2 * unProb.N + 1,
        FileContent.begin() + 3 * unProb.N + 1,
        std::back_inserter(unProb.DateDue)
    );

    // Écriture de l'algorithme
    unAlgo.CptEval = 0;
}

//DESCRIPTION: Fonction d'affichage a l'ecran permettant de voir si les donnees du fichier probleme ont ete lues correctement
void StreamProbleme(TProblem unProb, std::ostream& output) {
    // Entête
    std::string type = unProb.Type == 0 ? "MIN" : "MAX";
    output << "****************************************************" << std::endl;
    output << std::setw(30) << std::left << "NOM FICHIER: " << unProb.Nom << std::endl;
    output << std::setw(30) << std::left << "NB TACHES: " << unProb.N << std::endl;
    output << std::setw(30) << std::left << "TYPE: " << type << std::endl;
    output << std::endl;

    // Tâches
    output << "NO TACHE:" << std::endl;
    for (int i = 0; i < unProb.N ; i++) {
        output << std::setw(6) << std::right << i;
    }
    output << std::endl;

    // Temps
    output << "TEMPS:" << std::endl;
    for (int i = 0; i < unProb.N ; i++) {
        output << std::setw(6) << std::right << unProb.Temps[i];
    }
    output << std::endl;

    // Poids
    output << "POIDS:" << std::endl;
    for (int i = 0; i < unProb.N ; i++) {
        output << std::setw(6) << std::right << unProb.Poids[i];
    }
    output << std::endl;

    // Echeance
    output << "DATE DUE:" << std::endl;
    for (int i = 0; i < unProb.N ; i++) {
        output << std::setw(6) << std::right << unProb.DateDue[i];
    }
    output << std::endl;
    output << "****************************************************" << std::endl << std::endl;
}

void AfficherProbleme(TProblem unProb) {
    StreamProbleme(unProb, std::cout);
}

void StreamSolution(const TSolution uneSol, TProblem unProb, std::string Titre, bool AvecCalcul, std::ostream& output) {
    // Entête
    std::string etat = uneSol.Valide ? "VALIDE" : "INVALIDE";

    output << Titre << std::endl;
    output << "  OBJ: ";
    output << std::setw(9) << std::left << uneSol.FctObj;
    output << "Etat: ";
    output << std::setw(10) << std::left << etat;
    output << "#EVAL pour trouver: " << uneSol.NbEvaltoGet << std::endl;

    output << "  SEQ: ";
    for (int i = 0; i < unProb.N - 1; i++) {
        output << uneSol.Seq[i] << "-";
    }
    output << uneSol.Seq[unProb.N - 1] << std::endl;

    // Avec calcul
    if (AvecCalcul) {
        // Temps
        output << std::setw(9) << std::right << "Temps: ";
        
        for (int i = 0; i < unProb.N; i++) {
            output << std::setw(8) << std::right << unProb.Temps[uneSol.Seq[i]];
        }
        output << std::endl;

        // Fin
        output << std::setw(9) << std::right << "Fin: ";
        
        for (int i = 0; i < unProb.N; i++) {
            output << std::setw(8) << std::right << uneSol.Fin[i];
        }
        output << std::endl;

        // Echeance
        output << std::setw(9) << std::right << "Echeance: ";
        
        for (int i = 0; i < unProb.N; i++) {
            output << std::setw(8) << std::right << unProb.DateDue[uneSol.Seq[i]];
        }
        output << std::endl;

        // Retard
        output << std::setw(9) << std::right << "Retard: ";
        
        for (int i = 0; i < unProb.N; i++) {
            output << std::setw(8) << std::right << uneSol.Retard[i];
        }
        output << std::endl;

        // Poids
        output << std::setw(9) << std::right << "Poids: ";
        
        for (int i = 0; i < unProb.N; i++) {
            output << std::setw(8) << std::right << unProb.Poids[uneSol.Seq[i]];
        }
        output << std::endl;

        // wRetard
        output << std::setw(9) << std::right << "wRetard: ";
        
        for (int i = 0; i < unProb.N; i++) {
            output << std::setw(8) << std::right << uneSol.wRetard[i];
        }
        output << std::endl;
    }
    output << std::endl << std::endl;
}

//DESCRIPTION: Affichage d'une solution a l'ecran (avec ou sans d�tails des calculs)
void AfficherSolution(const TSolution uneSol, TProblem unProb, std::string Titre, bool AvecCalcul) {
    // Envoyer la solution dans cout
    StreamSolution(uneSol, unProb, Titre, AvecCalcul, std::cout);
}

//DESCRIPTION:	Evaluation de la fonction objectif d'une solution et MAJ du compteur d'evaluations. La fonction objectif represente la somme des retards ponderes de la sequence.
//				NB: L'evaluation d'une solution inclue la validation de la solution (presence de chacune des taches (sans doublons) + longueur de la solution + domaine admissible des variables)
void EvaluerSolution(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo) {
    // Vérification de la validité de la solution
    bool validSize = int(uneSol.Seq.size()) == unProb.N;

    std::vector<int> SortedSeq = uneSol.Seq;
    std::vector<int> SortedTasks(unProb.N);

    std::iota(SortedTasks.begin(), SortedTasks.end(), 0);
    std::sort(SortedSeq.begin(), SortedSeq.end());
    
    bool correct_elements = std::equal(SortedSeq.begin(), SortedSeq.end(), SortedTasks.begin());
    uneSol.Valide = validSize && correct_elements;

    // Calcul de la fonction objectif
    int cumul = 0;
    uneSol.FctObj = 0;

    for (int i = 0; i < unProb.N; i++) {
        // Calcul de la fin de chaque tâche
        cumul += unProb.Temps[uneSol.Seq[i]];
        uneSol.Fin[i] = cumul;

        // Calcul du retard de chaque tâche
        uneSol.Retard[i] = std::max(0, cumul - unProb.DateDue[uneSol.Seq[i]]);

        // Calcul du retard pondéré de chaque tâche
        uneSol.wRetard[i] = uneSol.Retard[i] * unProb.Poids[uneSol.Seq[i]];

        // Calcul de la fonction objectif
        uneSol.FctObj += uneSol.wRetard[i];
    }

    // Nombre d'évaluations pour trouver la solution
    unAlgo.CptEval++;
    uneSol.NbEvaltoGet = unAlgo.CptEval;
}

//DESCRIPTION:	Creation d'une sequence aleatoire des N taches et appel a l'evaluation de la fonction objectif. Allocation dynamique de memoire pour le detail de la solution
void CreerSolutionAleatoire(TSolution& uneSol, TProblem unProb, TAlgo& unAlgo) {
    // Créer la séquence de tâches ordonnées
    uneSol.Seq.resize(unProb.N);
    std::iota(uneSol.Seq.begin(), uneSol.Seq.end(), 0);

    // Mélanger les tâches
    std::random_shuffle(uneSol.Seq.begin(), uneSol.Seq.end());

    // Initialiser les vecteurs de solution
    uneSol.Fin.resize(unProb.N);
    uneSol.Retard.resize(unProb.N);
    uneSol.wRetard.resize(unProb.N);

    // Appeler l'évaluation de la solution
    EvaluerSolution(uneSol, unProb, unAlgo);
}

//DESCRIPTION: Affichage a l'ecran de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
void StreamResultats(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, bool AvecCalcul, std::ostream& output) {
    // 1re ligne
    output << "NOM(" << unProb.Nom << ")\t";
    output << "N(" << unProb.N << ")\t";
    output << "NB VOISINS(" << unAlgo.TailleVoisinage << ")\t";
    output << "TOTAL EVAL(" << unAlgo.CptEval << "/" << unAlgo.NB_EVAL_MAX << ")\t";
    output << "SOL DEPART(" << unAlgo.FctObjSolDepart << ")" << std::endl;

    // Solution
    StreamSolution(uneSol, unProb, "SOLUTION FINALE:", AvecCalcul, output);
}


void AfficherResultats(const TSolution uneSol, TProblem unProb, TAlgo unAlgo) {
    StreamResultats(uneSol, unProb, unAlgo, false, std::cout);
}

//DESCRIPTION: Affichage dans un fichier (en append) de la solution finale (et de son statut de validite), du nombre d'evaluations effectuees et de certains parametres
void AfficherResultatsFichier(const TSolution uneSol, TProblem unProb, TAlgo unAlgo, std::string FileName) {
    // Ouvrir le fichier
    std::ofstream File(FileName);

    StreamResultats(uneSol, unProb, unAlgo, true, File);

    // Close file
    File.close();
}

//DESCRIPTION:	Liberation de la memoire allouee dynamiquement pour les differentes structures en parametre
void LibererMemoireFinPgm(TSolution uneCourante, TSolution uneNext, TSolution uneBest, TProblem unProb) {
    // Pas vraiment nécessaires car les vecteurs se clear automatiquement quand ils sont
    // suprimés...
    uneCourante.Seq.clear();
    uneCourante.Fin.clear();
    uneCourante.Retard.clear();
    uneCourante.wRetard.clear();

    uneNext.Seq.clear();
    uneNext.Fin.clear();
    uneNext.Retard.clear();
    uneNext.wRetard.clear();

    uneBest.Seq.clear();
    uneBest.Fin.clear();
    uneBest.Retard.clear();
    uneBest.wRetard.clear();

    unProb.Temps.clear();
    unProb.Poids.clear();
    unProb.DateDue.clear();
}