
/** PhysicalStopTableSync class implementation.
    @file PhysicalStopTableSync.cpp

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

#include "PhysicalStopTableSync.h"

#include "Conversion.h"
#include "SQLiteResult.h"
#include "SQLite.h"

#include "ConnectionPlaceTableSync.h"

#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace db;
    using namespace env;
    using namespace util;

	template<> const string util::FactorableTemplate<SQLiteTableSync,PhysicalStopTableSync>::FACTORY_KEY("15.55.01 Physical stops");

    namespace db
    {
		template<> const std::string SQLiteTableSyncTemplate<PhysicalStopTableSync>::TABLE_NAME = "t012_physical_stops";
		template<> const int SQLiteTableSyncTemplate<PhysicalStopTableSync>::TABLE_ID = 12;
		template<> const bool SQLiteTableSyncTemplate<PhysicalStopTableSync>::HAS_AUTO_INCREMENT = true;

		/** Does not update the place */
		template<> void SQLiteDirectTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::Load(
			PhysicalStop* object,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( PhysicalStopTableSync::COL_NAME));
			object->setXY (rows->getDouble ( PhysicalStopTableSync::COL_X), rows->getDouble ( PhysicalStopTableSync::COL_Y));
			object->setOperatorCode(rows->getText ( PhysicalStopTableSync::COL_OPERATOR_CODE));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				PublicTransportStopZoneConnectionPlace* place = ConnectionPlaceTableSync::GetEditable(rows->getLongLong (PhysicalStopTableSync::COL_PLACEID), env, linkLevel).get();
				object->setPlace(place);

				place->addPhysicalStop(object);
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::Unlink(
			PhysicalStop* obj,
			Env* env
			)
		{
			PublicTransportStopZoneConnectionPlace* place = const_cast<PublicTransportStopZoneConnectionPlace*>(obj->getConnectionPlace());
/// @todo	place->removePhysicalStop(obj);

			obj->setPlace(NULL);
		}

		template<> void SQLiteDirectTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>::Save(PhysicalStop* object)
		{
			/// @todo Implementation
		}
    }

    namespace env
    {
		const std::string PhysicalStopTableSync::COL_NAME = "name";
		const std::string PhysicalStopTableSync::COL_PLACEID = "place_id";
		const std::string PhysicalStopTableSync::COL_X = "x";
		const std::string PhysicalStopTableSync::COL_Y = "y";
		const string PhysicalStopTableSync::COL_OPERATOR_CODE("operator_code");



		PhysicalStopTableSync::PhysicalStopTableSync ()
			: SQLiteRegistryTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>()
		{
			addTableColumn (TABLE_COL_ID, "INTEGER", true);
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_PLACEID, "INTEGER", false);
			addTableColumn (COL_X, "DOUBLE", true);
			addTableColumn (COL_Y, "DOUBLE", true);
			addTableColumn(COL_OPERATOR_CODE, "TEXT", true);

			addTableIndex(COL_PLACEID);
		}



		PhysicalStopTableSync::~PhysicalStopTableSync ()
		{

		}



		void PhysicalStopTableSync::Search(
			Env& env, 
			uid placeId /*= UNKNOWN_VALUE */,
			int first /*= 0 */,
			int number /*= 0 */,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (placeId != UNKNOWN_VALUE)
				query << " AND " << COL_PLACEID << "=" << placeId;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
