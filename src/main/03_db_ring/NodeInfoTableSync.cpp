#include "03_db_ring/NodeInfoTableSync.h"

#include "02_db/DBModule.h"

#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/Node.h"

#include "01_util/Conversion.h"
#include "01_util/UId.h"

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>


#include <set>
#include <sstream>


using namespace boost::posix_time;


namespace synthese
{

    using namespace util;
    using namespace db;
    using namespace dbring;

    namespace db
    {
	template<> const std::string SQLiteTableSyncTemplate<NodeInfo>::TABLE_NAME = "t998_nodes_infos";
	template<> const int SQLiteTableSyncTemplate<NodeInfo>::TABLE_ID = 998;
	template<> const bool SQLiteTableSyncTemplate<NodeInfo>::HAS_AUTO_INCREMENT = true;

	template<> void SQLiteTableSyncTemplate<NodeInfo>::load (NodeInfo* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
	{
	    object->setNodeId (Conversion::ToInt (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_NODEID)));
	    object->setRingId (Conversion::ToInt (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_RINGID)));
	    object->setHost (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_HOST));
	    object->setPort (Conversion::ToInt (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_PORT)));
	    object->setAuthority (Conversion::ToBool (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_AUTH)));
	    object->setState ((NodeState) Conversion::ToInt (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_STATE)));
	    object->setClock (Conversion::ToLongLong (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_CLOCK)));
	    object->setLastPendingTimestamp (from_iso_string (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_LASTPENDINGTIMESTAMP)));
	    object->setLastAcknowledgedTimestamp (from_iso_string (rows.getColumn (rowId, NodeInfoTableSync::TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP)));
	}


	template<> void SQLiteTableSyncTemplate<NodeInfo>::save (NodeInfo* object)
	{
	    const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
	    std::stringstream query;

	    uid key = util::encodeUId (TABLE_ID, 
				 object->getRingId (),
				 object->getNodeId (),
				 object->getNodeId ());

	    query << "REPLACE INTO " << TABLE_NAME << " VALUES ("
		  << key << "," << object->getNodeId () << ","  << object->getRingId () << "," << Conversion::ToSQLiteString (object->getHost ()) << "," 
		  << object->getPort () << "," << object->isAuthority () << "," << object->getState () << "," << object->getClock () 
		  << "," << Conversion::ToSQLiteString (to_iso_string (object->getLastPendingTimestamp ()))    
		  << "," << Conversion::ToSQLiteString (to_iso_string (object->getLastAcknowledgedTimestamp ())) << ")";	    
	    
	    sqlite->execUpdate(query.str());
	}
    }


    namespace dbring
    {
	
	const std::string NodeInfoTableSync::TABLE_COL_NODEID ("nodeid");
	const std::string NodeInfoTableSync::TABLE_COL_RINGID ("ringid");
	const std::string NodeInfoTableSync::TABLE_COL_HOST ("host");
	const std::string NodeInfoTableSync::TABLE_COL_PORT ("port");
	const std::string NodeInfoTableSync::TABLE_COL_AUTH ("authority");
	const std::string NodeInfoTableSync::TABLE_COL_STATE ("state");
	const std::string NodeInfoTableSync::TABLE_COL_CLOCK ("clock");
	const std::string NodeInfoTableSync::TABLE_COL_LASTPENDINGTIMESTAMP ("last_pending_timestamp");
	const std::string NodeInfoTableSync::TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP ("last_acknowledged_timestamp");


	NodeInfoTableSync::NodeInfoTableSync ()
	    : SQLiteTableSyncTemplate<NodeInfo> (true, true, db::TRIGGERS_ENABLED_CLAUSE)
	{
	    // TODO : make host, port, auth updatables.
	    addTableColumn (TABLE_COL_ID, "INTEGER", false);
	    addTableColumn (TABLE_COL_NODEID, "INTEGER", false);
	    addTableColumn (TABLE_COL_RINGID, "INTEGER", false);
	    addTableColumn (TABLE_COL_HOST, "TEXT", false);
	    addTableColumn (TABLE_COL_PORT, "INTEGER", false);
	    addTableColumn (TABLE_COL_AUTH, "BOOLEAN", false);
	    addTableColumn (TABLE_COL_STATE, "INTEGER", true);
	    addTableColumn (TABLE_COL_CLOCK, "INTEGER", true);
	    addTableColumn (TABLE_COL_LASTPENDINGTIMESTAMP, "TEXT", true);
	    addTableColumn (TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP, "TEXT", true);
	    
	    std::vector<std::string> indexColumns;
	    indexColumns.push_back (TABLE_COL_NODEID);
	    indexColumns.push_back (TABLE_COL_RINGID);
	    addTableIndex(indexColumns);
	    addTableIndex(TABLE_COL_LASTPENDINGTIMESTAMP);
	    addTableIndex(TABLE_COL_LASTACKNOWLEDGEDTIMESTAMP);
	    
	}
	
	
	
	
	NodeInfoTableSync::~NodeInfoTableSync ()
	{
	}
    


	void 
	NodeInfoTableSync::rowsAdded (const SQLiteQueueThreadExec* sqlite, 
				      SQLiteSync* sync,
				      const SQLiteResult& rows, bool isFirstSync)
	{
	    rowsUpdated (sqlite, sync, rows);
	}
	
	

	void 
	NodeInfoTableSync::rowsUpdated (const SQLiteQueueThreadExec* sqlite, 
					SQLiteSync* sync,
					const SQLiteResult& rows)
	{
	    NodeInfo info;
	    std::set<int> processedRows;

	    // Check if there is info concerning this node. If there is, 
	    // process it first.
	    for (int i=0; i<rows.getNbRows (); ++i)
	    {
		load (&info, rows, i);
		if (info.getNodeId () == DbRingModule::GetNode ()->getId ())
		{
		    DbRingModule::GetNode ()->setNodeInfoCallback (info);
		    processedRows.insert (i);
		}
	    }
	    for (int i=0; i<rows.getNbRows (); ++i)
	    {
		if (processedRows.find (i) == processedRows.end ())
		{
		    load (&info, rows, i);
		    DbRingModule::GetNode ()->setNodeInfoCallback (info);
		}
	    }
	}
 

	void 
	NodeInfoTableSync::rowsRemoved (const SQLiteQueueThreadExec* sqlite, 
					SQLiteSync* sync,
					const SQLiteResult& rows)
	{

	}



}
}

