
/** PlaceAlias class header.
	@file PlaceAlias.h

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

#ifndef SYNTHESE_ENV_PLACEALIAS_H
#define SYNTHESE_ENV_PLACEALIAS_H


#include <vector>

#include "IncludingPlace.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"


namespace synthese
{
	namespace env
	{


		/** Place alias.

		 @ingroup m15
		*/
		class PlaceAlias : 
			public synthese::util::Registrable<uid, PlaceAlias>,
			public IncludingPlace
		{

		private:
		        

		public:

			PlaceAlias (const uid id = UNKNOWN_VALUE,
				std::string name = std::string(),
				const Place* aliasedPlace = NULL,
				const City* city = NULL);

			virtual ~PlaceAlias ();


			//! @name Getters/Setters
			//@{

			/** Gets official name of this place.
		     
				@return aliased place official name.
			 */
			const std::string& getOfficialName () const;


			/** Gets aliased place.
			 */
			const Place* getAliasedPlace () const;

			void setAliasedPlace(const Place* place);
			//@}



		};


	}
}

#endif 	    

