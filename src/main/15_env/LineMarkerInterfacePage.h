
/** LineMarkerInterfacePage class header.
	@file LineMarkerInterfacePage.h

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

#ifndef SYNTHESE_LineMarkerInterfacePage_H__
#define SYNTHESE_LineMarkerInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	namespace env
	{
		class CommercialLine;

		/** Line marker.
			@ingroup 15Library refLibrary
			@code line_marker @endcode

			Parameters :
				- 0 : table opening HTML code
				- 1 : table closing HTML code
				- 2 : width of the marker (pixels)
				- 3 : height of the marker (pixels)

			Object : CommercialLine
		*/
		class LineMarkerInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Display of line of schedule sheet.
				@param stream Stream to write on
				@param vars Execution variables
				@param tableOpeningHTML HTML d'ouverture du tableau (RIEN = Pas d'ouverture du tableau)
				@param tableClosingHTML HTML de fermeture du tableau (RIEN = Pas de fermeture du tableau)
				@param pixelWidth Largeur de la case en pixels
				@param pixelHeight Hauteur de la case en pixels
				@param commercialLine Commercial Line to display
				@param request Request
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& vars
				, const std::string& tableOpeningHTML
				, const std::string& tableClosingHTML
				, int pixelWidth
				, int pixelHeight
				, const env::CommercialLine* commercialLine
				, const server::Request* request = NULL
			) const;

		};
	}
}
#endif // SYNTHESE_LineMarkerInterfacePage_H__
