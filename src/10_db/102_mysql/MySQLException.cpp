
/** MySQLException class implementation.
	@file MySQLException.cpp
	@author Sylvain Pasche

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

#include "102_mysql/MySQLException.hpp"

#include <string>

namespace synthese
{
	namespace db
	{
		MySQLException::MySQLException(const std::string& message, unsigned int mysql_errno) :
			DBException(message),
			_errno(mysql_errno)
		{
		}



		MySQLException::~MySQLException() throw ()
		{
		}



		unsigned int MySQLException::getErrno() const
		{
			return _errno;
		}
	}
}
