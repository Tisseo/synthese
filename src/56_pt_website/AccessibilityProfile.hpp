
/** AccessibilityProfile class header.
	@file AccessibilityProfile.hpp

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

#ifndef SYNTHESE_pt_website_AccessibilityProfile_hpp__
#define SYNTHESE_pt_website_AccessibilityProfile_hpp__

#include "Object.hpp"

#include "AccessParameters.h"
#include "GraphTypes.h"
#include "PTServiceConfig.hpp"
#include "StandardFields.hpp"

namespace synthese
{
	FIELD_TYPE(UserClass, int)
	FIELD_TYPE(DistanceThresholds, graph::AccessParameters::DistanceThresholds)
	FIELD_TYPE(Speed, double)

	namespace pt_website
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(PTServiceConfig),
			FIELD(UserClass),
			FIELD(DistanceThresholds),
			FIELD(Speed)
		> AccessibilityProfileRecord;

		/** AccessibilityProfile class.
			@ingroup m56
		*/
		class AccessibilityProfile:
			public Object<AccessibilityProfile, AccessibilityProfileRecord>
		{
		public:
			typedef util::Registry<AccessibilityProfile> Registry;

			AccessibilityProfile(
				util::RegistryKeyType id=0
			);

			graph::AccessParameters getAccessParameters() const;

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();
		};
}	}

#endif // SYNTHESE_pt_website_AccessibilityProfile_hpp__

