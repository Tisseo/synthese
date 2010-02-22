
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

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<resa::ReservationTransaction>::KEY("ReservationTransaction");
	}

	namespace resa
	{


		ReservationTransaction::ReservationTransaction(
			RegistryKeyType key
		):	Registrable(key)
			, _bookingTime(second_clock::local_time())
			, _cancellationTime(not_a_date_time)
			, _originDateTime(not_a_date_time)
			, _destinationDateTime(not_a_date_time)
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

		void ReservationTransaction::setBookingTime( const ptime& time )
		{
			_bookingTime = time;
		}

		void ReservationTransaction::setCancellationTime( const ptime& time )
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

		const ptime& ReservationTransaction::getBookingTime() const
		{
			return _bookingTime;
		}

		const ptime& ReservationTransaction::getCancellationTime() const
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
			case OPTION: return statusText + " pouvant �tre annul�e avant le " + to_simple_string(getReservationDeadLine());
			case CANCELLED: return statusText + " le " + to_simple_string(_cancellationTime);
			case CANCELLED_AFTER_DELAY: return statusText + " le " + to_simple_string(_cancellationTime);
			case NO_SHOW: return statusText + " constat�e le " + to_simple_string(_cancellationTime);
			}

			return statusText;
		}



		ptime ReservationTransaction::getReservationDeadLine() const
		{
			ptime result(not_a_date_time);
			for (Reservations::const_iterator it(_reservations.begin()); it != _reservations.end(); ++it)
			{
				if (!(*it)->getReservationDeadLine().is_not_a_date_time() && (result.is_not_a_date_time() || (*it)->getReservationDeadLine() < result))
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
