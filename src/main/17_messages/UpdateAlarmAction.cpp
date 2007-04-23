
/** UpdateAlarmAction class implementation.
	@file UpdateAlarmAction.cpp

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

#include "04_time/TimeParseException.h"
#include "04_time/DateTime.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "17_messages/UpdateAlarmAction.h"
#include "17_messages/SingleSentAlarm.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace time;
	using namespace db;
	
	namespace messages
	{
		const string UpdateAlarmAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "typ";
		const string UpdateAlarmAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sda";
		const string UpdateAlarmAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "eda";
		const string UpdateAlarmAction::PARAMETER_ENABLED = Action_PARAMETER_PREFIX + "ena";


		ParametersMap UpdateAlarmAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateAlarmAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				_alarm = AlarmTableSync::getAlarm(_request->getObjectId());
				_singleSentAlarm = dynamic_pointer_cast<SingleSentAlarm, Alarm>(_alarm);

				it = map.find(PARAMETER_TYPE);
				if (it == map.end())
					throw ActionException("Type not specified");
				_type = (AlarmLevel) Conversion::ToInt(it->second);

				if (_singleSentAlarm.get())
				{
					it = map.find(PARAMETER_START_DATE);
					if (it == map.end())
						throw ActionException("Start date not specified");
					if (!it->second.empty())
					{
						_startDate = DateTime::FromString(it->second);
					}
				
					it = map.find(PARAMETER_END_DATE);
					if (it == map.end())
						throw ActionException("End date not specified");
					if (!it->second.empty())
					{
						_endDate = DateTime::FromString(it->second);
					}
				
					// Enabled status
					it = map.find(PARAMETER_ENABLED);
					if (it == map.end())
						throw ActionException("Enabled status not specified");
					_enabled = Conversion::ToBool(it->second);
				}
			}
			catch (DBEmptyResultException<Alarm>)
			{
				throw ActionException("Specified alarm not found");
			}
			catch(TimeParseException)
			{
				throw ActionException("Une date ou une heure est mal form�e");
			}
			catch(...)
			{
				throw ActionException("Unknown error");
			}
		}

		UpdateAlarmAction::UpdateAlarmAction()
			: Action()
			, _startDate(TIME_UNKNOWN), _endDate(TIME_UNKNOWN)
		{}

		void UpdateAlarmAction::run()
		{
			_alarm->setLevel(_type);
			if (_singleSentAlarm.get())
			{
				_singleSentAlarm->setPeriodStart(_startDate);
				_singleSentAlarm->setPeriodEnd(_endDate);
				_singleSentAlarm->setIsEnabled(_enabled);
			}
			AlarmTableSync::save(_alarm.get());
		}
	}
}