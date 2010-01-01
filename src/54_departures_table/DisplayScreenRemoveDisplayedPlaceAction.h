
/** DisplayScreenRemoveDisplayedPlaceAction class header.
	@file DisplayScreenRemoveDisplayedPlaceAction.h

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

#ifndef SYNTHESE_DisplayScreenRemoveDisplayedPlaceAction_H__
#define SYNTHESE_DisplayScreenRemoveDisplayedPlaceAction_H__

#include "30_server/Action.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace departurestable
	{
		class DisplayScreen;

		/** Action class : removes a displayed place from a displayed screen.
			@ingroup m54Actions refActions
		*/
		class DisplayScreenRemoveDisplayedPlaceAction : public util::FactorableTemplate<server::Action, DisplayScreenRemoveDisplayedPlaceAction>
		{
		public:
			static const std::string PARAMETER_PLACE;
			static const std::string PARAMETER_SCREEN;

		private:
			boost::shared_ptr<DisplayScreen>			_screen;
			boost::shared_ptr<const env::PublicTransportStopZoneConnectionPlace>	_place;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const security::Profile& profile) const;

			void setScreen(const util::RegistryKeyType id);
			void setPlace(const util::RegistryKeyType id);
		};
	}
}

#endif // SYNTHESE_DisplayScreenRemoveDisplayedPlaceAction_H__
