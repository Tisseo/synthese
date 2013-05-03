////////////////////////////////////////////////////////////////////////////////
///	DataSource table synchronizer class implementation.
///	@file DataSourceTableSync.cpp
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "DataSourceTableSync.h"
#include "SelectQuery.hpp"
#include "ReplaceQuery.h"
#include "CoordinatesSystem.hpp"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;

	template<> const string util::FactorableTemplate<DBTableSync,DataSourceTableSync>::FACTORY_KEY(
		"16.01 DataSource"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DataSourceTableSync>::TABLE(
			"t059_data_sources"
		);

		template<> const Field DBTableSyncTemplate<DataSourceTableSync>::_FIELDS[] = { Field() }; // Defined by the record



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DataSourceTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<DataSourceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level
			return true;
		}



		template<> void DBTableSyncTemplate<DataSourceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DataSourceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DataSourceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace impex
	{
		DataSourceTableSync::SearchResult DataSourceTableSync::Search(
			Env& env,
			string name,
			int first /*= 0*/
			, optional<size_t> number /*= 0*/
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<DataSourceTableSync> query;
			if (!name.empty())
			{
				query.addWhereField(
					Name::FIELD.name,
					"%"+ lexical_cast<string>(name) + "%",
					ComposedExpression::OP_LIKE
				);
			}
			if (orderByName)
			{
				query.addOrderField(Name::FIELD.name, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
