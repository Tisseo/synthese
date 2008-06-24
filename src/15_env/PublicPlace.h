
/** PublicPlace class header.
	@file PublicPlace.h

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

#ifndef SYNTHESE_ENV_PUBLICPLACE_H
#define SYNTHESE_ENV_PUBLICPLACE_H

#include "15_env/AddressablePlace.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "01_util/Constants.h"

#include <string>

namespace synthese
{
	namespace env
	{
		class City;


		/** Public place class.
			@ingroup m35
		*/
		class PublicPlace : public synthese::util::Registrable<uid, PublicPlace>, public AddressablePlace
		{

		private:

		protected:

		public:

			PublicPlace (
				const uid id = UNKNOWN_VALUE
				, std::string name = std::string()
				, const City* city = NULL
				);

			virtual ~PublicPlace ();


			//! @name Getters/Setters
			//@{

			//@}

			virtual uid getId() const;
		    
		};
	}
}

#endif 	    
