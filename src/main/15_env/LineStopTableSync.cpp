
/** LineStopTableSync class implementation.
	@file LineStopTableSync.cpp

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

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>
#include <assert.h>

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/LineStop.h"
#include "15_env/Point.h"
#include "15_env/RoadChunkTableSync.h"
#include "15_env/LineStopTableSync.h"
#include "15_env/EnvModule.h"

using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{



LineStopTableSync::LineStopTableSync ()
: ComponentTableSync (LINESTOPS_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (LINESTOPS_TABLE_COL_PHYSICALSTOPID, "INTEGER", false);
    addTableColumn (LINESTOPS_TABLE_COL_LINEID, "INTEGER", false);
    addTableColumn (LINESTOPS_TABLE_COL_RANKINPATH, "INTEGER", false);
    addTableColumn (LINESTOPS_TABLE_COL_ISDEPARTURE, "BOOLEAN", false);
    addTableColumn (LINESTOPS_TABLE_COL_ISARRIVAL, "BOOLEAN", false);
    addTableColumn (LINESTOPS_TABLE_COL_METRICOFFSET, "DOUBLE", true);
    addTableColumn (LINESTOPS_TABLE_COL_VIAPOINTS, "TEXT", true);
}



LineStopTableSync::~LineStopTableSync ()
{

}

    


void 
LineStopTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
			   synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    if (environment.getLineStops ().contains (id)) return;

    uid fromPhysicalStopId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_PHYSICALSTOPID)));

    uid lineId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_LINEID)));

    int rankInPath (
	Conversion::ToInt (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_RANKINPATH)));

    bool isDeparture (Conversion::ToBool (
			  rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_ISDEPARTURE)));
    bool isArrival (Conversion::ToBool (
			rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_ISARRIVAL)));
    
    double metricOffset (
	Conversion::ToDouble (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_METRICOFFSET)));
    
    std::string viaPointsStr (
	rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_VIAPOINTS));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    LineStop* ls = new synthese::env::LineStop (
	id, 
	environment.getLines ().get (lineId),
	rankInPath,
	isDeparture, 
	isArrival,
	metricOffset,
	EnvModule::getPhysicalStops ().get (fromPhysicalStopId)
	);

    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 (":");
    tokenizer viaPointsTokens (viaPointsStr, sep1);
    for (tokenizer::iterator viaPointIter = viaPointsTokens.begin();
	 viaPointIter != viaPointsTokens.end (); ++viaPointIter)
    {
	tokenizer valueTokens (*viaPointIter, sep2);
	tokenizer::iterator valueIter = valueTokens.begin();

	// X:Y
	ls->addViaPoint (synthese::env::Point (Conversion::ToDouble (*valueIter), 
					       Conversion::ToDouble (*(++valueIter))));
    }

    environment.getLines ().get (lineId)->addEdge (ls);
    environment.getLineStops ().add (ls);
}






void 
LineStopTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    LineStop* ls = environment.getLineStops().get (id);

    double metricOffset (
	Conversion::ToDouble (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_METRICOFFSET)));
    
    ls->setMetricOffset (metricOffset);

    std::string viaPointsStr (
	rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_VIAPOINTS));

    ls->clearViaPoints ();

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
	ls->addViaPoint (synthese::env::Point (Conversion::ToDouble (*valueIter), 
					       Conversion::ToDouble (*(++valueIter))));
    }


}





void 
LineStopTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getLineStops ().remove (id);
}















}

}

