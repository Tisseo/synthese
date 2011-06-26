
/** NoExportPolicy class header.
	@file NoExportPolicy.hpp

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

#ifndef SYNTHESE_impex_NoExportPolicy_hpp__
#define SYNTHESE_impex_NoExportPolicy_hpp__

#include "Exporter.hpp"

namespace synthese
{
	namespace impex
	{
		/** NoExportPolicy class.
			@ingroup m16
		*/
		template<class FF>
		class NoExportPolicy:
			public Exporter
		{
		public:
			static const bool EXPORTABLE;

			virtual void build(std::ostream&) const {}

			virtual std::string getOutputMimeType() const { return "text/plain"; }

			virtual void setFromParametersMap(
				const util::ParametersMap& map
			){}

			virtual util::ParametersMap getParametersMap() const { return util::ParametersMap(); }

			virtual std::string getFileFormatKey() const { return FF::FACTORY_KEY; }
		};

		template<class FF>
		const bool NoExportPolicy<FF>::EXPORTABLE(false);
}	}

#endif // SYNTHESE_impex_NoExportPolicy_hpp__
