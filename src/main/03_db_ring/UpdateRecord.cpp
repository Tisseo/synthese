#include "03_db_ring/UpdateRecord.h"
#include "01_util/Conversion.h"
#include "01_util/iostreams/Compression.h"

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/restrict.hpp>

using namespace synthese::util;
using namespace boost::posix_time;


namespace synthese
{

namespace dbring
{




UpdateRecord::UpdateRecord ()
    : _timestamp (min_date_time)
    , _emitterNodeId (-1)
    , _state (PENDING)
    , _compressedSQL ("0#")
{

}
 



UpdateRecord::UpdateRecord (const uid& key,
			    const boost::posix_time::ptime& timestamp, 
			    const NodeId& emitterNodeId,
			    const RecordState& state,
			    const std::string& compressedSQL)
    : _key (key)
    , _timestamp (timestamp)
    , _emitterNodeId (emitterNodeId)
    , _state (state)
    , _compressedSQL ()
{
    _compressedSQL.assign (compressedSQL);
}





UpdateRecord::~UpdateRecord ()
{
    
}



void 
UpdateRecord::setCompressedSQL (const std::string& compressedSQL) 
{ 
    _compressedSQL.assign (compressedSQL);
}




std::ostream& 
operator<< ( std::ostream& os, const UpdateRecord& op )
{
    os << op._key << " " << to_iso_string (op._timestamp) << " " << op._emitterNodeId 
       << " " << ((int) op._state) << " ";
    if (op.hasCompressedSQL ())
    {
	os.write (op.getCompressedSQL ().data (), op._compressedSQL.size ());
    }
    else
    {
	os << "0#";
    }
    return os;
}




std::istream& 
operator>> ( std::istream& is, UpdateRecord& op )
{
    char buf[64];

    is.getline (buf, sizeof (buf), ' ');
    op._key = Conversion::ToLongLong (buf);

    is.getline (buf, sizeof (buf), ' ');
    op._timestamp = from_iso_string (buf);

    //std::cerr << "..got timestamp " << to_iso_string (op._timestamp) << std::endl;

    is.getline (buf, sizeof (buf), ' ');
    op._emitterNodeId = Conversion::ToInt (buf);

    //std::cerr << "..got emmiter " << op._emitterNodeId << std::endl;

    is.getline (buf, sizeof (buf), ' ');
    op._state = (RecordState) Conversion::ToInt (std::string (buf));
    
    //std::cerr << "..got state " << buf << " " << op._state << std::endl;

    int size (0);
    is.getline (buf, sizeof (buf), '#');
    size = Conversion::ToInt (std::string (buf));

    //std::cerr << "..got compressed size " << size << std::endl;
    
    std::stringstream ss;
    ss << size << "#";

    boost::iostreams::copy (boost::iostreams::restrict (is, 0, size), ss);

    // std::cerr << "..got compressed data" << std::endl;

    op._compressedSQL.assign (ss.str ());

    return is;
}




bool 
UpdateRecord::hasCompressedSQL () const 
{ 
    return (_compressedSQL.size () > 2) && (_compressedSQL != "0#"); 
}




const std::string& 
UpdateRecord::getCompressedSQL () const 
{ 
    assert (hasCompressedSQL ());
    return _compressedSQL; 
}



bool 
operator== (const UpdateRecord& op1, const UpdateRecord& op2)
{
    return 
	(op1.getKey () == op2.getKey ()) &&
	(op1.getTimestamp () == op2.getTimestamp ()) &&
	(op1.getEmitterNodeId () == op2.getEmitterNodeId ()) &&
	(op1.getState () == op2.getState ()) &&
	(op1.getCompressedSQL () == op2.getCompressedSQL ());

}


bool 
operator!= (const UpdateRecord& op1, const UpdateRecord& op2)
{
    return !(op1 == op2); 
}



std::ostream& 
operator<< ( std::ostream& os, const UpdateRecordSet& op )
{
    os << op.size () << " " << std::flush;

    for (UpdateRecordSet::const_iterator it = op.begin ();
	 it != op.end (); ++it)
    {
	os << *(*it) << std::flush; 
    }
    return os;
}



std::istream& 
operator>> ( std::istream& is, UpdateRecordSet& op )
{
    char buf[8];
    is.getline (buf, sizeof (buf), ' ');
    int nbRecords (Conversion::ToInt (buf));
    for (int i=0; i<nbRecords; ++i)
    {
	UpdateRecord* updateRecord = new UpdateRecord ();
	is >> (*updateRecord);
	op.insert (UpdateRecordSPtr(updateRecord));
    }

    return is;

}






}
}
