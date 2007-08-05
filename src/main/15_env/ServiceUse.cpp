
/** ServiceUse class implementation.
	@file ServiceUse.cpp

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

#include "15_env/ServiceUse.h"
#include "15_env/Service.h"
#include "15_env/Edge.h"
#include "15_env/ReservationRule.h"

namespace synthese
{
	using namespace time;

	namespace env
	{

		ServiceUse::ServiceUse( const ServicePointer& servicePointer, const Edge* edge)
			: ServicePointer(servicePointer)
			, _secondEdge(edge)
			, _secondActualDateTime(servicePointer.getService()->getLeaveTime(servicePointer, edge))
		{
		}

		ServiceUse::ServiceUse()
			: _secondActualDateTime(TIME_UNKNOWN)
		{

		}
		const env::Edge* ServiceUse::getDepartureEdge() const
		{
			return (getMethod() == DEPARTURE_TO_ARRIVAL)
				? getEdge()
				: getSecondEdge();
		}

		const env::Edge* ServiceUse::getArrivalEdge() const
		{
			return (getMethod() == ARRIVAL_TO_DEPARTURE)
				? getEdge()
				: getSecondEdge();
		}

		const time::DateTime& ServiceUse::getDepartureDateTime() const
		{
			return  (getMethod() == DEPARTURE_TO_ARRIVAL)
				? getActualDateTime()
				: getSecondActualDateTime();
		}

		const time::DateTime& ServiceUse::getArrivalDateTime() const
		{
			return  (getMethod() == ARRIVAL_TO_DEPARTURE)
				? getActualDateTime()
				: getSecondActualDateTime();
		}

		const env::Edge* ServiceUse::getSecondEdge() const
		{
			return _secondEdge;
		}

		const time::DateTime& ServiceUse::getSecondActualDateTime() const
		{
			return _secondActualDateTime;
		}

		int ServiceUse::getDuration() const
		{
			return getArrivalDateTime() - getDepartureDateTime();
		}

		int ServiceUse::getDistance() const
		{
			return getArrivalEdge()->getMetricOffset() - getDepartureEdge()->getMetricOffset();
		}

		void ServiceUse::shift( int duration)
		{
			if (duration == 0)
				return;

			_actualTime += duration;
			_originDateTime += duration;
			_secondActualDateTime += duration;
			setServiceRange(getServiceRange() - duration);
		}

		bool ServiceUse::isReservationRuleCompliant( const time::DateTime& computingDateTime ) const
		{
			if (_service->getReservationRule()->isCompliant() == true)
				return _service->getReservationRule()->isRunPossible(_originDateTime, computingDateTime, getDepartureDateTime());

			return true;

		}

		time::DateTime ServiceUse::getReservationDeadLine() const
		{
			if (_service->getReservationRule()->isCompliant() || boost::logic::indeterminate(_service->getReservationRule()->isCompliant()))
				return _service->getReservationRule()->getReservationDeadLine(_originDateTime, getDepartureDateTime());

			return DateTime(TIME_UNKNOWN);
		}
	}
}
