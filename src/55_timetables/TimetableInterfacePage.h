
/** TimetableInterfacePage class header.
	@file TimetableInterfacePage.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_TimetableInterfacePage_H__
#define SYNTHESE_TimetableInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	
	namespace timetables
	{
		class Timetable;
		class TimetableGenerator;

		/** TimetableInterfacePage Interface Page Class.
			@ingroup m55Pages refPages
			@author Hugues
			@date 2009

			@code timetable @endcode

			Parameters :
				- 0 : Content
				- 1 : (no content)
				- 2 : Title
				- 3 : Notes content

			Object : TimetableGenerator
		*/
		class TimetableInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, TimetableInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...	
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const timetables::Timetable& object,
				const timetables::TimetableGenerator& generator,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
			
			TimetableInterfacePage();
		};
	}
}

#endif // SYNTHESE_TimetableInterfacePage_H__
