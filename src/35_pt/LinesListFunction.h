////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class header.
///	@file LinesListFunction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_LinesListFunction_H__
#define SYNTHESE_LinesListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace transportwebsite
	{
		class WebPage;
	}

	namespace pt
	{
		class TransportNetwork;
	
		////////////////////////////////////////////////////////////////////
		/// 35.15 Function : Lines list public Function class.
		/// @author Hugues Romain
		///	@date 2008
		///	@ingroup m35Functions refFunctions
		//////////////////////////////////////////////////////////////////////////
		/// <h2>Usage</h2>
		///	Key : LinesListFunction2
		///
		///	Parameters :
		///	<ul>
		///		<li>fonction=LinesListFunction2</li>
		///		<li>ni : id of the network of the lines</li>
		///		<li>pi (optional) : id of the display template to use for each line (will be called by LineMarkerInterfacePage).
		///		If pi is not specified, the standard XML output is generated instead.</li>
		///	</ul>
		///
		/// <h2>Standard XML output</h2>
		///	<h3>Description</h3>
		/// 
		///	@image html LinesListFunction.png
		///
		///	<h3>Download</h3>
		///
		///	<ul>
		///		<li><a href="include/35_pt/LinesListFunction.xsd">XML output schema</a></li>
		///	</ul>
		class LinesListFunction:
			public util::FactorableTemplate<server::Function,LinesListFunction>
		{
		 public:
			static const std::string PARAMETER_NETWORK_ID;
			static const std::string PARAMETER_PAGE_ID;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const pt::TransportNetwork> _network;
				boost::shared_ptr<const transportwebsite::WebPage> _page;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream, const server::Request& request) const;

			void setNetworkId(
				util::RegistryKeyType id
			);

			virtual bool isAuthorized(const server::Session* session) const;


			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_LinesListFunction_H__
