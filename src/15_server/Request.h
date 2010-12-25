////////////////////////////////////////////////////////////////////////////////
/// Request class header.
///	@file Request.h
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

#ifndef SYNTHESE_Request_H__
#define SYNTHESE_Request_H__

#include "Session.h"
#include "User.h"
#include "Profile.h"
#include "12_security/Types.h"
#include "12_security/Constants.h"
#include "ParametersMap.h"
#include "AjaxForm.hpp"

namespace synthese
{
	namespace server
	{
		class Action;
		class Function;

		/** Parsed request.
			@ingroup m15

			A request object determinates a couple of services to run, and can be used to launch it or to describe it (build of links, etc.)

			The two services are :
				- an action  (execution without display, see server::Action)
				- a fonction (build of a display)

			The request class can be used in several ways. The life cycle of the request object is divided in two steps :
				- the build
				- the output
			
			The build and the output follow much different ways. The request acts as a wrapper between the build and the output methods.

			The available build processes are :
				- build by a query string parse (static method createFromString)
				- build by classic object construction
				- build by copy followed by completion with the classic object construction methods

			The available output methods are  :
				- execution of the action and/or the display from the specified parameters
				- build of a query string (method getRequest)
				- build of an URL (method getURL)
				- build of an HTML form (method getHTMLForm, build of a class html::HTMLForm object)

			When the request is used to run the services, a session control is handled.

			The full sequence of the control is as following :
				- generic parsing of the string by the _parseString method
				- build of an empty Request by the Factory from the key read by the parsing process
				- build of an empty Action by the Factory from the key read by the parsing process
				- interpretation of the fields by the Action::_setFromParametersMap method
				- if a field is intended to be filled by the action run, then it is marked by the Action::_setFromParametersMap method
				- interpretation of the fields by the _setFromParametersMap method
				- if the action has to run before the session control (Action::beforeSessionControl method), run of the Action::run method
				- control of the session
				- interruption of the process if the  action needs a valid session (Action::_needsSession method)
				- if the action has to run after the session control (Action::beforeSessionControl method), run of the Action::run method
				- run of the _runAfterAction method
				- run of the _runBeforeDisplayIfNoSession method if no session is validated
				- interruption of the process if the function needs a valid session (return of the preceding method)
				- display with the _run method

			Sequences :
				- if the object is generated by the action :
					- read the names of action and function
					- get action parameters
					- test of the user ability to run the action
					- if error run function with action error
					- run action
					- if redirect after action, redirect
					- get function parameters
					- test of the user ability to run the action
					- if error do nothing
					- run function

				
		*/
		class Request
		{
		public:
			/** Exception asking the server to redirect.
				@ingroup m15
			*/
			class RedirectException:
				public std::exception
			{
				const std::string _location;
				const bool _permanently;

			public:
				//////////////////////////////////////////////////////////////////////////
				/// Constructor.
				/// @param location location where the request must be redirected
				/// @param permanently true=301 redirection, else 302
				RedirectException(const std::string& location, bool permanently): _location(location), _permanently(permanently) {}

				virtual ~RedirectException() throw() {}

				//! @name Getters
				//@{
					const std::string& getLocation() { return _location; }
					bool getPermanently() const { return _permanently; }
				//@}
			};

			/** Forbidden request Exception class.
			@ingroup m15
			*/
			class ForbiddenRequestException:
				public std::exception
			{
			};

			//////////////////////////////////////////////////////////////////////////
			/// Specified path is not found
			/// @ingroup m15
			class NotFoundException:
				public std::exception
			{
			};

			static const std::string PARAMETER_SEPARATOR;
			static const std::string PARAMETER_ASSIGNMENT;

			static const std::string PARAMETER_STARTER;
			static const std::string PARAMETER_FUNCTION;
			static const std::string PARAMETER_SERVICE;
			static const std::string PARAMETER_SESSION;
			static const std::string PARAMETER_OBJECT_ID;
			static const std::string PARAMETER_ACTION;
			static const std::string PARAMETER_ACTION_FAILED;
			static const std::string PARAMETER_ERROR_MESSAGE;
			static const std::string PARAMETER_ACTION_WILL_CREATE_OBJECT;
			static const std::string PARAMETER_NO_REDIRECT_AFTER_ACTION;

		protected:
			boost::shared_ptr<Action> _action;
			boost::shared_ptr<Function> _function;
			const Session*				_session;
			std::string					_ip;
			std::string					_clientURL;
			std::string					_hostName;
			bool						_actionWillCreateObject;
			boost::optional<util::RegistryKeyType>	_actionCreatedId;
			bool									_redirectAfterAction;
			std::string								_actionErrorMessage;

		public:
			Request();
			explicit Request(
				const Request& rhs,
				boost::shared_ptr<Action> action = boost::shared_ptr<Action>(),
				boost::shared_ptr<Function> function = boost::shared_ptr<Function>()
			);


		private:
			virtual void _loadAction() {}
			virtual void _loadFunction(
				const std::string& errorMessage,
				boost::optional<util::RegistryKeyType> actionCreatedId
			) {}
			void _deleteAction() { _action.reset(); }

		public:
			//! \name Getters
			//@{
				const Session*		getSession()		const { return _session; }
				const std::string&	getClientURL()		const { return _clientURL; }
				const std::string&	getIP()				const { return _ip; }
				const boost::optional<util::RegistryKeyType>& 	getActionCreatedId()	const { return _actionCreatedId; }
				bool				getActionWillCreateObject()	const { return _actionWillCreateObject; }
				const std::string&	getHostName()		const { return _hostName; }

				boost::shared_ptr<Action> getAction() { return _action; }
				boost::shared_ptr<const Action> getAction() const { return boost::const_pointer_cast<const Action>(_action); }
				boost::shared_ptr<Function> getFunction() { return _function; }
				boost::shared_ptr<const Function> getFunction() const { return boost::const_pointer_cast<const Function>(_function); }

			//@}

			//! \name Setters
			//@{
				void setActionCreatedId(util::RegistryKeyType id) { if(_actionWillCreateObject) _actionCreatedId = id; }
				void setActionWillCreateObject() { _actionWillCreateObject = true; }
				void setSession(Session* session);
				void setHostName(const std::string& value) { _hostName = value; }

				/** Client URL setter.
					@param url URL to store.
					@author Hugues Romain
					@date 2007
				*/
				void setClientURL(const std::string& url) { _clientURL = url; }
			//@}

			//! \name Modifiers
			//@{
				void deleteSession();
			//@}

			//! @name Services
			//@{
				ParametersMap _getParametersMap() const;
				virtual ParametersMap getParametersMap() const { return _getParametersMap(); }
			
				/** Run of the services.
					@param stream Stream to write the output on
					@author Hugues Romain
					@date 2007
				*/
				void run(std::ostream& stream);

				template<class R>
				bool isAuthorized(
					security::RightLevel publicr = security::USE
					, security::RightLevel privater = security::UNKNOWN_RIGHT_LEVEL
					, std::string parameter = security::GLOBAL_PERIMETER
				) const;

				bool isActionFunctionAuthorized() const;

				boost::shared_ptr<const security::User> getUser() const;

				std::string getOutputMimeType();

				/** URL generator.
					@return std::string The URL corresponding to the request (= client URL + query string)
					@param normalize true|default = converts the url considering it is generated by a web browser (see @ref _normalizeRequest), false = do not transform the URL.
						The second option is available if the Request object is used for building virtual urls containing scripts command (eg : synthese?id='+ document.getElementById("toto").value+'&...)
					@param absolute generates absolute URL (with HTTP protocol)
					@author Hugues Romain
					@date 2007
				*/
				std::string getURL(bool normalize = true, bool absolute=false)	const;



				/** HTML Form generator.
					@param name Name of the form (default = no name, sufficient for link generation)
					@return boost::shared_ptr<html::HTMLForm> The created form.
					@author Hugues Romain
					@date 2007					
				*/
				html::HTMLForm getHTMLForm(std::string name=std::string()) const;

				html::AjaxForm getAjaxForm(const std::string& name) const;

				/** Query string getter for building links.
					@return The query string corresponding to the request.
				*/
				std::string getURI() const;
			//@}

		};


		template <class R>
		bool Request::isAuthorized(
			security::RightLevel publicr,
			security::RightLevel privater,
			std::string parameter /*= security::GLOBAL_PERIMETER*/
		) const	{
			if (_session == NULL ||
				_session->getUser() == NULL ||
				_session->getUser()->getProfile() == NULL
			){
				return false;
			}
			return _session->getUser()->getProfile()->isAuthorized<R>(publicr, privater, parameter);
		}

	}
}
#endif // SYNTHESE_Request_H__
