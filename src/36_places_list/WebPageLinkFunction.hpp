
//////////////////////////////////////////////////////////////////////////////////////////
/// WebPageLinkFunction class header.
///	@file WebPageLinkFunction.hpp
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

#ifndef SYNTHESE_WebPageLinkFunction_H__
#define SYNTHESE_WebPageLinkFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"
#include "ParametersMap.h"

namespace synthese
{
	namespace transportwebsite
	{
		class WebPage;

		//////////////////////////////////////////////////////////////////////////
		///	56.15 Function : WebPageLinkFunction.
		///	@ingroup m56Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : link
		///
		/// Parameters :
		///	<ul>
		///		<li>target : id of a page</li>
		///		<li>text : If empty, use the target page name</li>
		///		<li>(other parameters) : other parameters are transmitted into the link without any transformation</li>
		///	</ul>
		///
		/// Examples :
		///	<ul>
		///		<li>target=123 : <pre><a href="fonction=page&p=123">Name of the 123 page</a></pre></li>
		///		<li>target=123&roid=12 : <pre><a href="fonction=page&p=123&roid=12">Name of the 123 page</a></pre></li>
		///		<li>target=123&text=toto : <pre><a href="fonction=page&p=123">toto</a></pre></li>
		///		<li>target=123&text=toto&roid=12 : <pre><a href="fonction=page&p=123&roid=12">toto</a></pre></li>
		///	</ul>
		class WebPageLinkFunction:
			public util::FactorableTemplate<server::Function,WebPageLinkFunction>
		{
		public:
			static const std::string PARAMETER_TARGET;
			static const std::string PARAMETER_TEXT;
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const WebPage> _target;
				std::string _text;
				server::ParametersMap _otherParameters;
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

#endif // SYNTHESE_WebPageLinkFunction_H__
