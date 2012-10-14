
/** HourPeriod class implementation.
	@file HourPeriod.cpp

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

#include "HourPeriod.h"

#include "ParametersMap.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	
	namespace pt_website
	{
		const std::string HourPeriod::ATTR_CAPTION = "caption";
		const std::string HourPeriod::ATTR_BEGIN_HOUR = "begin_hour";
		const std::string HourPeriod::ATTR_END_HOUR = "end_hour";

		HourPeriod::HourPeriod (
			const string& caption
			, const time_duration& beginHour
			, const time_duration& endHour
		)	: _beginHour(beginHour)
			, _endHour(endHour)
			, _caption(caption)
		{}


		HourPeriod::~HourPeriod ()
		{}



		const string& HourPeriod::getCaption() const
		{
			return _caption;
		}



		const time_duration& HourPeriod::getBeginHour() const
		{
			return _beginHour;
		}



		const time_duration& HourPeriod::getEndHour() const
		{
			return _endHour;
		}



		void HourPeriod::toParametersMap( util::ParametersMap& pm ) const
		{
			pm.insert(ATTR_CAPTION, _caption);
			if(!_beginHour.is_not_a_date_time())
			{
				pm.insert(ATTR_BEGIN_HOUR, to_simple_string(_beginHour));
			}
			if(!_endHour.is_not_a_date_time())
			{
				pm.insert(ATTR_END_HOUR, to_simple_string(_endHour));
			}
		}
}	}
