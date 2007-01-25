
/** AdminRequest class implementation.
	@file AdminRequest.cpp

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

#include <sstream>

#include "01_util/Conversion.h"
#include "01_util/FactoryException.h"

#include "11_interfaces/Interface.h"

#include "30_server/RequestException.h"

#include "32_admin/HomeAdmin.h"
#include "32_admin/AdminInterfacePage.h"
#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminRequest.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace interfaces;

	namespace admin
	{
		const std::string AdminRequest::PARAMETER_PAGE = "rub";
		const std::string AdminRequest::PARAMETER_ACTION_FAILED_PAGE = "afp";

		AdminRequest::AdminRequest()
			: Request(Request::NEEDS_SESSION)
			, _page(NULL), _actionFailedPage(NULL)
		{}

		AdminRequest::ParametersMap AdminRequest::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			map.insert(make_pair(PARAMETER_OBJECT_ID, Conversion::ToString(_object_id)));
			return map;
		}

		void AdminRequest::setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Page
			it = map.find(PARAMETER_PAGE);
			try
			{
				AdminInterfaceElement* page = (it == map.end())
					? Factory<AdminInterfaceElement>::create<HomeAdmin>()
					: Factory<AdminInterfaceElement>::create(it->second);
				page->setFromParametersMap(map);
				_page = page;
			}
			catch (FactoryException<AdminInterfaceElement> e)
			{
				throw RequestException("Page not found");
			}

			// Action failed page
			it = map.find(PARAMETER_ACTION_FAILED_PAGE);
			try
			{
				if (it == map.end())
				{
					_actionFailedPage = _page;
				}
				else
				{
					AdminInterfaceElement* page = Factory<AdminInterfaceElement>::create(it->second);
					page->setFromParametersMap(map);
					_actionFailedPage = page;
				}				
			}
			catch (FactoryException<AdminInterfaceElement> e)
			{
				throw RequestException("Action failed page not found");
			}

			// Parameters saving
			_parameters = map;
			
		}

		void AdminRequest::run( std::ostream& stream ) const
		{
			const AdminInterfacePage* aip;
			try
			{
				aip = _site->getInterface()->getPage<AdminInterfacePage>();
			}
			catch (Exception e)
			{
				throw RequestException("Admin interface page not implemented in database");
			}
			aip->display(stream, _page, _object_id, this);
		}

		AdminRequest::~AdminRequest()
		{
			if (_page != _actionFailedPage)
				delete _actionFailedPage;
			delete _page;
		}

		void AdminRequest::setPage( const AdminInterfaceElement* aie )
		{
			_page = aie;
		}

		std::string AdminRequest::getHTMLFormHeader( const std::string& name ) const
		{
			stringstream s;
			s << Request::getHTMLFormHeader(name);
			s << "<input type=\"hidden\" name=\"" << PARAMETER_PAGE << "\" value=\"" << _page->getFactoryKey() << "\" />";
			return s.str();
		}

		const AdminInterfaceElement* AdminRequest::getPage() const
		{
			return _page;
		}

		void AdminRequest::setParameter( const std::string& name, const std::string value )
		{
			ParametersMap::iterator it = _parameters.find(name);
			if (it == _parameters.end())
				_parameters.insert(make_pair(name, value));
			else
				it->second = value;
		}

		
	}
}
