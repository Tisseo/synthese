
/** RoadChunkTableSync class implementation.
	@file RoadChunkTableSync.cpp

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

#include "RoadChunkTableSync.h"
#include "AddressTableSync.h"
#include "RoadTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "LinkException.h"

#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace geometry;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, RoadChunkTableSync>::FACTORY_KEY("34.30.01 Road chunks");
	}

	namespace road
	{
		const string RoadChunkTableSync::COL_ADDRESSID ("address_id");
		const string RoadChunkTableSync::COL_RANKINPATH ("rank_in_path");
		const string RoadChunkTableSync::COL_VIAPOINTS ("via_points");  // list of ids
		const string RoadChunkTableSync::COL_ROADID ("road_id");  // NU
		const string RoadChunkTableSync::COL_METRICOFFSET ("metric_offset");  // U ??
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<RoadChunkTableSync>::TABLE(
			"t014_road_chunks"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<RoadChunkTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_ADDRESSID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_RANKINPATH, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_VIAPOINTS, SQL_TEXT),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_ROADID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_METRICOFFSET, SQL_DOUBLE, false),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<RoadChunkTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				RoadChunkTableSync::COL_ROADID.c_str(),
				RoadChunkTableSync::COL_RANKINPATH.c_str(),
				""
			),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Load(
			RoadChunk* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    // Rank in road
		    int rankInRoad (rows->getInt (RoadChunkTableSync::COL_RANKINPATH));
		    object->setRankInPath(rankInRoad);
			
			// Metric offset
			object->setMetricOffset(rows->getDouble (RoadChunkTableSync::COL_METRICOFFSET));

		    // Via points
		    object->clearViaPoints ();
		    string viaPointsStr (rows->getText (RoadChunkTableSync::COL_VIAPOINTS));
		    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		    boost::char_separator<char> sep1 (",");
		    boost::char_separator<char> sep2 (":");
		    tokenizer viaPointsTokens (viaPointsStr, sep1);
		    for (tokenizer::iterator viaPointIter = viaPointsTokens.begin();
			 viaPointIter != viaPointsTokens.end (); ++viaPointIter)
		    {
				tokenizer valueTokens (*viaPointIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();
				
				// X:Y
				object->addViaPoint (Point2D (Conversion::ToDouble (*valueIter), 
						      Conversion::ToDouble (*(++valueIter))));
		    }

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					shared_ptr<Road> road(RoadTableSync::GetEditable(rows->getLongLong(RoadChunkTableSync::COL_ROADID), env, linkLevel));
					object->setRoad(road.get());
					Address* fromAddress(AddressTableSync::GetEditable (rows->getLongLong (RoadChunkTableSync::COL_ADDRESSID), env, linkLevel).get());
					object->setFromAddress(fromAddress);
					road->addRoadChunk(*object);
				}
				catch (ObjectNotFoundException<Road>& e)
				{
					throw LinkException<RoadChunkTableSync>(rows, RoadChunkTableSync::COL_ROADID, e);
				}
				catch (ObjectNotFoundException<Address>& e)
				{
					throw LinkException<RoadChunkTableSync>(rows, RoadChunkTableSync::COL_ADDRESSID, e);
				}
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Unlink(
			RoadChunk* obj
		){
		}

	    
	    template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Save(
			RoadChunk* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<RoadChunkTableSync> query(*object);
			query.addField(object->getFromAddress() ? object->getFromAddress()->getKey() : RegistryKeyType(0));
			query.addField(object->getRankInPath());
			query.addField(string());
			query.addField(object->getRoad() ? object->getRoad()->getKey() : RegistryKeyType(0));
			query.addField(object->getMetricOffset());
			query.execute(transaction);
	    }
	}

	namespace road
	{
	    RoadChunkTableSync::SearchResult RoadChunkTableSync::Search(
			Env& env,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<RoadChunkTableSync> query;
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
	    }
	}
}
