
/** DisplayScreenContentRequest class header.
	@file DisplayScreenContentRequest.h

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

#ifndef SYNTHESE_DisplayScreenContentRequest_H__
#define SYNTHESE_DisplayScreenContentRequest_H__

#include "04_time/DateTime.h"

#include "30_server/Request.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;

		/** DisplayScreenContentRequest class.
			@ingroup m34
		*/
		class DisplayScreenContentRequest : public server::Request
		{
			static const std::string PARAMETER_DISPLAY_SCREEN;
			static const std::string PARAMETER_DATE;
			
			//! \name Page parameters
			//@{
				const DisplayScreen*	_screen;
				time::DateTime			_date;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void setFromParametersMap(const Request::ParametersMap& map);

		public:
			DisplayScreenContentRequest();
			~DisplayScreenContentRequest();

			void	setDisplayScreen(const DisplayScreen* screen);
			

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream) const;
		};
	}
}
#endif // SYNTHESE_DisplayScreenContentRequest_H__
