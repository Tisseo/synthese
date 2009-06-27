////////////////////////////////////////////////////////////////////////////////
/// UserUpdateAction class implementation.
///	@file UserUpdateAction.cpp
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

#include "UserUpdateAction.h"
#include "SecurityRight.h"
#include "UserTableSync.h"
#include "ProfileTableSync.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "SecurityLog.h"
#include "DBLogModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action, security::UserUpdateAction>::FACTORY_KEY("uua");
	
	namespace security
	{
		const std::string UserUpdateAction::PARAMETER_LOGIN = Action_PARAMETER_PREFIX + "login";
		const std::string UserUpdateAction::PARAMETER_SURNAME = Action_PARAMETER_PREFIX + "surn";
		const std::string UserUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const std::string UserUpdateAction::PARAMETER_ADDRESS = Action_PARAMETER_PREFIX + "addr";
		const std::string UserUpdateAction::PARAMETER_POSTAL_CODE = Action_PARAMETER_PREFIX + "post";
		const std::string UserUpdateAction::PARAMETER_CITY = Action_PARAMETER_PREFIX + "city";
		const std::string UserUpdateAction::PARAMETER_PHONE = Action_PARAMETER_PREFIX + "phon";
		const std::string UserUpdateAction::PARAMETER_EMAIL = Action_PARAMETER_PREFIX + "email";
		const std::string UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN = Action_PARAMETER_PREFIX + "auth";
		const std::string UserUpdateAction::PARAMETER_PROFILE_ID = Action_PARAMETER_PREFIX + "prof";


		ParametersMap UserUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UserUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::GetEditable(_request->getObjectId(), _env);

				_login = map.getString(PARAMETER_LOGIN, true, FACTORY_KEY);
				if (_login.empty())
					throw ActionException("Le login ne peut �tre vide");
				// Put a control of unicity

				_surname = map.getString(PARAMETER_SURNAME, true, FACTORY_KEY);

				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
				if (_name.empty())
					throw ActionException("Le nom de l'utilisateur ne peut �tre vide");

				_address = map.getString(PARAMETER_ADDRESS, true, FACTORY_KEY);

				_postalCode = map.getString(PARAMETER_POSTAL_CODE, true, FACTORY_KEY);

				_phone = map.getString(PARAMETER_PHONE, true, FACTORY_KEY);

				_city = map.getString(PARAMETER_CITY, true, FACTORY_KEY);

				_email = map.getString(PARAMETER_EMAIL, true, FACTORY_KEY);

				_authorizedLogin = map.getBool(PARAMETER_AUTHORIZED_LOGIN, true, false, FACTORY_KEY);

				uid id(map.getUid(PARAMETER_PROFILE_ID, false, FACTORY_KEY));
				if(id > 0)
				{
					*_profile = ProfileTableSync::Get(id, _env);
				}
			}
			catch (ObjectNotFoundException<Profile>& e)
			{
				throw ActionException("Profil introuvable");
			}
			catch (ObjectNotFoundException<User>& e)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}

		void UserUpdateAction::run()
		{
			stringstream s;
			DBLogModule::appendToLogIfChange(s, "Login", _user->getLogin(), _login);
			DBLogModule::appendToLogIfChange(s, "Adresse", _user->getAddress(), _address);
			DBLogModule::appendToLogIfChange(s, "E-mail", _user->getEMail(), _email);
			DBLogModule::appendToLogIfChange(s, "Code postal", _user->getPostCode(), _postalCode);
			DBLogModule::appendToLogIfChange(s, "Ville", _user->getCityText(), _city);
			DBLogModule::appendToLogIfChange(s, "T�l�phone", _user->getPhone(), _phone);
			if(_profile)
			{
				DBLogModule::appendToLogIfChange(s, "Profil", (_user->getProfile() != NULL) ? _user->getProfile()->getName() : string(), (*_profile != NULL) ? (*_profile)->getName() : string());
			}
			DBLogModule::appendToLogIfChange(s, "Autorisation de connexion", Conversion::ToString(_user->getConnectionAllowed()), Conversion::ToString(_authorizedLogin));
			DBLogModule::appendToLogIfChange(s, "Nom", _user->getName(), _name);
			DBLogModule::appendToLogIfChange(s, "Pr�nom", _user->getSurname(), _surname);

			_user->setLogin(_login);
			_user->setAddress(_address);
			_user->setEMail(_email);
			_user->setPostCode(_postalCode);
			_user->setCityText(_city);
			_user->setPhone(_phone);
			if(_profile)
			{
				_user->setProfile(_profile->get());
			}
			_user->setConnectionAllowed(_authorizedLogin);
			_user->setName(_name);
			_user->setSurname(_surname);

			UserTableSync::Save(_user.get());

			SecurityLog::addUserAdmin(_request->getUser().get(), _user.get(), s.str());
		}



		bool UserUpdateAction::_isAuthorized(
		) const {
			return _request->isAuthorized<SecurityRight>(WRITE) ||
				_request->getUser() != NULL && _request->getUser()->getKey() == _user->getKey();
		}
	}
}
