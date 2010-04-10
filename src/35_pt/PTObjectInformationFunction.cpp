
/** PTObjectInformationFunction class implementation.
	@file PTObjectInformationFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "PTObjectInformationFunction.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"
#include "LineTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "City.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,pt::PTObjectInformationFunction>::FACTORY_KEY("PTObjectInformationFunction");
	
	namespace pt
	{
		const string PTObjectInformationFunction::PARAMETER_INFO("info");
		
		ParametersMap PTObjectInformationFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void PTObjectInformationFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			int tableId(decodeTableId(id));

			if(tableId == TransportNetworkTableSync::TABLE.ID)
			{
				_network = Env::GetOfficialEnv().get<TransportNetwork>(id);
			}
			else if(tableId == CommercialLineTableSync::TABLE.ID)
			{
				_line = Env::GetOfficialEnv().get<CommercialLine>(id);
			}
			else if(tableId == LineTableSync::TABLE.ID)
			{
				_route = Env::GetOfficialEnv().get<Line>(id);
			}
			else if(ConnectionPlaceTableSync::TABLE.ID)
			{
				_stop = Env::GetOfficialEnv().get<PublicTransportStopZoneConnectionPlace>(id);
			}

			_info = map.get<string>(PARAMETER_INFO);
		}



		void PTObjectInformationFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_network.get())
			{
				if(_info == "name")
				{
					stream << _network->getName();
				}
			}
			else if(_line.get())
			{
				if(_info == "short_name")
				{
					stream << _line->getShortName();
				}
				else if(_info == "name")
				{
					stream << _line->getName();
				}
				else if(_info == "style")
				{
					stream << _line->getStyle();
				}
				else if(_info == "image")
				{
					stream << _line->getImage();
				}
				else if(_info == "network_id" && _line->getNetwork())
				{
					stream << _line->getNetwork()->getKey();
				}
			}
			else if(_route.get())
			{
				if(_info == "line_id" && _route->getCommercialLine())
				{
					stream << _route->getCommercialLine()->getKey();
				}
			}
			else if(_stop.get())
			{
				if(_info == "city_name" && _stop->getCity())
				{
					stream << _stop->getCity()->getName();
				}
				else if(_info == "name")
				{
					stream << _stop->getName();
				}
			}
		}
		
		
		
		bool PTObjectInformationFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PTObjectInformationFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
