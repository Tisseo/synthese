
/** NonConcurrencyRule class header.
	@file NonConcurrencyRule.h

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

#ifndef SYNTHESE_env_NonConcurrencyRule_h__
#define SYNTHESE_env_NonConcurrencyRule_h__

#include "Registrable.h"
#include "Registry.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace env
	{
		class CommercialLine;

		/** Non-concurrency rule class.
			@ingroup m35
		*/
		class NonConcurrencyRule
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<NonConcurrencyRule>	Registry;

		protected:
			CommercialLine* _prorityLine;
			CommercialLine* _hiddenLine;
			boost::posix_time::time_duration	_delay;

		public:
			NonConcurrencyRule(
				util::RegistryKeyType key = UNKNOWN_VALUE
			);

			CommercialLine* getPriorityLine()	const;
			CommercialLine*	getHiddenLine()		const;
			const boost::posix_time::time_duration&	getDelay()			const;

			void setPriorityLine(CommercialLine* value);
			void setHiddenLine(CommercialLine* value);
			void setDelay(const boost::posix_time::time_duration& value);

		};
	}
}

#endif // SYNTHESE_env_NonConcurrencyRule_h__
