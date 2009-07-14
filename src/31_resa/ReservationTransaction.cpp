
/** ReservationTransaction class implementation.
	@file ReservationTransaction.cpp

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

#include "ReservationTransaction.h"
#include "Registry.h"
#include "Reservation.h"
#include "ResaModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> const string Registry<resa::ReservationTransaction>::KEY("ReservationTransaction");
	}

	namespace resa
	{


		ReservationTransaction::ReservationTransaction(
			RegistryKeyType key
		):	Registrable(key)
			, _bookingTime(TIME_CURRENT)
			, _cancellationTime(TIME_UNKNOWN)
			, _originDateTime(TIME_UNKNOWN)
			, _destinationDateTime(TIME_UNKNOWN)
			, _cancelUserId(UNKNOWN_VALUE)
			, _bookingUserId(UNKNOWN_VALUE)
			, _customerUserId(UNKNOWN_VALUE)
			, _lastReservation(UNKNOWN_VALUE)
		{

		}

		void ReservationTransaction::setLastReservation( uid id )
		{
			_lastReservation = id;
		}

		void ReservationTransaction::setSeats( int seats )
		{
			_seats = seats;
		}

		void ReservationTransaction::setBookingTime( const time::DateTime& time )
		{
			_bookingTime = time;
		}

		void ReservationTransaction::setCancellationTime( const time::DateTime& time )
		{
			_cancellationTime = time;
		}

		void ReservationTransaction::setCustomerUserId( uid id )
		{
			_customerUserId = id;
		}

		void ReservationTransaction::setCustomerName( const std::string& name )
		{
			_customerName = name;
		}

		void ReservationTransaction::setCustomerPhone( const std::string& phone )
		{
			_customerPhone = phone;
		}

		void ReservationTransaction::setBookingUserId( uid id )
		{
			_bookingUserId = id;
		}

		void ReservationTransaction::setCancelUserId( uid id )
		{
			_cancelUserId = id;
		}

		uid ReservationTransaction::getLastReservation() const
		{
			return _lastReservation;
		}

		int ReservationTransaction::getSeats() const
		{
			return _seats;
		}

		const time::DateTime& ReservationTransaction::getBookingTime() const
		{
			return _bookingTime;
		}

		const time::DateTime& ReservationTransaction::getCancellationTime() const
		{
			return _cancellationTime;
		}

		uid ReservationTransaction::getCustomerUserId() const
		{
			return _customerUserId;
		}

		const std::string& ReservationTransaction::getCustomerName() const
		{
			return _customerName;
		}

		const std::string& ReservationTransaction::getCustomerPhone() const
		{
			return _customerPhone;
		}

		uid ReservationTransaction::getBookingUserId() const
		{
			return _bookingUserId;
		}

		uid ReservationTransaction::getCancelUserId() const
		{
			return _cancelUserId;
		}

		void ReservationTransaction::setCustomerEMail( const std::string& email )
		{
			_customerEMail = email;
		}

		const std::string& ReservationTransaction::getCustomerEMail() const
		{
			return _customerEMail;
		}


		Reservation* ReservationTransaction::newReservation()
		{
			Reservation* reservation(new Reservation);
			reservation->setTransaction(this);
			return reservation;
		}



		const ReservationTransaction::Reservations& ReservationTransaction::getReservations() const
		{
			return _reservations;
		}



		synthese::resa::ReservationStatus ReservationTransaction::getStatus() const
		{
			ReservationStatus status(NO_RESERVATION);
			for(Reservations::const_iterator it(_reservations.begin()); it != _reservations.end(); ++it)
			{
				ReservationStatus rs((*it)->getStatus());

				if (rs == NO_SHOW)
					return NO_SHOW;

				if (rs < status)
					status = rs;
			}
			return status;
		}



		std::string ReservationTransaction::getFullStatusText() const
		{
			ReservationStatus status(getStatus());
			string statusText(ResaModule::GetStatusText(status));

			switch(status)
			{
			case OPTION: return statusText + " pouvant �tre annul�e avant le " + getReservationDeadLine().toString();
			case CANCELLED: return statusText + " le " + _cancellationTime.toString();
			case CANCELLED_AFTER_DELAY: statusText + " le " + _cancellationTime.toString();
			case NO_SHOW: return statusText + " constant�e le " + _cancellationTime.toString();
			}

			return statusText;
		}



		time::DateTime ReservationTransaction::getReservationDeadLine() const
		{
			DateTime result(TIME_UNKNOWN);
			for (Reservations::const_iterator it(_reservations.begin()); it != _reservations.end(); ++it)
			{
				if (!(*it)->getReservationDeadLine().isUnknown() && (result.isUnknown() || (*it)->getReservationDeadLine() < result))
					result = (*it)->getReservationDeadLine();
			}
			return result;
		}



		void ReservationTransaction::addReservation(Reservation* resa )
		{
			_reservations.push_back(resa);
		}
	}
}
