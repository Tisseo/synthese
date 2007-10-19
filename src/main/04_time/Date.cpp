
/** Date class implementation.
	@file Date.cpp

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

#include <sstream>
#include <iomanip>
#include <cmath>
#include <ctime>

#include "01_util/Conversion.h"

#include "04_time/Date.h"
#include "04_time/DateTime.h"
#include "04_time/TimeParseException.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace time
	{

		const Date Date::UNKNOWN_DATE(TIME_UNKNOWN);


		Date::Date( int day, int month, int year )
			: _day(UNKNOWN_VALUE), _month(UNKNOWN_VALUE), _year(UNKNOWN_VALUE)
		{
			time_t rawtime;
			struct tm * timeinfo = 0;

			bool tomorrow(false);
			if (day == TIME_TOMORROW)
			{
				day = TIME_CURRENT;
				tomorrow = true;
			}
			if (month == TIME_TOMORROW)
				month = TIME_CURRENT;
			if (year == TIME_TOMORROW)
				year = TIME_CURRENT;

			if ( day == TIME_CURRENT || month == TIME_CURRENT || year == TIME_CURRENT )
			{
				std::time ( &rawtime );
				timeinfo = localtime ( &rawtime );
			}

			// Month
			if ( month == TIME_CURRENT || month == TIME_SAME && day == TIME_CURRENT )
				_month = ( *timeinfo ).tm_mon + 1;
			else if ( month == TIME_MAX || month == TIME_SAME && day == TIME_MAX )
				_month = MONTHS_PER_YEAR;
			else if ( month == TIME_MIN || month == TIME_SAME && day == TIME_MIN )
				_month = 1;
			else if ( month == TIME_UNKNOWN || month == TIME_SAME && day == TIME_UNKNOWN )
				_month = UNKNOWN_VALUE;
			else if ( month != TIME_UNCHANGED &&
					( month != TIME_SAME || day != TIME_UNCHANGED ) &&
					month >= 1 && month <= MONTHS_PER_YEAR )
				_month = month;

			// Year
			if ( year == TIME_CURRENT || year == TIME_SAME && day == TIME_CURRENT )
				_year = ( *timeinfo ).tm_year + 1900;
			else if ( year == TIME_MAX || year == TIME_SAME && day == TIME_MAX )
				_year = MAX_YEAR;
			else if ( year == TIME_MIN || year == TIME_SAME && day == TIME_MIN )
				_year = 1;
			else if ( year == TIME_UNKNOWN || year == TIME_SAME && day == TIME_UNKNOWN )
				_year = UNKNOWN_VALUE;
			else if ( year != TIME_UNCHANGED && ( year != TIME_SAME || day != TIME_UNCHANGED ) && year >= 0 && year <= MAX_YEAR )
				_year = year;

			// Day
			if ( day == TIME_CURRENT )
				_day = ( *timeinfo ).tm_mday;
			else if ( day == TIME_MAX )
				_day = _month.getDaysCount( _year );
			else if ( day == TIME_MIN )
				_day = 1;
			else if ( day == TIME_UNKNOWN )
				_day = UNKNOWN_VALUE;
			else if ( day != TIME_UNCHANGED && day >= 1 && day <= 31 )
				_day = day;

			if (tomorrow)
				operator++(0);
		}


		Date::~Date()
		{
		}



		int
		Date::getDay() const
		{
			return _day.getValue ();
		}




		int
		Date::getMonth() const
		{
			return _month.getValue ();
		}



		int
		Date::getYear() const
		{
			return _year.getValue ();
		}



		bool
		Date::isValid () const
		{
			return _year.getValue () >= 0
				&& _month.getValue () > 0
				&& _month.getValue () <= MONTHS_PER_YEAR
				&& _day.getValue () > 0
				&& _day.getValue () <= _month.getDaysCount( _year );
		}




		int
		Date::getWeekDay () const
		{
			int mz = getMonth () - 2;
			int az = getYear ();
			if ( mz <= 0 )
			{
				mz += MONTHS_PER_YEAR;
				az --;
			}
			int s = az / 100;
			int e = az % 100;

			int j = getDay () + ( int ) floor ( 2.6 * mz - 0.2 );
			j += e + ( e / 4 ) + ( s / 4 ) - 2 * s;
			if ( j >= 0 )
				j %= 7;
			else
			{
				j = ( -j ) % 7;
				if ( j > 0 )
					j = 7 - j;
			}
			return j;
		}



		bool
		Date::isYearUnknown () const
		{
			return _year.getValue () == UNKNOWN_VALUE;
		}



		bool
		Date::isUnknown () const
		{
			return _year.getValue() == UNKNOWN_VALUE &&
				_month.getValue () == UNKNOWN_VALUE &&
				_day.getValue () == UNKNOWN_VALUE;
		}



		Date&
		Date::operator++( int )
		{
			_day++;
			if ( getDay () > _month.getDaysCount ( _year ) )
			{
				_day = 1;
				_month++;
				if ( getMonth () > MONTHS_PER_YEAR )
				{
					_month = 1;
					_year++;
				}
			}
			return ( *this );
		}




		Date&
		Date::operator--( int )
		{
			_day--;
			if ( getDay () == 0 )
			{
				_month--;
				if ( getMonth () == 0 )
				{
					_year--;
					_month = 12;
				}
				_day = _month.getDaysCount ( _year );
			}
			return *this;
		}



		Date&
		Date::operator+=( int daysToAdd )
		{
			for ( ; daysToAdd != 0; daysToAdd-- )
				operator++( 0 );
			return ( *this );
		}


		Date&
		Date::operator-=( int daysToSub )
		{
			// To be optimized...
			for ( ; daysToSub != 0; daysToSub-- )
				operator--( 0 );
			return ( *this );
		}



		Date& Date::operator = ( const std::string& op )
		{
			switch ( op.size() )
			{
				case 0:
					operator=(Date(TIME_UNKNOWN));
					break;

				case 1:
					operator=(Date(op[ 0 ]));
					break;

				default:
					int year = atoi ( op.substr ( 0, 4 ).c_str () );
					int month = atoi ( op.substr ( 4, 2 ).c_str () );
					int day = atoi ( op.substr ( 6, 2 ).c_str () );
					operator=(Date(day, month, year));
					break;
			}
			return *this;
		}


		Date&
		Date::operator = ( const DateTime& op )
		{
			return ( *this = op.getDate() );
		}









		std::ostream&
		operator<< ( std::ostream& os, const Date& op )
		{
			os << std::setw( 4 ) << std::setfill ( '0' )
			<< op.getYear ()
			<< std::setw( 2 ) << std::setfill ( '0' )
			<< op.getMonth ()
			<< std::setw( 2 ) << std::setfill ( '0' )
			<< op.getDay ();

			return os;
		}





		bool
		operator < ( const Date& op1, const Date& op2 )
		{
			return op1.getYear () < op2.getYear ()
				|| op1.getYear () == op2.getYear ()
				&& ( op1.getMonth () < op2.getMonth ()
						|| op1.getMonth () == op2.getMonth ()
						&& op1.getDay () < op2.getDay ()
					);
		}


		bool
		operator <= ( const Date& op1, const Date& op2 )
		{
			return op1.getYear () < op2.getYear ()
				|| op1.getYear () == op2.getYear ()
				&& ( op1.getMonth () < op2.getMonth ()
						|| op1.getMonth () == op2.getMonth ()
						&& op1.getDay () <= op2.getDay ()
					);

		}



		bool
		operator <= ( const Date& op1, const DateTime& op2 )
		{
			return op1 <= op2.getDate();
		}




		bool
		operator > ( const Date& op1, const Date& op2 )
		{
			return op1.getYear () > op2.getYear ()
				|| op1.getYear () == op2.getYear ()
				&& ( op1.getMonth () > op2.getMonth ()
						|| op1.getMonth () == op2.getMonth ()
						&& op1.getDay () > op2.getDay ()
					);

		}


		bool
		operator >= ( const Date& op1, const Date& op2 )
		{
			return op1.getYear () > op2.getYear ()
				|| op1.getYear () == op2.getYear ()
				&& ( op1.getMonth () > op2.getMonth ()
						|| op1.getMonth () == op2.getMonth ()
						&& op1.getDay () >= op2.getDay ()
					);

		}


		bool
		operator == ( const Date& op1, const Date& op2 )
		{
			return op1.getYear () == op2.getYear ()
				&& op1.getMonth () == op2.getMonth ()
				&& op1.getDay () == op2.getDay ();

		}


		bool
		operator != ( const Date& op1, const Date& op2 )
		{
			return op1.getYear () != op2.getYear ()
				|| op1.getMonth () != op2.getMonth ()
				|| op1.getDay () != op2.getDay ();

		}




		int
		Date::operator - ( const Date& op2 ) const
		{
			if ( *this < op2 )
				return ( -( op2 - *this ) );

			// Temporary...
			if ( ( getMonth () != op2.getMonth () ) ||
					( getYear () != op2.getYear () ) )
			{

				return _month.getDaysLeftToEndOfMonth ( getDay (), _year )
					+ op2._month.getDaysLeftToMonth ( op2._year, getMonth (), _year )
					+ op2.getDay ();
			}
			else
			{
				return getDay () - op2.getDay ();
			}
		}



		Date 
		Date::FromSQLDate (const std::string& sqlTimestamp)
		{
			if (sqlTimestamp.empty())
				return UNKNOWN_DATE;

			if(	sqlTimestamp.size() == 1)
				return Date(sqlTimestamp[0]);

			int firstSlash = (int) sqlTimestamp.find('-');
			int secondSlash = (int) sqlTimestamp.find('-', firstSlash+1);

			if (firstSlash == -1 || secondSlash == -1)
				throw TimeParseException(sqlTimestamp);

			return Date(Conversion::ToInt (sqlTimestamp.substr (secondSlash+1, sqlTimestamp.length() - secondSlash)),
				Conversion::ToInt (sqlTimestamp.substr (firstSlash+1, secondSlash - firstSlash)),
				Conversion::ToInt (sqlTimestamp.substr (0, firstSlash+1)));
		}


		Date 
		Date::FromString (const std::string& str)
		{
			if (str.empty())
				return UNKNOWN_DATE;

			int firstSlash = (int) str.find('/');
			int secondSlash = (int) str.find('/', firstSlash+1);

			if (firstSlash == -1 || secondSlash == -1)
				throw TimeParseException("Invalid date");

			return Date(
				Conversion::ToInt (str.substr (0, firstSlash+1)),
				Conversion::ToInt (str.substr (firstSlash+1, secondSlash - firstSlash)),
				Conversion::ToInt (str.substr (secondSlash+1, str.length() - secondSlash))
				);
		}

		std::string Date::toSQLString( bool withApostrophes ) const
		{
			stringstream s;
			if (withApostrophes)
				s << "'";
			if (!isUnknown())
			{
				s	<< setw( 4 ) << setfill ( '0' )
					<< getYear () << "-"
					<< setw( 2 ) << setfill ( '0' )
					<< getMonth () << "-"
					<< setw( 2 ) << setfill ( '0' )
					<< getDay ();

			}
			if (withApostrophes)
				s << "'";
			return s.str();
		}

		std::string Date::toString() const
		{
			if (isUnknown())
				return string();

			return Conversion::ToString(_day.getValue()) + "/" + Conversion::ToString(_month.getValue()) +"/" + Conversion::ToString(_year.getValue());
		}

	}
}
