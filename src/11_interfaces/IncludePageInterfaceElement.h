
/** IncludePageInterfaceElement class header.
	@file IncludePageInterfaceElement.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_IncludePageInterfaceElement_H__
#define SYNTHESE_IncludePageInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		/** Page inclusion interface element.

			Parameters :
				- 0 : Class code
				- 1 : Page code (optional)
				- ... : parameters to transmit at the subpage
		*/
		class IncludePageInterfaceElement : public util::FactorableTemplate<LibraryInterfaceElement, IncludePageInterfaceElement>
		{
		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement>	_class_code;
			boost::shared_ptr<interfaces::LibraryInterfaceElement>	_page_code;
			ValueElementList							_parameters;

		public:
			std::string display(std::ostream& stream, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;
			void storeParameters(ValueElementList& vel);
		};
	}
}

#endif // SYNTHESE_IncludePageInterfaceElement_H__

