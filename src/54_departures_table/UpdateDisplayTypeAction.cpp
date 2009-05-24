
/** UpdateDisplayTypeAction class implementation.
@file UpdateDisplayTypeAction.cpp

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

#include "UpdateDisplayTypeAction.h"

#include "DisplayTypeTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "DBLogModule.h"

#include "Interface.h"
#include "InterfaceTableSync.h"

#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace db;
	using namespace dblog;
	using namespace security;
	

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departurestable::UpdateDisplayTypeAction>::FACTORY_KEY("updatedisplaytype");
	}

	namespace departurestable
	{
		const string UpdateDisplayTypeAction::PARAMETER_ID = Action_PARAMETER_PREFIX + "dtu_id";
		const string UpdateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "dtu_na";
		const string UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "dtu_di";
		const string UpdateDisplayTypeAction::PARAMETER_AUDIO_INTERFACE_ID(Action_PARAMETER_PREFIX + "dtu_ai");
		const string UpdateDisplayTypeAction::PARAMETER_MONITORING_INTERFACE_ID(Action_PARAMETER_PREFIX + "dtu_mi");
		const string UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "dtu_ro";
		const string UpdateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER(Action_PARAMETER_PREFIX + "dtu_st");
		const string UpdateDisplayTypeAction::PARAMETER_TIME_BETWEEN_CHECKS(Action_PARAMETER_PREFIX + "dtu_tc");

		ParametersMap UpdateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_dt.get())
				map.insert(PARAMETER_ID, _dt->getKey());
			map.insert(PARAMETER_NAME, _name);
			if (_interface.get())
				map.insert(PARAMETER_INTERFACE_ID, _interface->getKey());
			map.insert(PARAMETER_ROWS_NUMBER, _rows_number);
			map.insert(PARAMETER_MAX_STOPS_NUMBER, _max_stops_number);
			return map;
		}

		void UpdateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			setTypeId(map.getUid(PARAMETER_ID, true, FACTORY_KEY));
			try
			{
				// Name
				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
				if (_name != _dt->getName())
				{
					if (_name.empty())
						throw ActionException("Le nom ne peut �tre vide.");

					Env env;
					DisplayTypeTableSync::Search(env, _name, UNKNOWN_VALUE, 0, 1);
					if (!env.getRegistry<DisplayType>().empty())
						throw ActionException("Un type portant le nom sp�cifi� existe d�j�. Veuillez utiliser un autre nom.");
				}

				// Rows number
				_rows_number = map.getInt(PARAMETER_ROWS_NUMBER, true, FACTORY_KEY);
				if (_rows_number < 0)
				{
					throw ActionException("Un nombre positif de rang�es doit �tre choisi");
				}

				// Interface
				RegistryKeyType id(map.getUid(PARAMETER_INTERFACE_ID, true, FACTORY_KEY));
				if (id > 0)
				{
					_interface = InterfaceTableSync::Get(id, _env);
				}
				id = map.getUid(PARAMETER_AUDIO_INTERFACE_ID, true, FACTORY_KEY);
				if (id > 0)
				{
					_audioInterface = InterfaceTableSync::Get(id, _env);
				}
				id = map.getUid(PARAMETER_MONITORING_INTERFACE_ID, true, FACTORY_KEY);
				if (id > 0)
				{
					_monitoringInterface = InterfaceTableSync::Get(id, _env);
				}

				// Max stops number
				_max_stops_number = map.getInt(PARAMETER_MAX_STOPS_NUMBER, true, FACTORY_KEY);
				if (_max_stops_number < UNKNOWN_VALUE)
				{
					throw ActionException("Un nombre positif d'arr�ts interm�diaires doit �tre choisi");
				}

				// Time between checks
				_timeBetweenChecks = minutes(map.getInt(PARAMETER_TIME_BETWEEN_CHECKS, true, FACTORY_KEY));
				if (_timeBetweenChecks.minutes() < 0)
				{
					throw ActionException("La dur�e entre les tests de supervision doit �tre positive.");
				}
			}
			catch(ObjectNotFoundException<Interface>& e)
			{
				throw ActionException("Interface not found / "+ e.getMessage());
			}
		}

		void UpdateDisplayTypeAction::run()
		{
			// Log entry content
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _dt->getName(), _name);
			DBLogModule::appendToLogIfChange(log, "Interface d'affichage", (_dt->getDisplayInterface() != NULL) ? _dt->getDisplayInterface()->getName() : "(aucune)", (_interface.get() != NULL) ? _interface->getName() : "(aucune)");
			DBLogModule::appendToLogIfChange(log, "Interface de supervision", (_dt->getMonitoringInterface() != NULL) ? _dt->getMonitoringInterface()->getName() : "(aucune)", (_interface.get() != NULL) ? _interface->getName() : "(aucune)");
			DBLogModule::appendToLogIfChange(log, "Interface audio", (_dt->getAudioInterface() != NULL) ? _dt->getAudioInterface()->getName() : "(aucune)", (_interface.get() != NULL) ? _interface->getName() : "(aucune)");
			DBLogModule::appendToLogIfChange(log, "Nombre de lignes", _dt->getRowNumber(), _rows_number);
			DBLogModule::appendToLogIfChange(log, "Nombre d'arr�ts interm�diaires", _dt->getMaxStopsNumber(), _max_stops_number);
			DBLogModule::appendToLogIfChange(log, "Temps entre les contr�les de supervision", to_simple_string(_dt->getTimeBetweenChecks()), to_simple_string(_timeBetweenChecks));

			// Update
			_dt->setName(_name);
			_dt->setDisplayInterface(_interface.get());
			_dt->setMonitoringInterface(_monitoringInterface.get());
			_dt->setAudioInterface(_audioInterface.get());
			_dt->setRowNumber(_rows_number);
			_dt->setMaxStopsNumber(_max_stops_number);
			_dt->setTimeBetweenChecks(_timeBetweenChecks);
			DisplayTypeTableSync::Save(_dt.get());

			// Log
			ArrivalDepartureTableLog::addUpdateTypeEntry(_dt.get(), _request->getUser().get(), log.str());
		}



		void UpdateDisplayTypeAction::setTypeId(
			util::RegistryKeyType id
		){
			try
			{
				// Display type
				_dt = DisplayTypeTableSync::GetEditable(id, _env, UP_LINKS_LOAD_LEVEL);
			}			
			catch (ObjectNotFoundException<DisplayType>& e)
			{
				throw ActionException("Display Type not found / "+ e.getMessage());
			}
		}



		bool UpdateDisplayTypeAction::_isAuthorized(

			) const {
			return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
		}
	}
}
