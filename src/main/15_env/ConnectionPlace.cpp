#include "ConnectionPlace.h"

#include "04_time/DateTime.h"

#include "15_env/Alarm.h"
#include "15_env/PhysicalStop.h"
#include "15_env/Vertex.h"


#include <limits>
#include <assert.h>

namespace synthese
{
namespace env
{


const int ConnectionPlace::UNKNOWN_TRANSFER_DELAY = -1;
const int ConnectionPlace::FORBIDDEN_TRANSFER_DELAY = std::numeric_limits<int>::max ();




ConnectionPlace::ConnectionPlace (const uid& id,
				  const std::string& name,
				  const City* city,
				  const ConnectionType& connectionType,
				  int defaultTransferDelay)
    : synthese::util::Registrable<uid,ConnectionPlace> (id)
    , AddressablePlace (name, city)
    , _connectionType (connectionType)
    , _defaultTransferDelay (defaultTransferDelay)
    , _minTransferDelay (std::numeric_limits<int>::max ())
    , _maxTransferDelay (0)
    , _alarm (0)
{
}




ConnectionPlace::~ConnectionPlace ()
{
}





int 
ConnectionPlace::getDefaultTransferDelay () const
{
    return _defaultTransferDelay;
}





int 
ConnectionPlace::getMinTransferDelay () const
{
    return _minTransferDelay;
}


int 
ConnectionPlace::getMaxTransferDelay () const
{
    return _maxTransferDelay;
}






void 
ConnectionPlace::setDefaultTransferDelay (int defaultTransferDelay)
{
    _defaultTransferDelay = defaultTransferDelay;
}




const ConnectionPlace::ConnectionType& 
ConnectionPlace::getConnectionType () const
{
    return _connectionType;
}



void 
ConnectionPlace::setConnectionType (const ConnectionType& connectionType)
{
    _connectionType = connectionType;
}



bool 
ConnectionPlace::isConnectionAuthorized () const
{
    return _connectionType != CONNECTION_TYPE_FORBIDDEN;
}



bool 
ConnectionPlace::isConnectionRoadOnly () const
{
    return _connectionType != CONNECTION_TYPE_ROADONLY;
}



int 
ConnectionPlace::getTransferDelay (int departureRank, int arrivalRank) const
{
    if (_connectionType == CONNECTION_TYPE_FORBIDDEN) return FORBIDDEN_TRANSFER_DELAY;
    std::map< std::pair<int, int>, int >::const_iterator iter = 
	_transferDelays.find (std::make_pair (departureRank, arrivalRank));
    
    // If not defined in map, return default transfer delay
    if (iter == _transferDelays.end ()) return _defaultTransferDelay;
    return iter->second;
}
 


void 
ConnectionPlace::addTransferDelay (int departureRank, int arrivalRank, int transferDelay)
{
    _transferDelays[std::make_pair (departureRank, arrivalRank)] = transferDelay;
    if (transferDelay < _minTransferDelay)
    {
	_minTransferDelay = transferDelay;
    }
    if (transferDelay < _maxTransferDelay)
    {
	_maxTransferDelay = transferDelay;
    }
}

    

void 
ConnectionPlace::clearTransferDelays ()
{
    _transferDelays.clear ();
}




void 
ConnectionPlace::addPhysicalStop (const PhysicalStop* physicalStop)
{
    _physicalStops.push_back (physicalStop);
}



bool 
ConnectionPlace::hasApplicableAlarm (const synthese::time::DateTime& start, 
				     const synthese::time::DateTime& end) const
{
    if (_alarm == 0) return false;
    return _alarm->isApplicable (start, end);
}



const Alarm* 
ConnectionPlace::getAlarm () const
{
    return _alarm;
}




void 
ConnectionPlace::setAlarm (const Alarm* alarm)
{
    _alarm = alarm;
}






VertexAccess 
ConnectionPlace::getVertexAccess (const AccessDirection& accessDirection,
				  const AccessParameters& accessParameters,
				  const Vertex* destination,
				  const Vertex* origin) const
{
    VertexAccess access;
    access.path.push_back (destination);

    if (origin != 0)
    {
	if (accessDirection == FROM_ORIGIN)
	{
	    access.approachTime = getTransferDelay (
		origin->getRankInConnectionPlace (),
		destination->getRankInConnectionPlace ());
	} 
	else
	{
	    access.approachTime = getTransferDelay (
		destination->getRankInConnectionPlace (),
		origin->getRankInConnectionPlace ());
	}
    }
    else
    {
	access.approachDistance = 0;
	access.approachTime = 0;
    }

    return access;
}
    






void
ConnectionPlace::getImmediateVertices (VertexAccessMap& result, 
				       const AccessDirection& accessDirection,
				       const AccessParameters& accessParameters,
				       const Vertex* origin,
				       bool returnAddresses,
				       bool returnPhysicalStops) const
{
    AddressablePlace::getImmediateVertices (result, accessDirection, accessParameters,
					    origin, returnAddresses, returnPhysicalStops);
    
    if (returnPhysicalStops)
    {
	for (std::vector<const PhysicalStop*>::const_iterator it = _physicalStops.begin ();
	     it != _physicalStops.end (); ++it)
	{
	    if (origin == (*it)) continue;
	    result.insert ((*it), getVertexAccess (accessDirection,
						   accessParameters,
						   (*it), origin));
	}
    }
}




    







}
}
