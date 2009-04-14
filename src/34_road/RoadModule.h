
/** RoadModule class header.
	@file RoadModule.h

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

#ifndef SYNTHESE_RoadModule_H__
#define SYNTHESE_RoadModule_H__


#include "GraphModuleTemplate.h"


namespace synthese
{
	/**	@defgroup m34Actions 34 Actions
		@ingroup m34

		@defgroup m34Pages 34 Pages
		@ingroup m34

		@defgroup m34Library 34 Interface library
		@ingroup m34

		@defgroup m34Functions 34 Functions
		@ingroup m34

		@defgroup m34LS 34 Table synchronizers
		@ingroup m34

		@defgroup m34Admin 34 Administration pages
		@ingroup m34

		@defgroup m34Rights 34 Rights
		@ingroup m34

		@defgroup m34Logs 34 DB Logs
		@ingroup m34

		@defgroup m34 34 Road network
		@ingroup m3
	@{
	*/

	/** 34 Road network module namespace.
	*/
	namespace road
	{
		/** 34 Transport network module class.
		*/
		class RoadModule:
			public graph::GraphModuleTemplate<RoadModule>
		{


		public:
		
			void initialize();
			

			virtual std::string getName() const;
			
		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__
