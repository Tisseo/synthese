
/** ScenarioSentAlarm class implementation.
	@file ScenarioSentAlarm.cpp

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

#include "ScenarioSentAlarm.h"
#include "AlarmTemplate.h"
#include "SentScenario.h"
#include "Registry.h"

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const std::string Registry<messages::ScenarioSentAlarm>::KEY("ScenarioSentAlarm");
	}


	namespace messages
	{
		ScenarioSentAlarm::ScenarioSentAlarm(
			const SentScenario& scenario,
			const AlarmTemplate& source
		):	SentAlarm(),
			_scenario(&scenario),
			_template(&source)
		{
			setLevel(source.getLevel());
			setShortMessage(source.getShortMessage());
			setLongMessage(source.getLongMessage());
		}



		ScenarioSentAlarm::ScenarioSentAlarm(
			RegistryKeyType key,
			const SentScenario* scenario
		):	Registrable(key),
			SentAlarm(),
			_scenario(scenario),
			_template(NULL)
		{
		}



		ScenarioSentAlarm::ScenarioSentAlarm(
			const SentScenario& scenario,
			const ScenarioSentAlarm& source
		):	SentAlarm(),
			_scenario(&scenario),
			_template(source._template)
		{
			setLevel(source.getLevel());
			setShortMessage(source.getShortMessage());
			setLongMessage(source.getLongMessage());
		}



		ScenarioSentAlarm::~ScenarioSentAlarm()
		{
		}



		bool ScenarioSentAlarm::getIsEnabled() const
		{
			return _scenario->getIsEnabled();
		}



		const time::DateTime& ScenarioSentAlarm::getPeriodStart() const
		{
			return _scenario->getPeriodStart();
		}

		const time::DateTime& ScenarioSentAlarm::getPeriodEnd() const
		{
			return _scenario->getPeriodEnd();
		}

		const SentScenario* ScenarioSentAlarm::getScenario() const
		{
			return _scenario;
		}

		void ScenarioSentAlarm::setScenario( const SentScenario* scenario )
		{
			_scenario = scenario;
		}
		
		const AlarmTemplate* ScenarioSentAlarm::getTemplate(
		) const {
			return _template;
		}
		
		void ScenarioSentAlarm::setTemplate(const AlarmTemplate* value
		){
			_template = value;
		}
	}
}
