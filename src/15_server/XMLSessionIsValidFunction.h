//////////////////////////////////////////////////////////////////////////////////////////
/// XMLSessionIsValidFunction class header.
///	@file XMLSessionIsValidFunction.h
///	@author Hugues Romain (RCSmobility)
///	@date 2009-2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_XMLSessionIsValidFunction_H__
#define SYNTHESE_XMLSessionIsValidFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace server
	{
		//////////////////////////////////////////////////////////////////////////
		/// Public function to test if a session id represents a valid session.
		/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Session_test
		//////////////////////////////////////////////////////////////////////////
		///	@author Hugues Romain (RCSmobility)
		///	@date 2009-2011
		///	@ingroup m15Functions refFunctions
		class XMLSessionIsValidFunction:
			public util::FactorableTemplate<server::Function,XMLSessionIsValidFunction>
		{
		public:
			static const std::string PARAMETER_SESSION_ID_TO_CONTROL;
			static const std::string PARAMETER_CMS_TEMPLATE_ID;

		private:
			static const std::string TAG_VALID_SESSION;
			static const std::string ATTR_ID;
			static const std::string ATTR_VALID;

			//! \name Page parameters
			//@{
				std::string _sessionIdToControl;
				boost::shared_ptr<const cms::Webpage> _cmsTemplate;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Session_test#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain (RCSmobility)
			/// @date 2009-2011
			server::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Session_test#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain (RCSmobility)
			/// @date 2009-2011
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);


		public:



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Session_test#XML-Response
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to display the content on.
			/// @author Hugues Romain (RCSmobility)
			/// @date 2009-2011
			virtual void run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues Romain (RCSmobility)
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain (RCSmobility)
			/// @date 2009-2011
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_XMLSessionIsValidFunction_H__
