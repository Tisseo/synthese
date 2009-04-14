
/** EnvModule class header.
	@file EnvModule.h

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

#ifndef SYNTHESE_EnvModule_H__
#define SYNTHESE_EnvModule_H__

#include <vector>
#include <set>
#include <utility>

#include "ModuleClass.h"
#include "FactorableTemplate.h"
#include "UtilTypes.h"
#include "Env.h"

#include "12_security/Types.h"

namespace synthese
{
	namespace graph
	{
		class Vertex;
		class Journey;
	}
	

	/** 35 Transport network module namespace.
		The env name is due to historical reasons.
	*/
	namespace env
	{
		class NonPermanentService;
		

		/** 35 Transport network module class.
		*/
		class EnvModule : public util::FactorableTemplate<util::ModuleClass, EnvModule>
		{
		private:


		public:
		
			void initialize();

			static boost::shared_ptr<NonPermanentService> FetchEditableService(
				const util::RegistryKeyType& id,
				util::Env& env = util::Env::GetOfficialEnv()
			);

			
			static std::vector<std::pair<uid, std::string> >	getCommercialLineLabels(
				const security::RightsOfSameClassMap& rights 
				, bool totalControl 
				, security::RightLevel neededLevel 
				, bool withAll=false
				);

			static void getNetworkLinePlaceRightParameterList(security::ParameterLabelsVector& m);

			virtual std::string getName() const;
			
			static int GetMaxAlarmLevel(
				const graph::Journey&
			);
		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__
