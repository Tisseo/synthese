
/** AddPreselectionPlaceToDisplayScreen class implementation.
	@file AddPreselectionPlaceToDisplayScreen.cpp

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

#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "ArrivalDepartureTableRight.h"
#include "AddPreselectionPlaceToDisplayScreen.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "ArrivalDepartureTableLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace db;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<Action, departurestable::AddPreselectionPlaceToDisplayScreen>::FACTORY_KEY("apptds");
	}
	
	namespace departurestable
	{
		const string AddPreselectionPlaceToDisplayScreen::PARAMETER_SCREEN_ID(
			Action_PARAMETER_PREFIX + "s"
		);
		const string AddPreselectionPlaceToDisplayScreen::PARAMETER_PLACE(
			Action_PARAMETER_PREFIX + "p"
		);


		ParametersMap AddPreselectionPlaceToDisplayScreen::getParametersMap() const
		{
			ParametersMap map;
			if(_screen.get()) map.insert(PARAMETER_SCREEN_ID, _screen->getKey());
			return map;
		}

		void AddPreselectionPlaceToDisplayScreen::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCREEN_ID),
					*_env
				);
				_place = ConnectionPlaceTableSync::Get(
					map.get<RegistryKeyType>(PARAMETER_PLACE),
					*_env
				);
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
			catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
			{
				throw ActionException("Specified place not found");
			}
		}

		void AddPreselectionPlaceToDisplayScreen::run()
		{
			_screen->addForcedDestination(_place.get());
			DisplayScreenTableSync::Save(_screen.get());
			
			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				*_screen,
				"Ajout de l'arr�t de pr�s�lection "+ _place->getFullName(),
				*_request->getUser()
			);
		}



		bool AddPreselectionPlaceToDisplayScreen::_isAuthorized(
		) const {
			return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
		}
		
		
		void AddPreselectionPlaceToDisplayScreen::setScreen(boost::shared_ptr<DisplayScreen> value)
		{
			_screen = value;
		}
	
	
		void AddPreselectionPlaceToDisplayScreen::setScreen(
			boost::shared_ptr<const DisplayScreen> value)
		{
			_screen = const_pointer_cast<DisplayScreen>(value);
		}
	}
}
