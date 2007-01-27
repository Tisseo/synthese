#include "Environment.h"


using synthese::lexmatcher::LexicalMatcher;


namespace synthese
{
	using namespace time;
namespace env
{


Environment::Environment (const uid& id)
    : synthese::util::Registrable<uid,Environment> (id)
    , _citiesMatcher ()
{
}



Environment::~Environment ()
{
    // Delete all objects registered in this environment.
    _addresses.clear ();
    _axes.clear ();
    _cities.clear ();
    _connectionPlaces.clear ();
    _documents.clear ();
    _lines.clear ();
    _lineStops.clear ();
    _physicalStops.clear ();
    _roadChunks.clear ();
    _roads.clear ();
    _continuousServices.clear ();
    _scheduledServices.clear ();
}




Address::Registry& 
Environment::getAddresses ()
{
    return _addresses;
}



const Address::Registry& 
Environment::getAddresses () const
{
    return _addresses;
}


Axis::Registry& 
Environment::getAxes ()
{
    return _axes;
}



const Axis::Registry& 
Environment::getAxes () const
{
    return _axes;
}



Date Environment::interpretDate( const std::string& text ) const
{
	Date tempDate;
	if ( !text.size () )
		tempDate.updateDate( TIME_CURRENT );

	else if ( text.size () == 1 )
		switch ( text[ 0 ] )
	{
		case TEMPS_MIN_CIRCULATIONS:
			tempDate = getMinDateInUse ();
			break;
		case TEMPS_MAX_CIRCULATIONS:
			tempDate = getMaxDateInUse ();
			break;
		default:
			tempDate.updateDate(text[ 0 ] );
	}
	else
		tempDate.FromString(text);
	return tempDate;
}




City::Registry&
Environment::getCities ()
{
    return _cities;
}



const City::Registry&
Environment::getCities () const
{
    return _cities;
}



Document::Registry&
Environment::getDocuments ()
{
    return _documents;
}


const Document::Registry&
Environment::getDocuments () const
{
    return _documents;
}



Fare::Registry&
Environment::getFares ()
{
    return _fares;
}


const Fare::Registry&
Environment::getFares () const
{
    return _fares;
}



Line::Registry&
Environment::getLines ()
{
    return _lines;
}


const Line::Registry&
Environment::getLines () const
{
    return _lines;
}


LineStop::Registry&
Environment::getLineStops ()
{
    return _lineStops;
}


const LineStop::Registry&
Environment::getLineStops () const
{
    return _lineStops;
}



ConnectionPlace::Registry&
Environment::getConnectionPlaces ()
{
    return _connectionPlaces;
}



const ConnectionPlace::Registry&
Environment::getConnectionPlaces () const
{
    return _connectionPlaces;
}



PhysicalStop::Registry&
Environment::getPhysicalStops ()
{
    return _physicalStops;
}



PedestrianCompliance::Registry& 
Environment::getPedestrianCompliances ()
{
    return _pedestrianCompliances;
}


const PedestrianCompliance::Registry& 
Environment::getPedestrianCompliances () const
{
    return _pedestrianCompliances;
}



HandicappedCompliance::Registry& 
Environment::getHandicappedCompliances ()
{
    return _handicappedCompliances;
}



const HandicappedCompliance::Registry& 
Environment::getHandicappedCompliances () const
{
    return _handicappedCompliances;
}



BikeCompliance::Registry& 
Environment::getBikeCompliances ()
{
    return _bikeCompliances;
}



const BikeCompliance::Registry& 
Environment::getBikeCompliances () const
{
    return _bikeCompliances;
}





const PhysicalStop::Registry&
Environment::getPhysicalStops () const
{
    return _physicalStops;
}


PlaceAlias::Registry& 
Environment::getPlaceAliases ()
{
    return _placeAliases;
}



const PlaceAlias::Registry& 
Environment::getPlaceAliases () const
{
    return _placeAliases;
}



PublicPlace::Registry& 
Environment::getPublicPlaces ()
{
    return _publicPlaces;
}



const PublicPlace::Registry& 
Environment::getPublicPlaces () const
{
    return _publicPlaces;
}






RoadChunk::Registry& 
Environment::getRoadChunks ()
{
    return _roadChunks;
}



const RoadChunk::Registry& 
Environment::getRoadChunks () const
{
    return _roadChunks;
}




Road::Registry& 
Environment::getRoads ()
{
    return _roads;
}



const Road::Registry& 
Environment::getRoads () const
{
    return _roads;
}



ScheduledService::Registry& 
Environment::getScheduledServices ()
{
    return _scheduledServices;
}



const ScheduledService::Registry& 
Environment::getScheduledServices () const
{
    return _scheduledServices;
}



ContinuousService::Registry& 
Environment::getContinuousServices ()
{
    return _continuousServices;
}



const ContinuousService::Registry& 
Environment::getContinuousServices () const
{
    return _continuousServices;
}


TransportNetwork::Registry& 
Environment::getTransportNetworks ()
{
    return _transportNetworks;
}



const TransportNetwork::Registry& 
Environment::getTransportNetworks () const
{
    return _transportNetworks;
}




ReservationRule::Registry& 
Environment::getReservationRules ()
{
    return _reservationRules;
}



const ReservationRule::Registry& 
Environment::getReservationRules () const
{
    return _reservationRules;
}




synthese::lexmatcher::LexicalMatcher<uid>& 
Environment::getCitiesMatcher ()
{
    return _citiesMatcher;
}




const synthese::lexmatcher::LexicalMatcher<uid>& 
Environment::getCitiesMatcher () const
{
    return _citiesMatcher;
}





const Place* 
Environment::fetchPlace (const uid& id) const
{
    const Place* place = fetchAddressablePlace (id);
    if (place == 0) place = fetchIncludingPlace (id);
    return place;
}




const AddressablePlace* 
Environment::fetchAddressablePlace (const uid& id) const
{
    if (_connectionPlaces.contains (id)) return _connectionPlaces.get (id);
    if (_publicPlaces.contains (id)) return _publicPlaces.get (id);
    if (_roads.contains (id)) return _roads.get (id);

    return 0;
}




const IncludingPlace* 
Environment::fetchIncludingPlace (const uid& id) const
{
    if (_placeAliases.contains (id)) return _placeAliases.get (id);
    if (_cities.contains (id)) return _cities.get (id);

    return 0;
}



const Path* 
Environment::fetchPath (const uid& id) const
{
    if (_lines.contains (id)) return _lines.get (id);
    if (_roads.contains (id)) return _roads.get (id);
    return 0;
}



Path* 
Environment::fetchPath (const uid& id)
{
    if (_lines.contains (id)) return _lines.get (id);
    if (_roads.contains (id)) return _roads.get (id);
    return 0;
}



const Service* 
Environment::fetchService (const uid& id) const
{
    if (_scheduledServices.contains (id)) return _scheduledServices.get (id);
    if (_continuousServices.contains (id)) return _continuousServices.get (id);
    return 0;
}



Service* 
Environment::fetchService (const uid& id)
{
    if (_scheduledServices.contains (id)) return _scheduledServices.get (id);
    if (_continuousServices.contains (id)) return _continuousServices.get (id);
    return 0;
}



const Vertex* 
Environment::fetchVertex (const uid& id) const
{
    if (_physicalStops.contains (id)) return _physicalStops.get (id);
    if (_addresses.contains (id)) return _addresses.get (id);
    return 0;
}



Vertex* 
Environment::fetchVertex (const uid& id)
{
    if (_physicalStops.contains (id)) return _physicalStops.get (id);
    if (_addresses.contains (id)) return _addresses.get (id);
    return 0;
}







Environment::CityList Environment::guessCity (const std::string& fuzzyName, int nbMatches) const
{
    CityList result;
    LexicalMatcher<uid>::MatchResult matches =  _citiesMatcher.bestMatches (fuzzyName, nbMatches);
    for (LexicalMatcher<uid>::MatchResult::iterator it = matches.begin ();
	 it != matches.end (); ++it)
    {
	uid id = it->value;
	result.push_back (getCities ().get (id));
    }
    return result;
}




const synthese::time::Date& 
Environment::getMinDateInUse () const
{
    return _minDateInUse;
}


const synthese::time::Date& 
Environment::getMaxDateInUse () const
{
    return _maxDateInUse;
}




void 
Environment::updateMinMaxDatesInUse (synthese::time::Date newDate, bool marked)
{
    if (marked)
    {
	if ( (_minDateInUse == synthese::time::Date::UNKNOWN_DATE) ||
	     (newDate < _minDateInUse) ) 
	{
	    _minDateInUse = newDate;
	}

	if ( (_maxDateInUse == synthese::time::Date::UNKNOWN_DATE) ||
	     (newDate > _maxDateInUse) ) 
	{
	    _maxDateInUse = newDate;
	}
    }
    else
    {
	// TODO not written yet...
    }
    
}





}
}


