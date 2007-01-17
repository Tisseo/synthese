
/** InterfaceTableSync class implementation.
	@file InterfaceTableSync.cpp

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

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfaceModule.h"
#include "11_interfaces/InterfaceTableSync.h"

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace interfaces;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Interface>::TABLE_NAME = "t024_interfaces";
		template<> const int SQLiteTableSyncTemplate<Interface>::TABLE_ID = 24;
		template<> const bool SQLiteTableSyncTemplate<Interface>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Interface>::load(Interface* interf, const db::SQLiteResult& rows, int rowId)
		{
			interf->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			interf->setNoSessionDefaultPageCode(rows.getColumn(rowId, InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE));
		}

		template<> void SQLiteTableSyncTemplate<Interface>::save(Interface* interf)
		{
			/// @todo Implementation
		}
	}

	namespace interfaces
	{
		const std::string InterfaceTableSync::TABLE_COL_ID = "id";
		const std::string InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE = "no_session_default_page";

		InterfaceTableSync::InterfaceTableSync()
			: db::SQLiteTableSyncTemplate<Interface> ( TABLE_NAME, true, true, db::TRIGGERS_ENABLED_CLAUSE )
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NO_SESSION_DEFAULT_PAGE, "TEXT", true);
		}


		void InterfaceTableSync::rowsUpdated( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}


		void InterfaceTableSync::rowsAdded( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				Interface* obj = new Interface(Conversion::ToLongLong(rows.getColumn(i,TABLE_COL_ID)));
				load(obj, rows, i);
				InterfaceModule::getInterfaces().add(obj);
			}
		}


		void InterfaceTableSync::rowsRemoved( const SQLiteQueueThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}
	}
}



