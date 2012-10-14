
/** AccessibilityProfile class implementation.
	@file AccessibilityProfile.cpp

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

#include "AccessibilityProfile.hpp"

namespace synthese
{
	using namespace graph;
	using namespace pt_website;
	using namespace util;

	CLASS_DEFINITION(AccessibilityProfile, "t100_accessibility_profiles", 100)
	FIELD_DEFINITION_OF_TYPE(UserClass, "user_class", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DistanceThresholds, "distance_thresholds", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Speed, "speed", SQL_DOUBLE)

	namespace pt_website
	{
		AccessibilityProfile::AccessibilityProfile(
			RegistryKeyType id
		):	Registrable(id),
			Object<AccessibilityProfile, AccessibilityProfileRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(PTServiceConfig),
					FIELD_VALUE_CONSTRUCTOR(UserClass, USER_PEDESTRIAN),
					FIELD_DEFAULT_CONSTRUCTOR(DistanceThresholds),
					FIELD_VALUE_CONSTRUCTOR(Speed, 0)
			)	)
		{}



		graph::AccessParameters AccessibilityProfile::getAccessParameters() const
		{
			AccessParameters ap(
				get<UserClass>(),
				false,
				false,
				get<DistanceThresholds>(),
				get<Speed>()
			);
			return ap;
		}



		void AccessibilityProfile::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Indexation in the site object
			if(get<PTServiceConfig>())
			{
				get<PTServiceConfig>()->addAccessibilityProfile(*this);
			}
		}



		void AccessibilityProfile::unlink()
		{
			// Indexation in the site object
			if(get<PTServiceConfig>())
			{
				get<PTServiceConfig>()->removeAccessibilityProfile(*this);
			}
		}
}	}

