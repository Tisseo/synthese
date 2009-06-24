
////////////////////////////////////////////////////////////////////////////////
/// DbModuleConfigTableSync class header.
///	@file DbModuleConfigTableSync.h
///	@author Hugues Romain
///	@date 2008-12-14 23:54
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////


#ifndef SYNTHESE_DB_DBMODULECONFIGTABLESYNC_H
#define SYNTHESE_DB_DBMODULECONFIGTABLESYNC_H


#include <string>
#include <iostream>

#include "SQLiteTableSyncTemplate.h"
#include "SQLiteResult.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace db
	{
		class SQLite;
	}
	
	namespace server
	{
		/** DbModuleConfig SQLite table synchronizer.
			@ingroup m15LS refLS
		*/
		class DbModuleConfigTableSync:
			public db::SQLiteTableSyncTemplate<DbModuleConfigTableSync>
		{
		 public:

			static const std::string COL_PARAMNAME;
			static const std::string COL_PARAMVALUE;


			DbModuleConfigTableSync ();
			~DbModuleConfigTableSync ();


			 virtual const std::string& getTableName() const;
			void rowsAdded(
				db::SQLite* sqlite,
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows,
				bool isFirstSync = false
			);

			void rowsUpdated(
				db::SQLite* sqlite,
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows
			);

			void rowsRemoved(
				db::SQLite* sqlite,
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows
			);
		};
	}
}

#endif
