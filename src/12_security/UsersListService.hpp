
//////////////////////////////////////////////////////////////////////////////////////////
///	UsersListService class header.
///	@file UsersListService.hpp
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

#ifndef SYNTHESE_UsersListService_H__
#define SYNTHESE_UsersListService_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace security
	{
		class User;

		//////////////////////////////////////////////////////////////////////////
		///	12.15 Function : UsersListService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Users list
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m12Functions refFunctions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.4.0
		class UsersListService:
			public util::FactorableTemplate<server::Function,UsersListService>
		{
		public:
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SURNAME;
			static const std::string PARAMETER_PHONE;
			static const std::string PARAMETER_LOGIN;
			static const std::string PARAMETER_NUMBER;

			static const std::string TAG_USER;


		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<User> _user;
				std::string _name;
				std::string _surname;
				std::string _phone;
				std::string _login;
				boost::optional<size_t> _number;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Users list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.4.0
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Users list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.4.0
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			UsersListService();



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

#endif // SYNTHESE_UsersListService_H__

