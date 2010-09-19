
//////////////////////////////////////////////////////////////////////////////////////////
/// WebpageNextFunction class header.
///	@file WebpageNextFunction.hpp
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_WebpageNextFunction_H__
#define SYNTHESE_WebpageNextFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		///	36.15 Function : WebpageNextFunction.
		///	@ingroup m36Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : WebpageNextFunction
		///
		/// Parameters :
		///	<ul>
		///		<li>page_id : id of the page to read. If undefined, the current page is chosen</li>
		///	</ul>
		class WebpageNextFunction:
			public util::FactorableTemplate<server::Function,WebpageNextFunction>
		{
		public:
			static const std::string PARAMETER_PAGE_ID;
			
		protected:
			//! \name Pparameters
			//@{
				boost::shared_ptr<const Webpage> _webpage;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			server::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);
			
			
		public:
			//! @name Setters
			//@{
				void setWebpage(boost::shared_ptr<const Webpage> value) { _webpage = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual void run(std::ostream& stream, const server::Request& request) const;
			
			
			
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
	}
}

#endif // SYNTHESE_WebpageNextFunction_H__
