////////////////////////////////////////////////////////////////////////////////
///	MessagesAdvancedSelectTableSync class implementation.
///	@file MessagesAdvancedSelectTableSync.cpp
///	@author Hugues Romain (RCS)
///	@date ven jan 30 2009
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

#include "MessagesAdvancedSelectTableSync.h"
#include "DBModule.h"
#include "ScenarioTableSync.h"
#include "AlarmTableSync.h"
#include "Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace time;
	using namespace util;
	
	namespace messages
	{
		SentMessages GetSentMessages(
			const Date& date,
			StatusSearch status,
			AlarmConflict conflict,
			AlarmLevel level,
			RegistryKeyType scenarioId,
			int first,
			int number,
			bool orderByDate,
			bool orderByLevel,
			bool orderByStatus,
			bool orderByConflict,
			bool raisingOrder
		){
			DateTime now(TIME_CURRENT);
			
			stringstream query;
			
			if(scenarioId != 0)
			{
				query <<
					"SELECT " <<
						TABLE_COL_ID << "," <<
						ScenarioTableSync::COL_PERIODSTART << " AS start," <<
						ScenarioTableSync::COL_PERIODEND << " AS end," <<
						ScenarioTableSync::COL_NAME << " AS name," <<
						static_cast<int>(ALARM_LEVEL_SCENARIO) << " AS level," <<
						ScenarioTableSync::COL_ENABLED << " AS enabled " <<
					" FROM " << ScenarioTableSync::TABLE.NAME <<
					" WHERE " <<
						ScenarioTableSync::COL_IS_TEMPLATE << "=0";
				switch(status)
				{
				case BROADCAST_OVER:
					query << " AND " << ScenarioTableSync::COL_PERIODEND << " IS NOT NULL "
						<< " AND " << ScenarioTableSync::COL_PERIODEND << "<" << now.toSQLString()
					;
					break;
					
				case BROADCAST_RUNNING:
					query << " AND (" << ScenarioTableSync::COL_PERIODEND << " IS NULL "
						<< " OR " << ScenarioTableSync::COL_PERIODEND << ">" << now.toSQLString()
						<< ") AND " << ScenarioTableSync::COL_ENABLED
					;
					break;
					
				case BROADCAST_RUNNING_WITH_END:
					query << " AND (" << ScenarioTableSync::COL_PERIODSTART << " IS NULL "
						<< " OR " << ScenarioTableSync::COL_PERIODSTART << "<=" << now.toSQLString()
						<< ") AND " << ScenarioTableSync::COL_ENABLED
						<<  " AND " << ScenarioTableSync::COL_PERIODEND << " IS NOT NULL "
						<< " AND " << ScenarioTableSync::COL_PERIODEND << ">" << now.toSQLString()
					;
					break;
					
				case BROADCAST_RUNNING_WITHOUT_END:
					query << " AND (" << ScenarioTableSync::COL_PERIODSTART << " IS NULL "
						<< " OR " << ScenarioTableSync::COL_PERIODSTART << "<=" << now.toSQLString()
						<< ") AND " << ScenarioTableSync::COL_ENABLED
						<<  " AND " << ScenarioTableSync::COL_PERIODEND << " IS NULL "
					;
					break;
					
				case FUTURE_BROADCAST:
					query << " AND " << ScenarioTableSync::COL_PERIODSTART << " IS NOT NULL "
						<< " AND " << ScenarioTableSync::COL_PERIODSTART << ">" << now.toSQLString()
						<< " AND " << ScenarioTableSync::COL_ENABLED
					;
					break;
					
				case BROADCAST_DRAFT:
					query << " AND (" << ScenarioTableSync::COL_PERIODEND << " IS NULL "
						<< " OR " << ScenarioTableSync::COL_PERIODEND << ">" << now.toSQLString()
						<< ") AND NOT " << ScenarioTableSync::COL_ENABLED
					;
					break;
				}
				if(scenarioId != UNKNOWN_VALUE)
				{
					query << " AND " << ScenarioTableSync::COL_TEMPLATE << "=" << scenarioId;
				}
				if(!date.isUnknown())
				{
					query << " AND (" << ScenarioTableSync::COL_PERIODSTART << " IS NULL OR " <<
						ScenarioTableSync::COL_PERIODSTART << " <= " << date.toSQLString() <<
						") AND (" << ScenarioTableSync::COL_PERIODEND << " IS NULL OR " <<
						ScenarioTableSync::COL_PERIODEND << " >= " << date.toSQLString() <<
						")"
					;
				}
			}

			if(scenarioId == 0 || scenarioId == UNKNOWN_VALUE)
			{
				if(scenarioId == UNKNOWN_VALUE)
				{
					query << " UNION ";
				}
				query <<
					" SELECT " <<
					TABLE_COL_ID << "," <<
					AlarmTableSync::COL_PERIODSTART << " AS start," <<
					AlarmTableSync::COL_PERIODEND << " AS end," <<
					AlarmTableSync::COL_SHORT_MESSAGE << " AS name," <<
					AlarmTableSync::COL_LEVEL << " AS level," << // Level
					AlarmTableSync::COL_ENABLED << " AS enabled" <<
					
					
	/*				<< ",(SELECT COUNT(" << AlarmObjectLinkTableSync::COL_OBJECT_ID << ") FROM " <<
					AlarmObjectLinkTableSync::TABLE.NAME << " AS aol3 WHERE aol3." <<
					AlarmObjectLinkTableSync::COL_ALARM_ID << "=a." << TABLE_COL_ID << ") AS " <<
					_COL_RECIPIENTS_NUMBER*/
					
	/*				<< ",(SELECT MAX(al2."  << COL_LEVEL << ") FROM " << AlarmObjectLinkTableSync::TABLE.NAME <<
					" AS aol1 INNER JOIN " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol2 ON aol1." <<
					AlarmObjectLinkTableSync::COL_OBJECT_ID << "=aol2." <<
					AlarmObjectLinkTableSync::COL_OBJECT_ID << " AND aol1." <<
					AlarmObjectLinkTableSync::COL_ALARM_ID << " != aol2." <<
					AlarmObjectLinkTableSync::COL_ALARM_ID << " INNER JOIN " << TABLE.NAME << " AS al2 ON al2." <<
					TABLE_COL_ID << " = aol2." << AlarmObjectLinkTableSync::COL_ALARM_ID << " WHERE "
					<< " aol1." << AlarmObjectLinkTableSync::COL_ALARM_ID << "=a." << TABLE_COL_ID
					<< " AND al2." << COL_IS_TEMPLATE << "=0 "
					<< " AND (al2." << COL_PERIODSTART << " IS NULL OR a." << COL_PERIODEND << " IS NULL OR al2." <<
					COL_PERIODSTART << " <= a." << COL_PERIODEND << ")"
					<< " AND (al2." << COL_PERIODEND << " IS NULL OR a." << COL_PERIODSTART << " IS NULL OR al2." <<
					COL_PERIODEND <<" >= a." << COL_PERIODSTART << ")"
					<< ") AS " << _COL_CONFLICT_LEVEL*/
					
					" FROM " << AlarmTableSync::TABLE.NAME << " AS a " <<
					" WHERE " <<
						AlarmTableSync::COL_IS_TEMPLATE << "=0 AND " <<
						AlarmTableSync::COL_SCENARIO_ID << "=0"
				;
				switch(status)
				{
				case BROADCAST_OVER:
					query << " AND " << AlarmTableSync::COL_PERIODEND << " IS NOT NULL "
						<< " AND " << AlarmTableSync::COL_PERIODEND << "<" << now.toSQLString()
					;
					break;
					
				case BROADCAST_RUNNING:
					query << " AND (" << AlarmTableSync::COL_PERIODEND << " IS NULL "
						<< " OR " << AlarmTableSync::COL_PERIODEND << ">" << now.toSQLString()
						<< ") AND " << AlarmTableSync::COL_ENABLED
					;
					break;
					
				case BROADCAST_RUNNING_WITH_END:
					query << " AND (" << AlarmTableSync::COL_PERIODSTART << " IS NULL "
						<< " OR " << AlarmTableSync::COL_PERIODSTART << "<=" << now.toSQLString()
						<< ") AND " << AlarmTableSync::COL_ENABLED
						<<  " AND " << AlarmTableSync::COL_PERIODEND << " IS NOT NULL "
						<< " AND " << AlarmTableSync::COL_PERIODEND << ">" << now.toSQLString()
					;
					break;
					
				case BROADCAST_RUNNING_WITHOUT_END:
					query << " AND (" << AlarmTableSync::COL_PERIODSTART << " IS NULL "
						<< " OR " << AlarmTableSync::COL_PERIODSTART << "<=" << now.toSQLString()
						<< ") AND " << AlarmTableSync::COL_ENABLED
						<<  " AND " << AlarmTableSync::COL_PERIODEND << " IS NULL "
					;
					break;
					
				case FUTURE_BROADCAST:
					query << " AND " << AlarmTableSync::COL_PERIODSTART << " IS NOT NULL "
						<< " AND " << AlarmTableSync::COL_PERIODSTART << ">" << now.toSQLString()
						<< " AND " << AlarmTableSync::COL_ENABLED
					;
					break;
					
				case BROADCAST_DRAFT:
					query << " AND (" << AlarmTableSync::COL_PERIODEND << " IS NULL "
						<< " OR " << AlarmTableSync::COL_PERIODEND << ">" << now.toSQLString()
						<< ") AND NOT " << AlarmTableSync::COL_ENABLED
					;
					break;
				}
							/*if (conflict == ALARM_NO_CONFLICT)
					query << " AND " << _COL_CONFLICT_LEVEL << " IS NULL";
				if (conflict == ALARM_WARNING_ON_INFO)
					query << " AND a." << COL_LEVEL << "=" << ((int) ALARM_LEVEL_WARNING) << " AND " << _COL_CONFLICT_LEVEL << "=" << ((int) ALARM_LEVEL_INFO);
				if (conflict == ALARM_INFO_UNDER_WARNING)
					query << " AND a." << COL_LEVEL << "=" << ((int) ALARM_LEVEL_INFO) << " AND " << _COL_CONFLICT_LEVEL << "=" << ((int) ALARM_LEVEL_WARNING);
				if (conflict == ALARM_CONFLICT)
					query << " AND a." << COL_LEVEL << "=" << _COL_CONFLICT_LEVEL;
				*/if (level != ALARM_LEVEL_UNKNOWN)
					query << " AND a." << AlarmTableSync::COL_LEVEL << "=" << (static_cast<int>(level));
				if(!date.isUnknown())
				{
					query << " AND (" << AlarmTableSync::COL_PERIODSTART << " IS NULL OR " <<
						AlarmTableSync::COL_PERIODSTART << " <= " << date.toSQLString() <<
						") AND (" << AlarmTableSync::COL_PERIODEND << " IS NULL OR " <<
						AlarmTableSync::COL_PERIODEND << " >= " << date.toSQLString() <<
						")"
					;
				}
			}
			
			if(orderByDate)
			{
				query <<
					" ORDER BY" <<
						" start IS NULL " << (raisingOrder ? "DESC" : "ASC") << "," <<
						" start " << (raisingOrder ? "ASC" : "DESC") << "," <<
						" end IS NULL " << (raisingOrder ? "ASC" : "DESC") << "," <<
						" end " << (raisingOrder ? "ASC" : "DESC")
				;
			}
			else
			{
			}
			
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);
		
			SentMessages result;
			SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
			while (rows->next ())
			{
				SentMessage msg;
				msg.id = rows->getKey();
				msg.startTime = DateTime::FromSQLTimestamp(rows->getText("start"));
				msg.endTime = DateTime::FromSQLTimestamp(rows->getText("end"));
				msg.name = rows->getText("name");
				msg.enabled = rows->getBool("enabled");
				msg.level = static_cast<AlarmLevel>(rows->getInt("level"));
				msg.conflict = ALARM_NO_CONFLICT;
				result.push_back(msg);
			}
			return result;
		}

	}
}
