
/** LogicalStopNameValueInterfaceElement class header.
	@file LogicalStopNameValueInterfaceElement.h

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

#ifndef SYNTHESE_LogicalStopNameValueInterfaceElement_H__
#define SYNTHESE_LogicalStopNameValueInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace env
	{
		/** Logical stop name.
			@code stop_name @endcode
			@ingroup m35Values refLibrary
		*/
		class LogicalStopNameValueInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, LogicalStopNameValueInterfaceElement>
		{
		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _uid;

		public:
			std::string display(
				std::ostream&
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL) const;

			/** Parser.
			@param text Optional parameter :
				- if defined : the uid of the stop to name
				- if empty : the name of the stop provided at runtime as current object
			@todo Handle the designation number
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};
	}
}
#endif // SYNTHESE_LogicalStopNameValueInterfaceElement_H__
