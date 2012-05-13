
//////////////////////////////////////////////////////////////////////////////////////////
///	ServicesListService class header.
///	@file ServicesListService.hpp
///	@author Hugues Romain
///	@date 2012
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

#ifndef SYNTHESE_ServicesListService_H__
#define SYNTHESE_ServicesListService_H__

#include "FactorableTemplate.h"
#include "FunctionWithSite.h"

namespace synthese
{
	namespace pt
	{
		class CommercialLine;

		//////////////////////////////////////////////////////////////////////////
		///	35.15 Function : ServicesListService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Services List
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m35Functions refFunctions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.4.0
		class ServicesListService:
			public util::FactorableTemplate<cms::FunctionWithSite<false>, ServicesListService>
		{
		public:
			static const std::string PARAMETER_WAYBACK;
			static const std::string PARAMETER_PAGE_ID;
			static const std::string PARAMETER_DISPLAY_DATE;

			static const std::string DATA_ID;
			static const std::string DATA_DEPARTURE_SCHEDULE;
			static const std::string DATA_DEPARTURE_PLACE_NAME;
			static const std::string DATA_ARRIVAL_PLACE_NAME;
			static const std::string DATA_RUNS_AT_DATE;
			static const std::string DATA_SERVICE;
			
		protected:
			//! \name Page parameters
			//@{
				boost::logic::tribool _wayBack;
				const cms::Webpage* _page;
				boost::shared_ptr<const CommercialLine> _line;
				boost::gregorian::date _displayDate;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Services List#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.4.0
			util::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Services List#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.4.0
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);
			
			
		public:
			ServicesListService();


			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2012
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2012
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2012
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_ServicesListService_H__

