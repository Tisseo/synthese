
/** UpdateRightAction class header.
	@file UpdateRightAction.h

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

#ifndef SYNTHESE_UpdateRightAction_H__
#define SYNTHESE_UpdateRightAction_H__

#include <boost/shared_ptr.hpp>

#include "12_security/Types.h"
#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Right update action class.
			@ingroup m12Actions refActions
		*/
		class UpdateRightAction :
			public util::FactorableTemplate<server::Action,UpdateRightAction>
		{
		public:
			static const std::string PARAMETER_PROFILE_ID;
			static const std::string PARAMETER_RIGHT_CODE;
			static const std::string PARAMETER_RIGHT_PARAMETER;
			static const std::string PARAMETER_PUBLIC_VALUE;
			static const std::string PARAMETER_PRIVATE_VALUE;

		private:
			boost::shared_ptr<Profile>	_profile;
			boost::shared_ptr<Right>	_right;
			RightLevel				_publicLevel;
			RightLevel				_privateLevel;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:

			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const security::Profile& profile) const;
			
			void setProfile(boost::shared_ptr<const Profile> value);
		};
	}
}

#endif // SYNTHESE_UpdateRightAction_H__
