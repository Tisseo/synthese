
/** CreateDisplayScreenAction class header.
	@file CreateDisplayScreenAction.h

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

#ifndef SYNTHESE_CreateDisplayScreenAction_H__
#define SYNTHESE_CreateDisplayScreenAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace departurestable
	{
		class DisplayScreen;
		class DisplayScreenCPU;

		/** Display screen creation action class.
			@ingroup m54Actions refActions
		*/
		class CreateDisplayScreenAction : public util::FactorableTemplate<server::Action, CreateDisplayScreenAction>
		{
		public:
			static const std::string PARAMETER_LOCALIZATION_ID;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_CPU_ID;

		private:
			boost::shared_ptr<const DisplayScreen>			_template;
			boost::shared_ptr<const env::PublicTransportStopZoneConnectionPlace>	_place;
			boost::shared_ptr<const DisplayScreenCPU>		_cpu;

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

			void setPlace(util::RegistryKeyType id);
			void setCPU(util::RegistryKeyType id);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_CreateDisplayScreenAction_H__
