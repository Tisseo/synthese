/** Impl�mentation lieu logique
@file LogicalPlace.cpp
*/

#include <sstream>


#include "LogicalPlace.h"
#include "map/Address.h"
#include "map/Road.h"
#include "cDistanceCarree.h"
#include "cCommune.h"
#include "NetworkAccessPoint.h"
#include "cGareLigne.h"
#include "cArretPhysique.h"

using namespace std;
using namespace synmap;

const int LogicalPlace::UNKNOWN_TRANSFER_DELAY = -1;
const int LogicalPlace::FORBIDDEN_TRANSFER_DELAY = 99;

/** Constructeur de lieu logique de type alias.
 @todo A REVOIR
*/
LogicalPlace::LogicalPlace( LogicalPlace* logicalPlace )
        : cPoint( *logicalPlace )
        , _volatile( false )
        , _transferRules( CorrInterdite )
        , _id( 0 )
        , _road( NULL )
{

    addAliasedLogicalPlace( logicalPlace );
}


/** Constructeur de lieu logique de type standard
 @author Hugues Romain
 @date 2001-2006
*/
LogicalPlace::LogicalPlace( int id, tNiveauCorrespondance __transferRules )
        : cPoint()
        , _volatile( false )
        , _transferRules( __transferRules )
        , _id( id )
        , _road( NULL )
{
    // Valeurs initialis�es
    // _minTransferDelay = CorrespondanceAutorisee() != CorrInterdite ? FORBIDDEN_TRANSFER_DELAY : 0;
}


/** Constructeur de lieu logique de type route enti�re.
 @param id ID du lieu logique
 @param road Route support du lieu logique
 
 @todo MJ mettre ici le remplissage du lieu logique avec toutes les intersections de la route
*/
LogicalPlace::LogicalPlace( Road* road )
        : cPoint()
        , _volatile( false )
        , _transferRules( CorrInterdite )
        , _id( 0 )
        , _road( road )
{
    // Valeurs initialis�es
    // _minTransferDelay = FORBIDDEN_TRANSFER_DELAY;

    setDesignation( road->getTown(), road->getName() );
}


/** Constructeur de lieu logique de type adresse pr�cise.
 @param address Adresse
 @todo Faire le nom qui marche pas
*/
LogicalPlace::LogicalPlace( synmap::Address* address )
        : cPoint()
        , _volatile( true )
        , _transferRules( CorrInterdite )
        , _id( 0 )
        , _road( NULL )
{
    stringstream name;
    name << address->getAddressNumber() << " " << address->getRoad() ->getName();
    setDesignation( address->getRoad() ->getTown(), name.str() );
    addNetworkAccessPoint( address, 0 );
}


/** Constructeur de lieu logique de type principal de commune
 @param town Commune
*/
LogicalPlace::LogicalPlace( cCommune* town )
        : cPoint( *town )
        , _volatile( false )
        , _transferRules( CorrInterdite )
        , _id( 0 )
        , _road( NULL )
{
    setDesignation( town, town->getName() );
}


/** Constructeur de lieu logique de type lieu public
 @param town Commune
*/
LogicalPlace::LogicalPlace( cCommune* town, std::string name )
        : cPoint()
        , _volatile( false )
        , _transferRules( CorrInterdite )
        , _id( 0 )
        , _road( NULL )
{
    setDesignation( town, name );
}



/** Constructeur de lieu logique de type tout lieu de commune
 @param town Commune
*/
LogicalPlace::LogicalPlace( int id, cCommune* town )
        : cPoint( *town )
        , _volatile( false )
        , _transferRules( CorrInterdite )
        , _id( id )
        , _road( NULL )
{}



/*! Destructeur.
*/
LogicalPlace::~LogicalPlace()
{
    for ( size_t i = 0; i < _networkAccessPoints.size(); i++ )
        delete _networkAccessPoints[ i ];
}


LogicalPlace::tNiveauCorrespondance LogicalPlace::NiveauCorrespondance( const cDistanceCarree& D ) const
{
    if ( _transferRules == CorrRecommandeeCourt )
        return ( D.DistanceCarree() > O2COURTLONG ) ? CorrAutorisee : CorrRecommandee;

    return _transferRules;
}



/** Modificateur d'un d�lai minimal de correspondance entre deux quais.
 @param NumeroArretPhysiqueDepart ArretPhysique de d�part (indique la ligne dans la matrice de d�lais de correspondance)
 @param NumeroArretPhysiqueArrivee ArretPhysique d'arriv�e (indique la colonne dans la matrice de d�lais de correspondance)
 @param delay le d�lai minimal de correspondance entre les deux quais, indiqu� en minutes (format num�rique).
 
 Voir
  - LogicalPlace::UNKNOWN_TRANSFER_DELAY
  - LogicalPlace::FORBIDDEN_TRANSFER_DELAY
 
 @return true si l'op�ration a �t� effectu�e avec succ�s
 @author Hugues Romain
 @date 2005-2006
*/
void LogicalPlace::setDelaiCorrespondance( int NumeroArretPhysiqueDepart, int NumeroArretPhysiqueArrivee, int delay )
{
    // Ecriture de la donn�e
    _transferDelay[ NumeroArretPhysiqueDepart ][ NumeroArretPhysiqueArrivee ] = delay;

    /*  if (delay != UNKNOWN_TRANSFER_DELAY && delay != FORBIDDEN_TRANSFER_DELAY)
      {
       // Ecriture du d�lai minimal le plus important attente au d�part du quai de d�part sp�cifi�
    //   if (_transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee] > _maxTransferDelay[NumeroArretPhysiqueDepart])
    //    _maxTransferDelay[NumeroArretPhysiqueDepart] = _transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee];
       
       // Ecriture du d�lai minimal le plus faible du point d'arr�t
    //   if (_transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee] < _minTransferDelay)
    //    _minTransferDelay = _transferDelay[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee];
    */
}


/*
synthese::time::DateTime LogicalPlace::MomentArriveePrecedente(const synthese::time::DateTime& MomentArrivee, const synthese::time::DateTime& MomentArriveeMin) const
{
 synthese::time::DateTime tempMomentArrivee;
 synthese::time::DateTime tempMomentArriveePrecedente = MomentArriveeMin;
 for (cGareLigne* curGLA = _firstArrivalLineStop; curGLA != NULL; curGLA = curGLA->PAArriveeSuivante())
 {
  tempMomentArrivee = MomentArrivee;
  tempMomentArrivee -= int(1);
  if (curGLA->Precedent(tempMomentArrivee, MomentArriveeMin) != -1)
  {
   if (tempMomentArrivee > tempMomentArriveePrecedente)
    tempMomentArriveePrecedente = tempMomentArrivee;
  }
 }
 return (tempMomentArriveePrecedente);
}
 
synthese::time::DateTime LogicalPlace::MomentDepartSuivant(const synthese::time::DateTime& MomentDepart, const synthese::time::DateTime& MomentDepartMax, const synthese::time::DateTime& __MomentCalcul) const
{
 synthese::time::DateTime tempMomentDepart;
 synthese::time::DateTime tempMomentDepartSuivant = MomentDepartMax;
 for (cGareLigne* curGLD= _firstDepartureLineStop; curGLD!=NULL; curGLD=curGLD->PADepartSuivant())
 {
  tempMomentDepart = MomentDepart;
  tempMomentDepart += int(1);
  if (curGLD->Prochain(tempMomentDepart, MomentDepartMax, __MomentCalcul) != INCONNU)
  {
   if (tempMomentDepart < tempMomentDepartSuivant)
    tempMomentDepartSuivant = tempMomentDepart;
  }
 }
 return (tempMomentDepartSuivant);
}
*/


/** Recherche de la solution sans changement vers un autre PA.
 \param ArretPhysiqueArriveePrecedente 0 si aucun.
 \author Hugues Romain
 \date 2001
*/ 
/*cElementTrajet* LogicalPlace::ProchainDirect(LogicalPlace* Destination, synthese::time::DateTime& MomentDepart, const synthese::time::DateTime& ArriveeMax, int ArretPhysiqueArriveePrecedente) const
{
 synthese::time::DateTime tempMomentDepart;
 synthese::time::DateTime tempMomentArrivee;
 tNumeroService tempNumeroService;
 cGareLigne* curGLA;
 cElementTrajet* newET=NULL;
 
 for (cGareLigne* curGLD = vPremiereGareLigneDep; curGLD != NULL; curGLD = curGLD->PADepartSuivant())
  if (curGLD->Circule(MomentDepart, ArriveeMax) && (curGLA = Destination->DessertALArrivee(curGLD->Ligne())) && curGLD->getHoraireDepartPremier(0) < curGLA->getHoraireArriveePremier(0))
  {
   // Calcul de l'horaire ??
   tempMomentDepart = MomentDepart;
   if (vCorrespondanceAutorisee && ArretPhysiqueArriveePrecedente)
    tempMomentDepart += vTableauAttente[ArretPhysiqueArriveePrecedente][curGLD->ArretPhysique()];
   
   tempNumeroService = curGLD->Prochain(tempMomentDepart, ArriveeMax);
   if (tempNumeroService != -1)
   {
    tempMomentArrivee = tempMomentDepart;
    curGLA->CalculeArrivee(*curGLD, tempNumeroService, tempMomentDepart, tempMomentArrivee);
 
    // Stockage
    if (newET==NULL || tempMomentArrivee < newET->MomentArrivee())
    {
     if (newET == NULL)
      newET = new cElementTrajet;
     newET->setInformations(curGLA, curGLD, tempMomentDepart, tempMomentArrivee);
     newET->setService(tempNumeroService);
    }
   }
  }
 
 return(newET);
}*/



/* SET Enlever pour resa car non utilis
cElementTrajet* LogicalPlace::AcheminementDepuisDepot(LogicalPlace* Destination, const synthese::time::DateTime& MomentDepart, cReseau* Reseau) const
{
 for (cGareLigne* curGareLigne = PremiereGareLigneDep(); curGareLigne != NULL; curGareLigne = curGareLigne->PADepartSuivant())
  if (!curGareLigne->Ligne()->Axe()->Autorise() && (Reseau == NULL || curGareLigne->Ligne()->getReseau() == Reseau) && curGareLigne->Destination()->ArretLogique() == Destination)
  {
   cElementTrajet* curET = new cElementTrajet;
   synthese::time::DateTime MomentDepartNew;
   MomentDepartNew = MomentDepart;
   curGareLigne->CalculeDepart(*curGareLigne->Destination(), curGareLigne->Destination()->Precedent(MomentDepartNew, MomentDepart), MomentDepart, MomentDepartNew);
   curET->setInformations(curGareLigne->Destination(), curGareLigne, MomentDepartNew, MomentDepart);
   return(curET);
  }
 return(NULL);
}
 
 
cElementTrajet* LogicalPlace::AcheminementVersDepot(LogicalPlace* Origine, const synthese::time::DateTime& MomentArrivee, cReseau* Reseau) const
{
 for (cGareLigne* curGareLigne = PremiereGareLigneArr(); curGareLigne != NULL; curGareLigne = curGareLigne->PAArriveeSuivante())
  if (!curGareLigne->Ligne()->Axe()->Autorise() && (Reseau == NULL || curGareLigne->Ligne()->getReseau() == Reseau) && curGareLigne->Origine()->ArretLogique() == Origine)
  {
   cElementTrajet* curET = new cElementTrajet;
   synthese::time::DateTime MomentArriveeNew;
   MomentArriveeNew = MomentArrivee;
   curGareLigne->CalculeArrivee(*curGareLigne->Origine(), curGareLigne->Origine()->Prochain(MomentArriveeNew, MomentArrivee), MomentArrivee, MomentArriveeNew);
   curET->setInformations(curGareLigne, curGareLigne->Origine(), MomentArrivee, MomentArriveeNew);
   return(curET);
  }
 return(NULL);
}
*/ 
/*
cGareLigne* LogicalPlace::DessertAuDepart(const cLigne* Ligne) const
{
 // Recherche peut etre amelioree en utilisant Reseau puis Axe
 for (cGareLigne* curGLD = _firstDepartureLineStop; curGLD != NULL; curGLD = curGLD->PADepartSuivant())
  if (curGLD->Ligne() == Ligne)
   return(curGLD);
 return(NULL);
}
 
cGareLigne* LogicalPlace::DessertALArrivee(const cLigne* Ligne) const
{
 // Recherche peut etre amelioree en utilisant Reseau puis Axe
 for (cGareLigne* curGLA = _firstArrivalLineStop; curGLA != NULL; curGLA = curGLA->PAArriveeSuivante())
  if (curGLA->Ligne() == Ligne)
   return(curGLA);
 return(NULL);
}
*/

/** Edition de VMax.
 @author Hugues Romain
 @date 2001
*/ 
/*bool LogicalPlace::setVMax(tCategorieDistance CategorieDistance, int newVitesseKMH)
{
 //vVitesseMax[CategorieDistance] = newVitesseKMH+1; 
 return(true);
}
*/

/*! Modificateur d�signation courte pour indicateur papier.
 \param newDesignationOD Designation courte de l'arret
 \author Hugues Romain
 \date 2001-2005
*/
void LogicalPlace::setDesignationOD( const string& newDesignationOD )
{
    _nameAsDestinationForTimetable = newDesignationOD;
}

/*
bool LogicalPlace::addService(char newType, cPhoto* newPhoto, const std::string& newDesignation)
{
 // SET PORTAGE LINUX
 size_t i;
 for (i=0; vService[i]!=NULL; i++)
 { }
 //END PORTAGE LINUX
 vService[i] = new cServiceEnGare(newType, newPhoto, newDesignation);
 return(true);
}
*/




/** Modificateur d�signation de 13 caracteres.
    @param __Designation13 Valeur a donner a la designation de 13 caracteres.En cas de chaine de longueur trop importante, la valeur est tronquee.
    return true si la copie a ete effectuee integralement, false si elle a ete tronquee.
*/
bool LogicalPlace::setDesignation13( const string& __Designation13 )
{
    _name13 = __Designation13.substr( 0, 13 );
    return _name13 == __Designation13;
}



/** Modificateur d�signation de 26 caracteres.
    @param __Designation26 Valeur a donner a la designation de 26 caracteres.En cas de chaine de longueur trop importante, la valeur est tronquee.
    return true si la copie a ete effectuee integralement, false si elle a ete tronquee.
*/
bool LogicalPlace::setDesignation26( const string& __Designation26 )
{
    _name26 = __Designation26.substr( 0, 26 );
    return _name26 == __Designation26;
}



/** Accesseur d�signation 13 caract�res.
    @return D�signation de 13 caract�res du point d'arr�t.
*/
const string& LogicalPlace::getDesignation13() const
{
    return _name13;
}



/** Accesseur d�signation 26 caract�res.
    @return D�signation de 26 caract�res du point d'arr�t.
*/
const string& LogicalPlace::getDesignation26() const
{
    return _name26;
}


/** Accesseur alerte en lecture.
*/
const cAlerte& LogicalPlace::getAlerte() const
{
    return _alert;
}



/** Accesseur alerte en �criture.
*/
cAlerte& LogicalPlace::getAlertForSettings()
{
    return _alert;
}





cArretPhysique* LogicalPlace::GetArretPhysique( int n ) const
{
    if ( n < 1 || n > ( int ) _networkAccessPoints.size() )
        return NULL;
    else
        return ( cArretPhysique* ) getNetworkAccessPoint( n );
}




int LogicalPlace::Index() const
{
    return _id;
}



const string& LogicalPlace::getDesignationOD() const
{
    return _nameAsDestinationForTimetable;
}



LogicalPlace::tNiveauCorrespondance LogicalPlace::CorrespondanceAutorisee() const
{
    return _transferRules;
}

int LogicalPlace::AttenteCorrespondance( size_t Dep, size_t Arr ) const
{
    return _transferRules ? _transferDelay[ Dep ][ Arr ] : int( 0 );
}

/*
bool LogicalPlace::declServices(size_t newNombreServices)
{
 vService = (cServiceEnGare**) calloc(newNombreServices+1, sizeof(cPhoto));
 return(true);
}
*/

/*
inline bool LogicalPlace::addPhoto(cPhoto* Photo)
{
 // SET PORTAGE LINUX
 size_t i;
 for (i=0; vPhoto[i]!=NULL; i++)
 { }
 //END PORTAGE LINUX
 vPhoto[i] = Photo;
 return(true);
 
}
*/ 
/*
int LogicalPlace::vitesseMax(size_t Categorie) const
{
 return(vVitesseMax[Categorie]);
}
*/ 
/*
inline const cPhoto* LogicalPlace::getPhoto(int iNumeroPhoto) const
{
 return(vPhoto[iNumeroPhoto]);
}
 
inline cServiceEnGare* LogicalPlace::GetService(int i) const
{
 if (vService)
  return(vService[i]);
 else
  return(NULL);
}
 
inline cPhoto* cServiceEnGare::getPhoto() const
{
 return(vPhoto);
}
 
inline const std::string& cServiceEnGare::Designation() const
{
 return(vDesignation);
}
*/


/** Affichage texte de la gare sur un flux (pour debug).
*/
template <class T>
T& operator<<( T& flux, const LogicalPlace& Obj )
{
    flux << Obj.getTown() ->getName() << " " << _name;
    return flux;
}


/** Cr�ation d'un lieu sp�cifique pr�cis sur un lieu route enti�re � partir d'un num�ro d'adresse.
 @param addressNumber Num�ro d'adresse (vide/INCONNU = pas de num�ro fourni)
 @return Pointeur vers le lieu le plus pr�cis possible :
  - rue entiere si impossible de pr�ciser (this non jetable)
  - pointeur vers un lieu cr�� et contenant l'adresse pr�cise, avec l'attribut _Jetable � true.
  - NULL si num�ro d'adresse identifi� comme invalide par les donn�es de la rues si disponibles
 
 @warning V�rifier la valeur _volatile du lieu retourn�. Si elle est � true, alors il faut d�truire l'objet apr�s utilisation sous peine de fuites m�moires.
 
 La cr�ation d'un lieu sp�cifique est effectu�e si et seulement si :
  - un num�ro d'adresse non inconnu est fourni
  - la route du lieu courant a les capacit�s de d�terminer la position d�sign�e par le num�ro d'adresse
  - le num�ro d'adresse appartient bel et bien � la plage d'adresses de la rue
 
 @todo MJ Impl�mentation � terminer
*/
LogicalPlace* LogicalPlace::accurateAddressLogicalPlace( synmap::Address::AddressNumber addressNumber )
{
    if ( false )  // Clause Calcul d'une adresse pr�cise possible � impl�menter
    {

        // TODO : verifier l'argument rang ici
        return new LogicalPlace( new synmap::Address( this, 0, _road, addressNumber ) );
    }

    // Retour par d�faut
    return this;
}


/** Ajout d'acc�s � r�seau.
 @param networkAccessPoint Adresse � ajouter
 @return le rang de l'�l�ment ajout� (INCONNU si erreur)
 
 Cette m�thode effectue les actions suivantes :
*/
int LogicalPlace::addNetworkAccessPoint( NetworkAccessPoint* networkAccessPoint, int id )
{
    /** - Insertion de l'�l�ment */
    try
    {
        if ( id == INCONNU )
        {
            _networkAccessPoints.push_back( networkAccessPoint );
            id = ( int ) _networkAccessPoints.size() - 1;
        }
        else
            _networkAccessPoints[ id ] = networkAccessPoint;
    }
    catch ( ... )
    {
        return INCONNU;
    }

    /** - Initialisation de la matrice de correspondances pour le nouvel arriv� � la valeur inconnu */
    if ( CorrespondanceAutorisee() != CorrInterdite )
    {
        //  _maxTransferDelay[id] = 0;
        for ( size_t i = 0; i < _networkAccessPoints.size(); i++ )
        {
            setDelaiCorrespondance( ( int ) i, id, UNKNOWN_TRANSFER_DELAY );
            setDelaiCorrespondance( id, ( int ) i, UNKNOWN_TRANSFER_DELAY );
        }
        setDelaiCorrespondance( id, id, FORBIDDEN_TRANSFER_DELAY );
    }

    /** - Gestion des coordonn�es g�ographiques */
    if ( !_networkAccessPoints[ id ] ->unknownLocation() )
    {
        int avgX = 0;
        int avgY = 0;
        int nbPoints = 0;
        for ( size_t i = 0; i <= _networkAccessPoints.size(); i++ )
        {
            if ( _networkAccessPoints[ i ] != NULL && !_networkAccessPoints[ i ] ->unknownLocation() )
            {
                avgX += _networkAccessPoints[ i ] ->XKMM();
                avgY += _networkAccessPoints[ i ] ->YKMM();
                nbPoints++;
            }
        }
        setX( ( CoordonneeKMM ) ( avgX / nbPoints ) );
        setY( ( CoordonneeKMM ) ( avgY / nbPoints ) );
    }

    return id;
}


/** Modificateur nom du lieu logique.
 @param town Commune
 @param name Nom
*/
void LogicalPlace::setDesignation( cCommune* town, const string& name )
{
    _town = town;
    _name = name;

    // G�n�ration automatique de Designation OD si besoin
    if ( !_nameAsDestinationForTimetable.size() )
    {
        setDesignationOD( getTown() ->getName() + " " + getName() );
    }

    // Enregistrement sur la commune
    _town->addLogicalPlace( this );

    // Lieu logique
    if ( _town->getAllPlaces() )
        _aliasedLogicalPlaces.push_back( _town->getAllPlaces() );

}


/** Ajout de lieu logique inclus/alis�.
 @param logicalPlace Lieu logique � inclure/aliaser
*/
void LogicalPlace::addAliasedLogicalPlace( LogicalPlace* logicalPlace )
{
    _aliasedLogicalPlaces.push_back( logicalPlace );
}


/** Accesseur arr�ts physiques.
 
*/
LogicalPlace::PhysicalStopsMap LogicalPlace::getPhysicalStops() const
{
    LogicalPlace::PhysicalStopsMap result;
    cArretPhysique* physicalStop;

    for ( size_t i = 0; i < _networkAccessPoints.size(); i++ )
    {
        if ( physicalStop = dynamic_cast<cArretPhysique*>( _networkAccessPoints[ i ] ) )
            result[ i ] = ( cArretPhysique* ) _networkAccessPoints[ i ];
    }
    return result;
}


/** Accesseur adresses.
*/
LogicalPlace::AddressesMap LogicalPlace::getAddresses() const
{
    LogicalPlace::AddressesMap result;
    synmap::Address* address;

    for ( size_t i = 0; i < _networkAccessPoints.size(); i++ )
    {
        if ( address = dynamic_cast<synmap::Address*>( _networkAccessPoints[ i ] ) )
            result[ i ] = ( synmap::Address* ) _networkAccessPoints[ i ];
    }
    return result;
}

const string& LogicalPlace::getName() const
{
    return _name;
}


