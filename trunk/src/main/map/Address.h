/** En-t�te classe adresse.
 @file Address.h
*/


#ifndef SYNTHESE_MAP_ADDRESS_H_
#define SYNTHESE_MAP_ADDRESS_H_

#include <vector>
#include <set>


namespace synmap
{


class PhysicalStop;
class Road;
class RoadChunk;


/** Adresse (rue + position m�trique)
 
 L'adresse est un positionnement sur une rue d�termin� par une position m�trique de puis l'origine de la rue.
 
 Le num�ro d'adresse est une donn�e de d�signation de l'adresse, mais peut �galement suffire � d�terminer la position d�s lors que la rue poss�de les informations n�cessaires.
*/
class Address
{
public:

	typedef enum { RIGHT_SIDE, LEFT_SIDE, UNKNOWN_SIDE } AddressNumberSide;

	/** Num�ro d'adresse.
		Ex : 70 rue de Paris : le num�ro d'adresse est 70.
	*/
	typedef double AddressNumber;

	/** Constante num�ro d'adresse inconnu.
	*/
	static const AddressNumber UNKNOWN_ADDRESS_NUMBER;

	


    /** Chemin d'acc�s aux arr�ts physiques depuis l'adresse */
    typedef std::pair< std::vector<const RoadChunk*>, const PhysicalStop*> PathToPhysicalStop;

protected:

    //! @name Localisation
    //@{
    Road* const      _road;    //!< Route sur laquelle se trouve l'adresse
    const AddressNumber _number; //!< Num�ro de l'adresse
    double       _metricOffset;  //!< Point m�trique de l'adresse sur la rue
    //@}


public:

    //! @name Constructeur et destructeur
    //@{
    Address(Road* road, AddressNumber number = UNKNOWN_ADDRESS_NUMBER);
    ~Address();
    //@}

    //! @name Calculateurs
    //@{
    std::set
      < std::pair<double, const PhysicalStop*> >
      findDistancesToPhysicalStops (double maxDistance) const;

    std::set
        < PathToPhysicalStop >
        findPathsToPhysicalStops (double maxDistance) const;

    //@}

    //! @name Accesseurs
    //@{
    Road* getRoad() const
    {
        return _road;
    }




    Address::AddressNumber getAddressNumber() const
    {
        return _number;
    }
    //@}
};

}

#endif
