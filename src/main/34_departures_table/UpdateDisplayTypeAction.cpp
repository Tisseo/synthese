
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

#include "01_util/Conversion.h"

#include "02_db/DBEmptyResultException.h"

#include "11_interfaces/Interface.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/UpdateDisplayTypeAction.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace interfaces;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departurestable::UpdateDisplayTypeAction>::FACTORY_KEY("updatedisplaytype");
	}

	namespace departurestable
	{
		const string UpdateDisplayTypeAction::PARAMETER_ID = Action_PARAMETER_PREFIX + "dtu_id";
		const string UpdateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "dtu_name";
		const string UpdateDisplayTypeAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "dtu_interf";
		const string UpdateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "dtu_rows";
		const string UpdateDisplayTypeAction::PARAMETER_MAX_STOPS_NUMBER(Action_PARAMETER_PREFIX + "dtu_stops");

		ParametersMap UpdateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_dt.get())
				map.insert(make_pair(PARAMETER_ID, Conversion::ToString(_dt->getKey())));
			map.insert(make_pair(PARAMETER_NAME, _name));
			if (_interface.get())
				map.insert(make_pair(PARAMETER_INTERFACE_ID, Conversion::ToString(_interface->getKey())));
			map.insert(make_pair(PARAMETER_ROWS_NUMBER, Conversion::ToString(_rows_number)));
			map.insert(make_pair(PARAMETER_MAX_STOPS_NUMBER, Conversion::ToString(_max_stops_number)));
			return map;
		}

		void UpdateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Display type ID
			uid id(Request::getUidFromParameterMap(map, PARAMETER_ID, true, FACTORY_KEY));
			try
			{
				_dt = DisplayTypeTableSync::get(id);
			}
			catch (DBEmptyResultException<DisplayType>&)
			{
				throw ActionException("Display Type not found");
			}

			// Name
			_name = Request::getStringFormParameterMap(map, PARAMETER_NAME, true, FACTORY_KEY);
			if (_name != _dt->getName())
			{
				if (_name.empty())
					throw ActionException("Le nom ne peut �tre vide.");

				vector<shared_ptr<DisplayType> > v(DisplayTypeTableSync::search(_name, 0, 1));
				if (!v.empty())
					throw ActionException("Un type portant le nom sp�cifi� existe d�j�. Veuillez utiliser un autre nom.");
			}

			// Rows number
			_rows_number = Request::getIntFromParameterMap(map, PARAMETER_ROWS_NUMBER, true, FACTORY_KEY);
			if (_rows_number < 0)
				throw ActionException("Un nombre positif de lignes doit �tre choisi");

			// Interface
			id = Request::getUidFromParameterMap(map, PARAMETER_INTERFACE_ID, true, FACTORY_KEY);
			if (!Interface::Contains(id))
				throw ActionException("Interface not found");
			_interface = Interface::Get(id);

			// Max stops number
			_max_stops_number = Request::getIntFromParameterMap(map, PARAMETER_MAX_STOPS_NUMBER, true, FACTORY_KEY);
			if (_max_stops_number < UNKNOWN_VALUE)
				throw ActionException("Un nombre positif d'arr�ts interm�diaires doit �tre choisi");

		}

		void UpdateDisplayTypeAction::run()
		{
			// Log entry content
			stringstream log;
			if (_dt->getName() != _name)
				log << " - Nom : " << _dt->getName() << " => " << _name;
			if (_dt->getInterface() != _interface)
				log << " - Interface : " << _dt->getInterface()->getName() << " => " << _interface->getName();
			if (_dt->getRowNumber() != _rows_number)
				log << " - Nombre de lignes : " << _dt->getRowNumber() <<  " => " << _rows_number;
			if (_dt->getMaxStopsNumber() != _max_stops_number)
				log << " - Nombre d'arr�ts interm�diaires : " << _dt->getMaxStopsNumber() << " => " << _max_stops_number;

			// Update
			_dt->setName(_name);
			_dt->setInterface(_interface);
			_dt->setRowNumber(_rows_number);
			_dt->setMaxStopsNumber(_max_stops_number);
			DisplayTypeTableSync::save(_dt.get());

			// Log
			ArrivalDepartureTableLog::addUpdateTypeEntry(_dt, _request->getUser(), log.str());
		}
	}
}
