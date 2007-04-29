
/** RoutePlannerSheetLineInterfacePage class header.
	@file RoutePlannerSheetLineInterfacePage.h

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

#ifndef SYNTHESE_RoutePlannerSheetLineInterfacePage_H__
#define SYNTHESE_RoutePlannerSheetLineInterfacePage_H__


#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}

	namespace server
	{
		class Request;
	}



	namespace routeplanner
	{
		/** Line of schedule sheet.
			@code schedule_sheet_row @endcode
			@ingroup m33Pages refPages
		*/
		class RoutePlannerSheetLineInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Display of line of schedule sheet.
				@param stream Stream to write on
				@param text (0) Text containing cells
				@param alternateColor (1) indicates if the line rank is odd or even. Values :
					- false/0 = odd
					- true/1 = even
				@param place (Place* object) : Corresponding place of the line
				@param site Displayed site
			*/
			void display( std::ostream& stream, 
				      const std::string& text, 
				      bool alternateColor, 
					  interfaces::VariablesMap& variables,
				      const synthese::env::ConnectionPlace* place,
				      const server::Request* request = NULL ) const;


		};
	}
}
#endif // SYNTHESE_RoutePlannerSheetLineInterfacePage_H__
