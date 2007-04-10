
/** AlarmObjectLink class header.
	@file AlarmObjectLink.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_AlarmObjectLink_h__
#define SYNTHESE_AlarmObjectLink_h__

#include <string>

#include "01_util/Registrable.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;

		/** AlarmObjectLink class.
			@ingroup m17
		*/
		template <class T>
		class AlarmObjectLink : public util::Registrable<uid, AlarmObjectLink<T> >
		{
			std::string		_recipientKey;
			boost::shared_ptr<T>	_object; 
			boost::shared_ptr<const Alarm>	_alarm;
			uid				_objectId;

		public:
			const std::string&	getRecipientKey() const { return _recipientKey; }
			boost::shared_ptr<T>		getObject() const { return _object; }
			uid					getObjectId() const { return _object.get() ? _object->getKey() : _objectId; }
			boost::shared_ptr<const Alarm>		getAlarm() const { return _alarm; }

			void setRecipientKey(const std::string& key) { _recipientKey = key; }
			void setObject(boost::shared_ptr<T> object) { _object = object; }
			void setObjectId(uid id) { _objectId = id; }
			void setAlarm(boost::shared_ptr<const Alarm> alarm) { _alarm = alarm; }

		};
	}
}

#endif // SYNTHESE_AlarmObjectLink_h__
