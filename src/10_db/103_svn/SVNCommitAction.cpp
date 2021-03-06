
//////////////////////////////////////////////////////////////////////////
/// SVNCommitAction class implementation.
/// @file SVNCommitAction.cpp
/// @author Hugues Romain
/// @date 2012
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

#include "SVNCommitAction.hpp"

#include "ActionException.h"
#include "DBModule.h"
#include "ObjectBase.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "SVNWorkingCopy.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, db::svn::SVNCommitAction>::FACTORY_KEY = "SVNCommit";

	namespace db
	{
		namespace svn
		{
			const string SVNCommitAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "_object_id";
			const string SVNCommitAction::PARAMETER_USER = Action_PARAMETER_PREFIX + "_user";
			const string SVNCommitAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "_password";
			const string SVNCommitAction::PARAMETER_MESSAGE = Action_PARAMETER_PREFIX + "_message";
			const string SVNCommitAction::PARAMETER_NO_COMMIT = Action_PARAMETER_PREFIX + "_no_commit";
			const string SVNCommitAction::PARAMETER_NO_UPDATE = Action_PARAMETER_PREFIX + "_no_update";

			
			
			ParametersMap SVNCommitAction::getParametersMap() const
			{
				ParametersMap map;

				// Object
				if(_object.get())
				{
					map.insert(PARAMETER_OBJECT_ID, _object->getKey());
				}

				return map;
			}
			
			
			
			void SVNCommitAction::_setFromParametersMap(const ParametersMap& map)
			{
				// Object
				try
				{
					_object = dynamic_pointer_cast<ObjectBase, Registrable>(
						DBModule::GetEditableObject(
							map.get<RegistryKeyType>(PARAMETER_OBJECT_ID),
							Env::GetOfficialEnv()
					)	);
				}
				catch(ObjectNotFoundException<Registrable>&)
				{
					throw ActionException("No such object");
				}
				catch(Exception&)
				{
					throw ActionException("This object cannot be saved into a subversion repository");
				}
				if(!_object.get())
				{
					throw ActionException("This object cannot be saved into a subversion repository");
				}

				// Check if the object has SVN storage capabilities
				if(!_object->hasField<SVNWorkingCopy>())
				{
					throw ActionException("This object cannot be saved into a subversion repository");
				}

				// User
				_user = map.getDefault<string>(PARAMETER_USER);

				// Password
				_password = map.getDefault<string>(PARAMETER_PASSWORD);

				// Message
				_message = map.getDefault<string>(PARAMETER_MESSAGE);

				// No commit
				_noCommit = map.getDefault<bool>(PARAMETER_NO_COMMIT, false);

				// No update
				_noUpdate = map.getDefault<bool>(PARAMETER_NO_UPDATE, false);
			}
			
			
			
			void SVNCommitAction::run(
				Request& request
			){
				SVNWorkingCopy& wc(const_cast<SVNWorkingCopy&>(_object->dynamic_get<SVNWorkingCopy>()));
				wc.commit(_message, _user, _password, _noCommit, _noUpdate);
			}
			
			
			
			bool SVNCommitAction::isAuthorized(
				const Session* session
			) const {
//				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
				return true;
			}
}	}	}

