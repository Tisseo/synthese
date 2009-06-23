
/** RoadTableSync class implementation.
	@file RoadTableSync.cpp

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

#include "RoadTableSync.h"
#include "RoadPlaceTableSync.h"
#include "CityTableSync.h"
#include "FareTableSync.h"
#include "RoadChunkTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, RoadTableSync>::FACTORY_KEY(
			"15.30.02 Roads"
		);
	}

	namespace road
	{
		const string RoadTableSync::COL_ROADTYPE ("road_type");
		const string RoadTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const string RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const string RoadTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const string RoadTableSync::COL_VIAPOINTS ("via_points");
		const string RoadTableSync::COL_ROAD_PLACE_ID("road_place_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<RoadTableSync>::TABLE(
				"t015_roads"
				);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<RoadTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadTableSync::COL_ROADTYPE, SQL_INTEGER),
			SQLiteTableSync::Field(RoadTableSync::COL_BIKECOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(RoadTableSync::COL_PEDESTRIANCOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(RoadTableSync::COL_VIAPOINTS, SQL_TEXT),
			SQLiteTableSync::Field(RoadTableSync::COL_ROAD_PLACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<RoadTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<RoadTableSync,Road>::Load(
			Road* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Type
			Road::RoadType roadType = (Road::RoadType) rows->getInt (RoadTableSync::COL_ROADTYPE);
			object->setType(roadType);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType roadPlaceId(rows->getLongLong(RoadTableSync::COL_ROAD_PLACE_ID));
				RoadPlace* roadPlace(RoadPlaceTableSync::GetEditable(roadPlaceId, env, linkLevel).get());
				object->setRoadPlace(roadPlace);
				roadPlace->addRoad(*object);
				
// 				uid bikeComplianceId (rows->getLongLong (RoadTableSync::COL_BIKECOMPLIANCEID));
// 				object->setBikeCompliance (BikeComplianceTableSync::Get (bikeComplianceId, env, linkLevel));

// 				uid handicappedComplianceId (rows->getLongLong (RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID));
// 				object->setHandicappedCompliance(HandicappedComplianceTableSync::Get(handicappedComplianceId, env, linkLevel));

// 				uid pedestrianComplianceId (rows->getLongLong (RoadTableSync::COL_PEDESTRIANCOMPLIANCEID));
// 				object->setPedestrianCompliance(PedestrianComplianceTableSync::Get(pedestrianComplianceId, env, linkLevel));
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<RoadTableSync,Road>::Unlink(
			Road* obj
		){
		}

		template<> void SQLiteDirectTableSyncTemplate<RoadTableSync,Road>::Save(Road* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query <<
				" REPLACE INTO " << TABLE.NAME << " VALUES(" <<
				object->getKey() << "," <<
				static_cast<int>(object->getType()) << "," <<
				"0,0,0,''," <<
				(object->getRoadPlace() ? object->getRoadPlace()->getKey() : RegistryKeyType(0)) <<
			")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace road
	{
		RoadTableSync::RoadTableSync()
			: SQLiteRegistryTableSyncTemplate<RoadTableSync,Road>()
		{
		}

		void RoadTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> roadPlaceId,
			boost::optional<util::RegistryKeyType> startingNodeId,
			boost::optional<util::RegistryKeyType> endingNodeId,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if(roadPlaceId)
			{
				query << " AND " << COL_ROAD_PLACE_ID << "=" << *roadPlaceId;
			}
			if(startingNodeId)
			{
				query << " AND (SELECT " << RoadChunkTableSync::COL_ADDRESSID << " FROM " << RoadChunkTableSync::TABLE.NAME << " WHERE " << RoadChunkTableSync::COL_ROADID << "=" << TABLE.NAME << "." << TABLE_COL_ID << " AND " << RoadChunkTableSync::COL_RANKINPATH << "=0)=" << *startingNodeId;
			}
			if(endingNodeId)
			{
				query << " AND (SELECT " << RoadChunkTableSync::COL_ADDRESSID << " FROM " << RoadChunkTableSync::TABLE.NAME << " WHERE " << RoadChunkTableSync::COL_ROADID << "=" << TABLE.NAME << "." << TABLE_COL_ID << " ORDER BY " << RoadChunkTableSync::COL_RANKINPATH << " DESC LIMIT 1)=" << *startingNodeId;
			}
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
