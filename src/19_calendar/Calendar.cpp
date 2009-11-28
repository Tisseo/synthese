
/** Calendar class implementation.
	@file Calendar.cpp

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


#include "Calendar.h"

#include <boost/foreach.hpp>
#include <set>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
    namespace calendar
    {
		date Calendar::getFirstActiveDate(
		) const {
			if(_markedDates.empty())
				return gregorian::date();

			_BitSets::const_iterator it(_markedDates.begin());

			date result(it->first, Jan, 1);
			date_duration day(it->second.find_first());
			return result + day;
		}



		date Calendar::getLastActiveDate(
		) const {
			if(_markedDates.empty())
				return gregorian::date();

			_BitSets::const_reverse_iterator it(_markedDates.rbegin());

			date result(it->first, Jan, 1);

			size_t lp;
			for(size_t p(it->second.find_first()); p != dynamic_bitset<>::npos; p = it->second.find_next(p))
			{
				lp = p;
			}
			date_duration day(lp);

			return result + day;
		}



		Calendar::DatesVector Calendar::getActiveDates(
		) const {
			DatesVector result;
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				date d(it.first, Jan, 1);
				for(size_t p(it.second.find_first()); p != dynamic_bitset<>::npos; p = it.second.find_next(p))
				{
					result.push_back(d + date_duration(p));
				}
			}
			return result;
		}
	
	
	
		bool Calendar::isActive(const date& d) const
		{
			_BitSets::const_iterator it(_markedDates.find(d.year()));
			if(it == _markedDates.end()) return false;
			return it->second.test(_BitPos(d));
		}
	
	
	
		void Calendar::setActive(const date& d)
		{
			_BitSets::iterator it(_markedDates.find(d.year()));
			if(it == _markedDates.end())
			{
				it = _markedDates.insert(
					make_pair(d.year(), _BitSets::mapped_type(366))
				).first;
			}

			it->second.set(_BitPos(d));
		}
		
		
		
		void Calendar::clear()
		{
			_markedDates.clear();
		}
		
		
		
		void Calendar::setInactive(const date& d)
		{
			_BitSets::iterator it(_markedDates.find(d.year()));
			if(it == _markedDates.end())
				return;

			it->second.set(_BitPos(d), false);

			if(!it->second.any())
				_markedDates.erase(it);
		}
	
	
	
		Calendar& Calendar::operator&= (const Calendar& op)
		{
			*this = *this & op;
			return *this;
		}
	
	
	
		Calendar& Calendar::operator|=(
			const Calendar& op
		){
			*this = *this | op;
			return *this;
		}
	
	
	
	
		Calendar Calendar::operator& (const Calendar& op2) const
		{
			Calendar dest;
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				_BitSets::const_iterator it2(op2._markedDates.find(it.first));
				if(it2 == op2._markedDates.end()) continue;

				dest._markedDates[it.first] = it.second & it2->second;
			}
			return dest;
		}
	
	
	
		Calendar Calendar::operator| (const Calendar& op2) const
		{
			Calendar dest;
			set<greg_year> years;
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				years.insert(it.first);
			}
			BOOST_FOREACH(const _BitSets::value_type& it, op2._markedDates)
			{
				years.insert(it.first);
			}
			BOOST_FOREACH(greg_year y, years)
			{
				_BitSets::const_iterator it1(_markedDates.find(y));
				_BitSets::const_iterator it2(op2._markedDates.find(y));
				if(it1 == _markedDates.end())
				{
					dest._markedDates[y] = it2->second;
				}
				else
				{
					if(it2 == op2._markedDates.end())
					{
						dest._markedDates[y] = it1->second;
					}
					else
					{
						dest._markedDates[y] = it1->second | it2->second;
					}
				}
			}
			return dest;
		}
	
	
	
		bool Calendar::hasAtLeastOneCommonDateWith( const Calendar& op ) const
		{
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				_BitSets::const_iterator it2(op._markedDates.find(it.first));
				if(it2 == op._markedDates.end()) continue;

				if((it.second & it2->second).any()) return true;
			}
			return false;
		}



		bool Calendar::operator==( const Calendar& op ) const
		{
			return _markedDates == op._markedDates;
		}



		void Calendar::subDates( const Calendar& calendar )
		{
			BOOST_FOREACH(const _BitSets::value_type& it, calendar._markedDates)
			{
				_BitSets::iterator it2(_markedDates.find(it.first));
				if(it2 == _markedDates.end()) continue;

				for(size_t p(it.second.find_first()); p != dynamic_bitset<>::npos; p = it.second.find_next(p))
				{
					it2->second.set(p, false);
				}
			}
		}



		size_t Calendar::_BitPos( const boost::gregorian::date& d )
		{
			return d.day_of_year() - 1;
		}



		bool Calendar::empty() const
		{
			return _markedDates.empty();
		}
	}
}
