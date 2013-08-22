
/** DesignatedLinePhysicalStop class implementation.
	@file DesignatedLinePhysicalStop.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DesignatedLinePhysicalStop.hpp"

#include "JourneyPattern.hpp"
#include "LineStopTableSync.h"
#include "StopPoint.hpp"

#include <geos/geom/LineString.h>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	
	namespace pt
	{
		DesignatedLinePhysicalStop::DesignatedLinePhysicalStop(
			util::RegistryKeyType id,
			JourneyPattern* line,
			std::size_t rankInPath,
			bool isDeparture,
			bool isArrival,
			double metricOffset,
			StopPoint* stop,
			bool scheduleInput,
			bool reservationNeeded
		):	Registrable(id),
			LinePhysicalStop(id, line, rankInPath, isDeparture, isArrival, metricOffset, stop),
			_scheduleInput(scheduleInput),
			_reservationNeeded(reservationNeeded)
		{}



		void DesignatedLinePhysicalStop::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			if(!getPhysicalStop()) throw Exception("Linestop save error. Missing physical stop");
			if(!getLine()) throw Exception("Linestop Save error. Missing line");

			pm.insert(TABLE_COL_ID, getKey());
			pm.insert(
				LineStopTableSync::COL_PHYSICALSTOPID,
				getPhysicalStop()->getKey()
			);
			pm.insert(
				LineStopTableSync::COL_LINEID,
				getLine()->getKey()
			);
			pm.insert(
				LineStopTableSync::COL_RANKINPATH,
				getRankInPath()
			);
			pm.insert(
				LineStopTableSync::COL_ISDEPARTURE,
				isDepartureAllowed()
			);
			pm.insert(
				LineStopTableSync::COL_ISARRIVAL,
				isArrivalAllowed()
			);
			pm.insert(
				LineStopTableSync::COL_METRICOFFSET,
				getMetricOffset()
			);
			pm.insert(
				LineStopTableSync::COL_SCHEDULEINPUT,
				getScheduleInput()
			);
			pm.insert(
				LineStopTableSync::COL_INTERNAL_SERVICE,
				false
			);
			pm.insert(
				LineStopTableSync::COL_RESERVATION_NEEDED,
				getReservationNeeded()
			);
			pm.insert(
				TABLE_COL_GEOMETRY,
				static_pointer_cast<geos::geom::Geometry, geos::geom::LineString>(getGeometry())
			);
		}
}	}
