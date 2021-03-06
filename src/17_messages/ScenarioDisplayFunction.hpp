
//////////////////////////////////////////////////////////////////////////////////////////
/// ScenarioDisplayFunction class header.
///	@file ScenarioDisplayFunction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_ScenarioDisplayFunction_H__
#define SYNTHESE_ScenarioDisplayFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"
#include "ScenarioTemplate.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace messages
	{
		//////////////////////////////////////////////////////////////////////////
		///	17.15 Function : ScenarioDisplayFunction.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Scenario_display
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m17Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		class ScenarioDisplayFunction:
			public util::FactorableTemplate<server::Function,ScenarioDisplayFunction>
		{
		public:
			static const std::string PARAMETER_MAIN_TEMPLATE;
			static const std::string PARAMETER_VARIABLE_TEMPLATE;
			static const std::string PARAMETER_MESSAGE_TEMPLATE;
			static const std::string PARAMETER_OUTPUT_FORMAT;

			static const std::string FORMAT_XML;
			static const std::string FORMAT_JSON;

			static const std::string DATA_VARIABLES;
			static const std::string DATA_MESSAGES;
			static const std::string DATA_RANK;

			static const std::string TAG_SCENARIO;

		protected:
			//! @name Parameters
			//@{
				std::string _outputFormat;
				boost::shared_ptr<const Scenario> _scenario;
			//@}

			//! \name Page parameters
			//@{
				boost::shared_ptr<const cms::Webpage> _mainTemplate;
				boost::shared_ptr<const cms::Webpage> _variableTemplate;
				boost::shared_ptr<const cms::Webpage> _messageTemplate;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Scenario_display#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Scenario_display#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Scenario_display#Main-page
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_ScenarioDisplayFunction_H__
