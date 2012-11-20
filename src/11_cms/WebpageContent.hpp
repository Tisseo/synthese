
/** WebpageContent class header.
	@file WebpageContent.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_cms_WebpageContent_hpp__
#define SYNTHESE_cms_WebpageContent_hpp__

#include "Factory.h"
#include "Function.h"
#include "ComplexObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"
#include "WebpageContentNode.hpp"
#include "MimeType.hpp"
#include "MimeTypes.hpp"

#include "shared_recursive_mutex.hpp"

#include <boost/logic/tribool.hpp>
#include <ostream>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	class FilesMap;
	class ObjectBase;

	namespace server
	{
		class Request;
	}

	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace cms
	{
		class Webpage;

		/** WebpageContent class.
			@ingroup m36
		*/
		class WebpageContent:
			public ComplexObjectFieldDefinition<WebpageContent>
		{
		public:
			typedef WebpageContent Type;

		private:

		protected:
			std::string _code;
			bool _ignoreWhiteChars;
			util::MimeType _mimeType;
			bool _doNotEvaluate;

		private:
			typedef std::vector<boost::shared_ptr<WebpageContentNode> > Nodes;
			mutable Nodes _nodes;
			boost::shared_ptr<util::shared_recursive_mutex> _sharedMutex;

			void _updateNodes();



			//////////////////////////////////////////////////////////////////////////
			/// Parses the content and put it in the nodes cache.
			/// @retval nodes object to write the result on
			/// @param it iterator on the beginning of the string to parse
			/// @param end iterator on the end of the string to parse
			/// @param termination termination string to detect to interrupt the parsing
			/// @return iterator on the end of the parsing
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// The parsing stops when the iterator has reached the end of the string, or if the ?> sequence has been found, indicating that the following text belongs to
			///	a lower level of recursion.
			/// If the level of recursion is superior than 0, then the output is encoded
			/// as an url to avoid mistake when the result of parsing is considered as
			/// a single parameter of a function call.
			void _parse(
				std::string::const_iterator& it,
				std::string::const_iterator end,
				std::set<std::string> termination
			);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Default constructor.
			WebpageContent();



			//////////////////////////////////////////////////////////////////////////
			/// Constructor by entire string parsing.
			WebpageContent(
				const std::string& code,
				bool ignoreWhiteChars = false,
				util::MimeType mimeType = util::MimeTypes::HTML,
				bool doNotEvaluate = false
			);



			//////////////////////////////////////////////////////////////////////////
			/// Constructor by string part parsing.
			WebpageContent(
				std::string::const_iterator& it,
				std::string::const_iterator end,
				std::set<std::string> termination
			);

			/// @name Getters
			//@{
				const std::string& getCode() const { return _code; }
				bool getIgnoreWhiteChars() const { return _ignoreWhiteChars; }
				const util::MimeType& getMimeType() const { return _mimeType; }
				bool getDoNotEvaluate() const { return _doNotEvaluate; }
			//@}

			/// @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Runs a nodes update if the value has changed.
				void setCode(const std::string& value);
			//@}

			/// @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Checks if the objects has at least a node to display.
				bool empty() const { return _nodes.empty(); }
			


				//////////////////////////////////////////////////////////////////////////
				/// Evaluates the nodes on a stream.
				/// @param stream stream to write on
				/// @param request current request
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page,
					util::ParametersMap& variables
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Evaluates the nodes on a new string.
				/// @param request current request
				/// @author Hugues Romain
				/// @date 2012
				/// @since 3.4.0
				std::string eval(
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page,
					util::ParametersMap& variables
				) const;
			//@}


			static void LoadFromRecord(
				Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			);



			static void SaveToFilesMap(
				const Type& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			);



			static void SaveToParametersMap(
				const Type& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			);



			static void SaveToDBContent(
				const Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			);


			static void GetLinkedObjectsIds(
				LinkedObjectsIds& list, 
				const Record& record
			);
		};
}	}

#endif // SYNTHESE_cms_WebpageContent_hpp__

