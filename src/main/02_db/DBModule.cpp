
/** DBModule class implementation.
	@file DBModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "00_tcp/TcpService.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteThreadExec.h"

#include "01_util/Log.h"
#include "01_util/ManagedThread.h"


#include <iostream>

using namespace synthese::util;

namespace synthese
{
	namespace db
	{
	    SQLiteQueueThreadExec* DBModule::_sqliteQueueThreadExec = 0;

	    void DBModule::initialize()
	    {
			int sqliteServicePort = 3592;

			// Initialize permanent ram loaded data
			_sqliteQueueThreadExec = new SQLiteQueueThreadExec (_databasePath);

			ManagedThread* sqliteQueueThread = 
			    new ManagedThread (_sqliteQueueThreadExec, "sqlite_queue", 1);

			// sqliteQueueThread->start ();

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
			// sqliteQueueThread->waitForReadyState ();

			synthese::tcp::TcpService* service = 
				synthese::tcp::TcpService::openService (sqliteServicePort);

			// Just one thread
			SQLiteThreadExec* sqliteThreadExec = new SQLiteThreadExec (service);
			
			ManagedThread* sqliteThread = 
			    new ManagedThread (sqliteThreadExec, "sqlite_tcp");
			
			// sqliteThread->start ();

			// sqliteThread->waitForReadyState ();
		
	    }



	    SQLiteQueueThreadExec*
	    DBModule::GetSQLite ()
	    {
			return _sqliteQueueThreadExec;
	    }

	}
}


