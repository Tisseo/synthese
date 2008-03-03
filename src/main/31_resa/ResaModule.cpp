
/** ResaModule class implementation.
	@file ResaModule.cpp

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

#include "ResaModule.h"

#include "05_html/HTMLTable.h"

#include "31_resa/Reservation.h"
#include "31_resa/ReservationTransaction.h"

namespace synthese
{
	template<> const std::string util::FactorableTemplate<util::ModuleClass, resa::ResaModule>::FACTORY_KEY("31_resa");

	namespace resa
	{

		std::string ResaModule::getName() const
		{
			return "TAD Réservation";
		}

		void ResaModule::DisplayReservation( std::ostream& stream, html::HTMLTable& t, const Reservation* reservation)
		{
			stream << t.row();
			stream << t.col() << reservation->getDepartureTime().toString();
			stream << t.col() << reservation->getDeparturePlaceName();
			stream << t.col() << reservation->getArrivalPlaceName();
			stream << t.col() << reservation->getArrivalTime().toString();
			stream << t.col() << reservation->getTransaction()->getSeats();
			stream << t.col() << reservation->getTransaction()->getCustomerName();
		}

	}
}
