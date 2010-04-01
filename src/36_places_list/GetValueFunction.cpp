
/** GetValueFunction class implementation.
	@file GetValueFunction.cpp
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
#include "GetValueFunction.hpp"
#include "FunctionWithSite.h"
#include "WebPageDisplayFunction.h"
#include "WebPage.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace transportwebsite;

	template<> const string util::FactorableTemplate<Function,server::GetValueFunction>::FACTORY_KEY("@");
	
	namespace server
	{
		const string GetValueFunction::PARAMETER_PARAMETER("p");
		
		ParametersMap GetValueFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_PARAMETER, _parameter);
			return map;
		}

		void GetValueFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_parameter = map.getDefault<string>(PARAMETER_PARAMETER);
		}

		void GetValueFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_parameter == "client_url")
			{
				stream << request.getClientURL();
			}
			else if(_parameter == "host_name")
			{
				stream << request.getHostName();
			}
			else if(_parameter == "site")
			{
				ParametersMap map(request.getParametersMap());
				if(map.getOptional<RegistryKeyType>(FunctionWithSiteBase::PARAMETER_SITE))
				{
					stream << map.get<RegistryKeyType>(FunctionWithSiteBase::PARAMETER_SITE);
				}
				else if(
					dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) &&
					dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get())->getPage().get() &&
					dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get())->getPage()->getRoot()
				){
					stream << dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get())->getPage()->getRoot()->getKey();
				}
			}
			else
			{
				ParametersMap map(request.getParametersMap());
				stream << map.getDefault<string>(_parameter);
			}
		}
		
		
		
		bool GetValueFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string GetValueFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
