
/** ChrInterfaceElement class header.
	@file ChrInterfaceElement.h

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

#ifndef SYNTHESE_ChrInterfaceElement_H__
#define SYNTHESE_ChrInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{

		/** ASCII character Value Interface Element Class.
			@ingroup m11Library refLibrary

			Usage :
			@code {{chr <number>}} @endcode

			The parameters are :
				-# number : Decimal number of the character to output

			The output is the character corresponding to the specified number in the ASCII table.

			Example :
			@code {{chr 77}} @endcode
			will output
			@code M @endcode
		*/
		class ChrInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, ChrInterfaceElement>
		{
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _asciiCode;

		public:
			/** Parameters parser.
				The parser copies the ValueElementList as is.
			*/
			void storeParameters (ValueElementList& vel);
			std::string display(
				std::ostream&
				, const ParametersVector&,
				interfaces::VariablesMap& variables,
				const void* object = NULL,
				const server::Request* request = NULL
				) const;

		};
	}
}

#endif // SYNTHESE_ChrInterfaceElement_H__
