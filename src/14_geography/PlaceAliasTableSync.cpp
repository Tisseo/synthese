
/** PlaceAliasTableSync class implementation.
	@file PlaceAliasTableSync.cpp

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

#include <sstream>

#include "PlaceAliasTableSync.h"

#include "CityTableSync.h"
#include "Fetcher.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, PlaceAliasTableSync>::FACTORY_KEY("14.10.01 Places Alias");
	}
	
	namespace geography
	{
		const string PlaceAliasTableSync::COL_NAME ("name");
		const string PlaceAliasTableSync::COL_ALIASEDPLACEID ("aliased_place_id");
		const string PlaceAliasTableSync::COL_CITYID ("city_id");
		const string PlaceAliasTableSync::COL_ISCITYMAINCONNECTION ("is_city_main_connection");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<PlaceAliasTableSync>::TABLE(
			"t011_place_aliases"
			);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<PlaceAliasTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(PlaceAliasTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(PlaceAliasTableSync::COL_ALIASEDPLACEID, SQL_INTEGER, false),
			SQLiteTableSync::Field(PlaceAliasTableSync::COL_CITYID, SQL_INTEGER, false),
			SQLiteTableSync::Field(PlaceAliasTableSync::COL_ISCITYMAINCONNECTION, SQL_BOOLEAN),
			SQLiteTableSync::Field()
		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<PlaceAliasTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::Load(
			PlaceAlias* obj,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			obj->setName (rows->getText (PlaceAliasTableSync::COL_NAME));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Because the fetch place has no equivalent in database read mode
//				assert(temporary == GET_REGISTRY);

				uid aliasedPlaceId (
					rows->getLongLong (PlaceAliasTableSync::COL_ALIASEDPLACEID));
				uid cityId (
					rows->getLongLong (PlaceAliasTableSync::COL_CITYID));
				City* city(CityTableSync::GetEditable(cityId, env, linkLevel).get());

				obj->setCity(city);
				obj->setAliasedPlace(Fetcher<NamedPlace>::Fetch(aliasedPlaceId, env, linkLevel).get());


				bool isCityMainConnection (rows->getBool ( PlaceAliasTableSync::COL_ISCITYMAINCONNECTION));
				if (isCityMainConnection)
				{
					city->addIncludedPlace (obj);
				}
				city->addPlaceToMatcher<PlaceAlias>(obj);
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::Unlink(
			PlaceAlias* obj
		){
			City* city(const_cast<City*>(obj->getCity()));
			if (city != NULL)
			{
				city->removePlaceFromMatcher<PlaceAlias>(obj);
				obj->setCity(NULL);
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::Save(
			PlaceAlias* object,
			optional<SQLiteTransaction&> transaction
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str(), transaction);
		}

	}

	namespace geography
	{
		PlaceAliasTableSync::PlaceAliasTableSync()
			: SQLiteRegistryTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>()
		{
		}



		void PlaceAliasTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			/// @todo Fill Where criteria
			// if (!name.empty())
			// 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
				;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
