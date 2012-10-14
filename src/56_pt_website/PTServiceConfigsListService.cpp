
//////////////////////////////////////////////////////////////////////////////////////////
///	PTServiceConfigsListService class implementation.
///	@file PTServiceConfigsListService.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PTServiceConfigsListService.hpp"

#include "PTServiceConfig.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,pt_website::PTServiceConfigsListService>::FACTORY_KEY = "pt_service_configs_list";
	
	namespace pt_website
	{
		const string PTServiceConfigsListService::TAG_CONFIG = "config";
		


		ParametersMap PTServiceConfigsListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void PTServiceConfigsListService::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
			{
				try
				{
					_configs.push_back(
						Env::GetOfficialEnv().get<PTServiceConfig>(
							map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					)	);
				}
				catch(ObjectNotFoundException<PTServiceConfig>&)
				{
					throw RequestException("No such config");
				}
			}
			else
			{
				BOOST_FOREACH(
					const Registry<PTServiceConfig>::Map::value_type& item,
					Env::GetOfficialEnv().getRegistry<PTServiceConfig>()
				){
					_configs.push_back(item.second);
				}
			}
		}



		ParametersMap PTServiceConfigsListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			BOOST_FOREACH(const shared_ptr<const PTServiceConfig>& config, _configs)
			{
				shared_ptr<ParametersMap> configPM(new ParametersMap);

				config->toParametersMap(*configPM, true);

				map.insert(TAG_CONFIG, configPM);
			}

			return map;
		}
		
		
		
		bool PTServiceConfigsListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PTServiceConfigsListService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
