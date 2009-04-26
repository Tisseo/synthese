
/** Alarm class implementation.
	@file Alarm.cpp

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

#include "Alarm.h"
#include "Scenario.h"
#include "AlarmTemplate.h"

using namespace boost;

namespace synthese
{
	using namespace util;

	namespace messages
	{

		Alarm::Alarm(
			util::RegistryKeyType key,
			const Scenario* scenario
		):	_level(ALARM_LEVEL_INFO),
			_scenario(scenario)
		{
		}

		Alarm::Alarm(
			const Alarm& source
		):	_level(source._level),
			_shortMessage(source._shortMessage),
			_longMessage(source._longMessage),
			_scenario(source._scenario)
		{
		}



		Alarm::Alarm(
			const Alarm& source,
			const Scenario* scenario
		):	_level(source._level),
			_shortMessage(source._shortMessage),
			_longMessage(source._longMessage),
			_scenario(scenario)
		{

		}

		const AlarmLevel& Alarm::getLevel () const
		{
			return _level;
		}


		void Alarm::setLevel (const AlarmLevel& level)
		{
			_level = level;
		}


		void Alarm::setLongMessage( const std::string& message )
		{
			_longMessage = message;
		}

		void Alarm::setShortMessage( const std::string& message )
		{
			_shortMessage = message;
		}

		const std::string& Alarm::getLongMessage() const
		{
			return _longMessage;
		}

		const std::string& Alarm::getShortMessage() const
		{
			return _shortMessage;
		}

		Alarm::~Alarm()
		{

		}



		void Alarm::setScenario( const Scenario* scenario )
		{
			_scenario = scenario;
		}



		const Scenario* Alarm::getScenario() const
		{
			return _scenario;
		}
	}
}
