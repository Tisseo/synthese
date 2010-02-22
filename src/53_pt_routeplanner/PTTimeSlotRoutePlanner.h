
/** PTTimeSlotRoutePlanner class header.
	@file PTTimeSlotRoutePlanner.h

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

#ifndef SYNTHESE_ptrouteplanner_PTTimeSlotRoutePlanner_h__
#define SYNTHESE_ptrouteplanner_PTTimeSlotRoutePlanner_h__

#include "TimeSlotRoutePlanner.h"
#include "PTRoutePlannerResult.h"
#include "RoutePlannerTypes.h"
#include "AccessParameters.h"
#include "GraphTypes.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace geography
	{
		class Place;
	}

	namespace ptrouteplanner
	{
		/** PTTimeSlotRoutePlanner class.
			@ingroup m53
		*/
		class PTTimeSlotRoutePlanner:
			public algorithm::TimeSlotRoutePlanner
		{
		private:
			const geography::Place* const _departurePlace;
			const geography::Place* const _arrivalPlace;

			graph::VertexAccessMap _extendToPhysicalStops(
				const graph::VertexAccessMap& vam,
				const graph::VertexAccessMap& destinationVam,
				graph::AccessDirection direction
			) const;

		public:
			PTTimeSlotRoutePlanner(
				const geography::Place* origin,
				const geography::Place* destination,
				const boost::posix_time::ptime& lowerDepartureTime,
				const boost::posix_time::ptime& higherDepartureTime,
				const boost::posix_time::ptime& lowerArrivalTime,
				const boost::posix_time::ptime& higherArrivalTime,
				const boost::optional<std::size_t>	maxSolutionsNumber,
				const graph::AccessParameters		accessParameters,
				const algorithm::PlanningOrder		planningOrder,
				std::ostream* logStream = NULL
			);

			PTRoutePlannerResult run() const;
		};
	}
}

#endif // SYNTHESE_ptrouteplanner_PTTimeSlotRoutePlanner_h__
