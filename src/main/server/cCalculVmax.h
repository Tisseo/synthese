/*------------------------------------*
|                                    |
|  APDOS / APISAPI - SYNTHESE v0.7   |
|  � Hugues Romain 2000/2003         |
|  cCalculVmax.h                     |
|  Header Classe Calcul des Vmax     |
|                                    |
*------------------------------------*/

#ifndef SYNTHESE_VMAX_H
#define SYNTHESE_VMAX_H

#include "cEnvironnement.h"
#include "cElementTrajet.h"

#include <fstream>

/** @ingroup m33
*/
class cCalculVmax
{
    private:

        // Variables
        cEnvironnement* vEnvironnement;
        cElementTrajet** vET;    // Par point d'arr�t
        int* vMeilleureDuree; // Par point d'arr�t

        // Methodes
        cElementTrajet** ListeDestinations( cElementTrajet* TrajetEffectue, bool EnCorrespondance, ofstream& F );
        void CalculRecursif( cElementTrajet* TrajetEffectue, bool EnCorrespondance, ofstream& F );


    public:

        // Methodes publiques
        void Calcule();

        // Constructeur
        cCalculVmax( cEnvironnement* );
};

#endif
