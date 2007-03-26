
/** MessagesModule class implementation.
	@file MessagesModule.cpp

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

#include "17_messages/MessagesModule.h"
#include "17_messages/TextTemplateTableSync.h"
#include "17_messages/TextTemplate.h"

using namespace std;

namespace synthese
{
	namespace messages
	{
		Alarm::Registry MessagesModule::_alarms;
		Scenario::Registry MessagesModule::_scenarii;

		Alarm::Registry& 
			MessagesModule::getAlarms ()
		{
			return _alarms;
		}

		Scenario::Registry& MessagesModule::getScenarii()
		{
			return _scenarii;
		}

		std::map<uid, std::string> MessagesModule::getScenariiLabels( bool withAll /*= false*/ )
		{
			map<uid,string> m;
			if (withAll)
				m.insert(make_pair(0, "(tous)"));
			for(Scenario::Registry::const_iterator it = _scenarii.begin(); it != _scenarii.end(); ++it)
				m.insert(make_pair(it->first, it->second->getName()));
			return m;

		}

		std::map<AlarmLevel, std::string> MessagesModule::getLevelLabels( bool withAll /*= false*/ )
		{
			map<AlarmLevel, string> m;
			if (withAll)
				m.insert(make_pair(ALARM_LEVEL_UNKNOWN, "(tous)"));
			m.insert(make_pair(ALARM_LEVEL_INFO, getLevelLabel(ALARM_LEVEL_INFO)));
			m.insert(make_pair(ALARM_LEVEL_WARNING, getLevelLabel(ALARM_LEVEL_WARNING)));
			return m;
		}

		std::map<AlarmConflict, std::string> MessagesModule::getConflictLabels( bool withAll /*= false*/ )
		{
			map<AlarmConflict, string> m;
			if (withAll)
				m.insert(make_pair(ALARM_CONFLICT_UNKNOWN, "(tous)"));
			m.insert(make_pair(ALARM_NO_CONFLICT, "Sans conflit"));
			m.insert(make_pair(ALARM_WARNING_ON_INFO, "Prioritaire sur complémentaire"));
			m.insert(make_pair(ALARM_CONFLICT, "En conflit"));
			return m;
		}

		std::map<uid, std::string> MessagesModule::getTextTemplateLabels(const AlarmLevel& level)
		{
			map<uid, string> m;
			vector<TextTemplate*> v = TextTemplateTableSync::search(level);
			for (vector<TextTemplate*>::iterator it = v.begin(); it != v.end(); ++it)
			{
				m.insert(make_pair((*it)->getKey(), (*it)->getName()));
				delete *it;
			}
			return m;
		}

		std::string MessagesModule::getLevelLabel( const AlarmLevel& level )
		{
			switch (level)
			{
			case ALARM_LEVEL_INFO : return "Complémentaire";
			case ALARM_LEVEL_WARNING : return "Prioritaire";
			default: return "Inconnu";
			}
		}

		void initialize()
		{

		}
	}
}
