
/** DateTime class implementation.
	@file DateTime.cpp

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

#include "DateTime.h"

#include "TimeParseException.h"
#include "Schedule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace time
	{
		DateTime::DateTime ( int day, int month, int year,
							int hours, int minutes, int seconds )
							: _date(day, month, year)
							, _hour((hours == TIME_SAME) ? day : hours, minutes, seconds)
		{
		}




		DateTime::DateTime (const DateTime& ref)
			: _date(ref._date)
			, _hour(ref._hour)
		{
		}



		DateTime::DateTime( const Date& date )
			: _date(date)
			, _hour(TIME_MIN)
		{
		}

		DateTime::DateTime( const Date& date, const Hour& hour )
			: _date(date), _hour(hour)
		{	}

		DateTime::DateTime( const Date& date, const Schedule& schedule )
			: _date(date), _hour(schedule.getHour())
		{
			_date += schedule.getDaysSinceDeparture();
		}

		DateTime::~DateTime()
		{
		}



		DateTime& 
		DateTime::operator = ( const DateTime& ref)
		{
			if (this == &ref) return (*this);
			_date = ref._date;
			_hour = ref._hour;
			return (*this);
		}



		DateTime&
		DateTime::operator += ( int minutesDuration )
		{
			if (minutesDuration < 0)
				return operator -=(-minutesDuration);
			
			_date += ( _hour += minutesDuration );
			return ( *this );
		}





		DateTime&
		DateTime::operator -= ( int minutesDuration )
		{
			if (minutesDuration < 0)
				return operator +=(-minutesDuration);

			_date -= ( _hour -= minutesDuration );
			return ( *this );
		}



		DateTime&
		DateTime::operator ++( int )
		{
			_date++;
			return ( *this );
		}



		DateTime&
		DateTime::operator --( int )
		{
			_date--;
			return ( *this );
		}



		bool
		DateTime::isValid () const
		{
			return ( _date.isValid () && _hour.isValid () );
		}



		DateTime&
		DateTime::operator = ( const Date& op )
		{
			_date = op;
			return ( *this );
		}



		DateTime&
		DateTime::operator = ( const Hour& op )
		{
			_hour = op;
			return ( *this );
		}



		int
		DateTime::getDay () const
		{
			return _date.getDay ();
		}



		int
		DateTime::getMonth () const
		{
			return _date.getMonth ();
		}




		int
		DateTime::getYear () const
		{
			return _date.getYear ();
		}



		int
		DateTime::getMinutes() const
		{
			return _hour.getMinutes();
		}



		int DateTime::getHours () const
		{
			return _hour.getHours ();
		}



		const Date&
		DateTime::getDate() const
		{
			return _date;
		}



		const Hour&
		DateTime::getHour() const
		{
			return _hour;
		}


		void		DateTime::setHour ( const Hour& hour)
		{
			_hour = hour;
		}



		void		DateTime::addDaysDuration ( int daysToAdd )
		{
			_date += daysToAdd;
		}



		void		DateTime::subDaysDuration ( int daysToSub )
		{
			_date -= daysToSub;

		}



		ostream&		operator<< ( ostream& os, const DateTime& op )
		{
			os << op.toSQLString(false);
			return os;
		}



		bool			DateTime::operator == ( const DateTime& op2 ) const
		{
			return (getDate () == op2.getDate () ) &&
				( getHour () == op2.getHour () );
		}



		bool			DateTime::operator != ( const DateTime& op2 ) const
		{
			return ( getDate () != op2.getDate () || getHour () != op2.getHour () );
		}



		bool			DateTime::operator<=( const DateTime &op2 ) const
		{
			return ( getDate () < op2.getDate () || getDate () == op2.getDate () && getHour () <= op2.getHour () );
		}



		bool			DateTime::operator < ( const DateTime &op2 ) const
		{
			return ( getDate () < op2.getDate () || getDate () == op2.getDate () && getHour () < op2.getHour () );
		}



		bool			DateTime::operator>=( const DateTime& op2 ) const
		{
			return ( getDate () > op2.getDate () ||
					getDate () == op2.getDate () && getHour () >= op2.getHour () );
		}



		bool			DateTime::operator > ( const DateTime &op2 ) const
		{
			return ( getDate () > op2.getDate () ||
					( getDate () == op2.getDate () && getHour () > op2.getHour () ) );
		}



		DateTime DateTime::operator + (int minutesDuration ) const
		{
			DateTime result (*this);
			result += minutesDuration;
			return result;
		}



		DateTime DateTime::operator - (int minutesDuration ) const
		{
			DateTime result (*this);
			result -= minutesDuration;
			return result;
		}




		DateTime // AAAA/(M)M/(J)J hh:mm
		DateTime::FromSQLTimestamp (const string& sqlTimestamp)
		{
			if (sqlTimestamp.empty() || sqlTimestamp == "NULL")
				return DateTime(time::TIME_UNKNOWN);

			if (sqlTimestamp.size() == 1)
				return DateTime(sqlTimestamp[0]);

			int spaceSeparator = (int) sqlTimestamp.find(' ');

			return (spaceSeparator == -1)
				? DateTime(Date::FromSQLDate(sqlTimestamp))
				: DateTime(Date::FromSQLDate(sqlTimestamp.substr(0, spaceSeparator)), Hour::FromSQLTime(sqlTimestamp.substr(spaceSeparator+1, sqlTimestamp.length() - spaceSeparator)));
		}



		DateTime 
		DateTime::FromString (const string& str)
		{
			if (str.empty())
				return DateTime(time::TIME_UNKNOWN);

			int spaceSeparator = (int) str.find(' ');

			return (spaceSeparator == -1)
				? DateTime(Date::FromString(str))
				: DateTime(Date::FromString(str.substr(0, spaceSeparator)), Hour::FromSQLTime(str.substr(spaceSeparator+1, str.length() - spaceSeparator)));
		}

		string DateTime::toSQLString(bool withApostrophes) const
		{
			return isUnknown()
				? "NULL"
				: ((withApostrophes ? "'" : string()) + _date.toSQLString(false) + " " + _hour.toSQLString(false) + (withApostrophes ? "'" : string()));
		}

		bool DateTime::isUnknown() const
		{
			return _date.isUnknown();
		}

		string DateTime::toString(bool withSeconds) const
		{
			return isUnknown()
				? string()
				: (_date.toString() + " " + _hour.toString(withSeconds));
		}



		boost::posix_time::time_duration DateTime::getSecondsDifference( const DateTime& op2 ) const
		{
			return toPosixTime() - op2.toPosixTime();
		}



		boost::posix_time::ptime DateTime::toPosixTime() const
		{
			return isUnknown() ? posix_time::ptime() : posix_time::ptime(_date.toGregorianDate(), _hour.toPosixTimeDuration());
		}



		DateTime DateTime::FromPosixTime( const boost::posix_time::ptime value )
		{
			return DateTime(Date::FromGregorianDate(value.date()), Hour::FromTimeDuration(value.time_of_day()));
		}
	}
}



