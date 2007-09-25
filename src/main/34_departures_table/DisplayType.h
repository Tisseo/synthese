
/** DisplayType class header.
	@file DisplayType.h

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

#ifndef SYNTHESE_DisplayType_H__
#define SYNTHESE_DisplayType_H__

#include <string>

#include "01_util/UId.h"
#include "01_util/Registrable.h"

namespace synthese
{
	namespace interfaces
	{
		class Interface;
	}

	namespace departurestable
	{
		/** Type of display screen, describing the input and output facilities.
			@ingroup m34
		*/
		class DisplayType : public util::Registrable<uid, DisplayType>
		{
		private:
			std::string										_name;
			boost::shared_ptr<const interfaces::Interface>	_interf;
			int												_rowNumber;
			int												_maxStopsNumber;

		public:
			DisplayType();
			DisplayType(const uid&);

			const std::string& getName() const;
			boost::shared_ptr<const interfaces::Interface> getInterface() const;
			int getRowNumber() const;
			int getMaxStopsNumber() const;

			void setName(const std::string& name);
			void setInterface(boost::shared_ptr<const interfaces::Interface> interf);
			void setRowNumber(int number);
			void setMaxStopsNumber(int number);

		};
	}
}

#endif // SYNTHESE_DisplayType_H__
