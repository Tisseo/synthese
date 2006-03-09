#ifndef SYNTHESE_CTABLEAUAFFICHAGESELECTIFDESTINATIONS_H
#define SYNTHESE_CTABLEAUAFFICHAGESELECTIFDESTINATIONS_H
#include "cTableauAffichage.h"

#include "cTableauDynamique.h"


/** Tableau d'affichage s�lectionnant les lignes � partir de la destination
 @ingroup m34
*/
class cTableauAffichageSelectifDestinations :
            public cTableauAffichage
{
    protected:
        cTableauDynamique<const LogicalPlace*> _DestinationsSelection; //!< Destinations � afficher absolument
        int _DureePresenceDestinationForcee; //!< Dur�e pendant laquelle une destination est forc�e

    public:
        cTableauAffichageSelectifDestinations( const std::string& __Code );
        ~cTableauAffichageSelectifDestinations( void );
        // Ajoute les terminus des lignes au d�part de l'arr�t
        bool AjouteDestinationsDefaut( void );
        // Ajout de destination forc�e
        bool AddDestination( const LogicalPlace* __Destination );
        // Calcul de la liste des d�parts � afficher
        cDescriptionPassage* Calcule( const synthese::time::DateTime& __MomentDebut );
};

#endif
