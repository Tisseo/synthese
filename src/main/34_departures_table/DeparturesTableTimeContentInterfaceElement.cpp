
/** DeparturesTableTimeContentInterfaceElement class implementation.
	@file DeparturesTableTimeContentInterfaceElement.cpp

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

#include <vector>

#include "04_time/DateTime.h"

#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

#include "15_env/LineMarkerInterfacePage.h"

#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DeparturesTableTimeContentInterfaceElement.h"
#include "34_departures_table/DepartureTableRowInterfacePage.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace time;

	namespace departurestable
	{
		DeparturesTableTimeContentInterfaceElement::DeparturesTableTimeContentInterfaceElement()
			: _zeroVIE(NULL)
			, _beforeIfNext(NULL)
			, _afterIfNext(NULL)
		{
			
		}

		DeparturesTableTimeContentInterfaceElement::~DeparturesTableTimeContentInterfaceElement()
		{
			delete _zeroVIE;
			delete _beforeIfNext;
			delete _afterIfNext;
		}

		void DeparturesTableTimeContentInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 3)
				throw InterfacePageException("Malformed departure table time cell declaration");

			_zeroVIE = vel.front();
			_beforeIfNext = vel.front();
			_afterIfNext = vel.front();
		}

		void DeparturesTableTimeContentInterfaceElement::display(ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			const synthese::time::DateTime& __Moment = ((const DeparturesTableModule::ArrivalDepartureRow*) object)->first.realDepartureTime;
			std::string __Zero = _zeroVIE->getValue(parameters, object, request);
			std::string __AvantSiImminent = _beforeIfNext->getValue(parameters, object, request);
			std::string __ApresSiImminent = _afterIfNext->getValue(parameters, object, request);

			synthese::time::DateTime __Maintenant;
			__Maintenant.updateDateTime( synthese::time::TIME_CURRENT );
			int __Duree = __Moment - __Maintenant;

			if ( __Duree <= 1 )
				stream << __AvantSiImminent;
			if ( __Moment.getHour ().getHours() < 10 )
				stream << __Zero;
			stream << __Moment.getHour ();
			if ( __Duree <= 1 )
				stream << __ApresSiImminent;
		}
	}
}
