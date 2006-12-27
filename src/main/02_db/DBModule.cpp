#include "02_db/DBModule.h"


#include "00_tcp/TcpService.h"


#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteThreadExec.h"

#include "01_util/Log.h"
#include "01_util/Thread.h"
#include "01_util/ThreadManager.h"

#include <iostream>

using namespace synthese::util;


namespace synthese
{
	namespace db
	{


	    SQLiteQueueThreadExec* DBModule::_sqliteQueueThreadExec = 0;


	    void 
	    DBModule::initialize ()
	    {
		int sqliteServicePort = 3592;

		// Initialize permanent ram loaded data
		Log::GetInstance().info("Loading live data...");
		_sqliteQueueThreadExec = new SQLiteQueueThreadExec (_databasePath);

		ThreadSPtr sqliteQueueThread (new Thread (_sqliteQueueThreadExec, "sqlite_queue"));
		ThreadManager::Instance ()->addThread (sqliteQueueThread);

		sqliteQueueThread->start ();

		SQLiteSync* syncHook = new SQLiteSync ();

		// Register all table syncs
		for (Factory<SQLiteTableSync>::Iterator it = 
			 Factory<SQLiteTableSync>::begin(); 
		     it != Factory<SQLiteTableSync>::end(); 
		     ++it)
		{
		    syncHook->addTableSynchronizer(it.getKey(), it.getObject());
		}
			
		_sqliteQueueThreadExec->registerUpdateHook (syncHook);
		sqliteQueueThread->waitForReadyState ();

		synthese::tcp::TcpService* service = 
		    synthese::tcp::TcpService::openService (sqliteServicePort);

		// Just one thread
		SQLiteThreadExec* sqliteThreadExec = 
		    new SQLiteThreadExec (service);
		
		ThreadSPtr sqliteThread (new Thread (sqliteThreadExec, "sqlite_tcp"));
		ThreadManager::Instance ()->addThread (sqliteThread);
		
		sqliteThread->start ();

		sqliteThread->waitForReadyState ();
		
	    }



	    SQLiteQueueThreadExec*
	    DBModule::GetSQLite ()
	    {
		return _sqliteQueueThreadExec;
	    }

	}
}


