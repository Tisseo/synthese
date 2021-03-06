
/** SimpleObjectField_Date class header.
	@file SimpleObjectField_Date.hpp

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

#ifndef SYNTHESE__MinutesField_hpp__
#define SYNTHESE__MinutesField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include <boost/lexical_cast.hpp>
#include <vector>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// date partial specialization
	template<class C>
	class MinutesField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::posix_time::time_duration Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string _minutesToString(const typename MinutesField<C>::Type& p)
		{
			return p.is_not_a_date_time() ?
				std::string() :
				boost::lexical_cast<std::string>(p.total_seconds() / 60)
			;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a string into a date.
		/// @param d the date to convert
		/// @return the converted string
		static typename MinutesField<C>::Type _stringToMinutes(const std::string& text)
		{
			return text.empty() ?
				boost::posix_time::time_duration(boost::posix_time::not_a_date_time) :
				boost::posix_time::minutes(boost::lexical_cast<int>(text));
		}


	public:
		static bool LoadFromRecord(
			typename MinutesField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			return SimpleObjectFieldDefinition<C>::_LoadFromStringWithDefaultValue(
				fieldObject,
				record,
				_stringToMinutes,
				boost::posix_time::time_duration(boost::posix_time::not_a_date_time)
			);
		}



		static void SaveToFilesMap(
			const typename MinutesField<C>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_minutesToString
			);
		}



		static void SaveToParametersMap(
			const typename MinutesField<C>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_minutesToString
			);
		}



		static void SaveToParametersMap(
			const typename MinutesField<C>::Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_minutesToString
			);
		}



		static void SaveToDBContent(
			const typename MinutesField<C>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			MinutesField<C>::SaveToDBContent(fieldObject, content);
		}



		static void SaveToDBContent(
			const typename MinutesField<C>::Type& fieldObject,
			DBContent& content
		){
			if(fieldObject.is_not_a_date_time())
			{
				boost::optional<std::string> emptyString;
				content.push_back(Cell(emptyString));
			}
			else
			{
				int i(fieldObject.total_seconds() / 60);
				content.push_back(Cell(i));
			}
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
		}
	};

	#define FIELD_MINUTES(N) struct N : public MinutesField<N> {};
}

#endif
