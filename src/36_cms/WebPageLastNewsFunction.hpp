
//////////////////////////////////////////////////////////////////////////////////////////
/// WebPageLastNewsFunction class header.
///	@file WebPageLastNewsFunction.hpp
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

#ifndef SYNTHESE_WebPageLastNewsFunction_H__
#define SYNTHESE_WebPageLastNewsFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		///	36.15 Function : Displays a set of sub-pages abstracts and links.
		///	@ingroup m56Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : last_news
		///
		/// Parameters :
		///	<ul>
		///		<li>root : id of the parent page of each news (can be the id of the site if a display page is defined)</li>
		///		<li>display_page (optional) : id of the page to use to display each news.
		///			If undefined, then the output uses RSS format.</li>
		///		<li>number : maximal number of displayed news</li>
		///	</ul>
		///
		/// Specification of RSS format : http://www.rssboard.org/rss-specification
		///
		/// The RSS output can be customized like this :
		///	<ul>
		///		<li>The title of the RSS feed is the name of the root page</li>
		///		<li>The description of the RSS feed is the abstract of the root page</li>
		///		<li>The image of the RSS feed is the image of the root page, if defined. Warning, in this case,
		///			the image should be an absolute URL</li>
		///		<li>the description of each item is the abstract field of each page</li>
		///		<li>the title of each item is the name of each page</li>
		///	</ul>
		///
		/// The generated list is sorted by page row and not by date.
		class WebPageLastNewsFunction:
			public util::FactorableTemplate<server::Function,WebPageLastNewsFunction>
		{
		public:
			static const std::string PARAMETER_ROOT_ID;
			static const std::string PARAMETER_DISPLAY_PAGE_ID;
			static const std::string PARAMETER_MAX_NUMBER;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const Webpage> _root;
				boost::shared_ptr<const Webpage> _displayPage;
				boost::optional<std::size_t> _number;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



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

#endif // SYNTHESE_WebPageLastNewsFunction_H__
