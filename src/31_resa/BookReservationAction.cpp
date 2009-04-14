
/** BookReservationAction class implementation.
	@file BookReservationAction.cpp

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

#include "BookReservationAction.h"

#include "Site.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "RoutePlannerFunction.h"
#include "RoutePlanner.h"
#include "JourneysResult.h"
#include "NamedPlace.h"
#include "ResaRight.h"
#include "ReservationTransaction.h"
#include "Reservation.h"
#include "ReservationTransactionTableSync.h"
#include "ReservationTableSync.h"
#include "ResaDBLog.h"
#include "ResaModule.h"
#include "ActionException.h"
#include "Request.h"

#include "Place.h"
#include "City.h"
#include "Journey.h"
#include "Service.h"
#include "Edge.h"
#include "Line.h"
#include "CommercialLine.h"
#include "Road.h"
#include "UseRule.h"
#include "NamedPlace.h"
#include "RoadPlace.h"
#include "Types.h"
#include "User.h"
#include "UserTableSync.h"
#include "GeographyModule.h"
#include "Conversion.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace routeplanner;
	using namespace security;
	using namespace env;
	using namespace transportwebsite;
	using namespace time;
	using namespace util;
	using namespace graph;
	using namespace road;
	using namespace geography;
	

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::BookReservationAction>::FACTORY_KEY("bra");
	}
	
	namespace resa
	{
		const string BookReservationAction::PARAMETER_SITE = Action_PARAMETER_PREFIX + "sit";
		
		const string BookReservationAction::PARAMETER_ACCESSIBILITY(Action_PARAMETER_PREFIX + "ac");
		const string BookReservationAction::PARAMETER_DISABLED_CUSTOMER(Action_PARAMETER_PREFIX + "dc");
		const string BookReservationAction::PARAMETER_DRT_ONLY(Action_PARAMETER_PREFIX + "do");

		const string BookReservationAction::PARAMETER_ORIGIN_CITY = Action_PARAMETER_PREFIX + "dct";
		const string BookReservationAction::PARAMETER_ORIGIN_PLACE = Action_PARAMETER_PREFIX + "dpt";
		const string BookReservationAction::PARAMETER_DESTINATION_CITY = Action_PARAMETER_PREFIX + "act";
		const string BookReservationAction::PARAMETER_DESTINATION_PLACE = Action_PARAMETER_PREFIX + "apt";
        const string BookReservationAction::PARAMETER_DATE_TIME = Action_PARAMETER_PREFIX + "da";


		const string BookReservationAction::PARAMETER_CREATE_CUSTOMER = Action_PARAMETER_PREFIX + "cc";
		const string BookReservationAction::PARAMETER_CUSTOMER_ID = Action_PARAMETER_PREFIX + "cuid";
		const string BookReservationAction::PARAMETER_CUSTOMER_NAME = Action_PARAMETER_PREFIX + "cuna";
		const string BookReservationAction::PARAMETER_CUSTOMER_SURNAME = Action_PARAMETER_PREFIX + "cs";
		const string BookReservationAction::PARAMETER_CUSTOMER_PHONE = Action_PARAMETER_PREFIX + "cuph";
		const string BookReservationAction::PARAMETER_CUSTOMER_EMAIL = Action_PARAMETER_PREFIX + "cupe";

		const string BookReservationAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "pass";

		const string BookReservationAction::PARAMETER_SEATS_NUMBER = Action_PARAMETER_PREFIX + "senu";


		ParametersMap BookReservationAction::getParametersMap() const
		{
			ParametersMap map;
			if (!_journey.empty())
			{
				if (_journey.getOrigin())
				{
					const NamedPlace* place(dynamic_cast<const NamedPlace*>(_journey.getOrigin()->getHub()));
					map.insert(PARAMETER_ORIGIN_CITY, place->getCity()->getName());
					map.insert(PARAMETER_ORIGIN_PLACE, place->getName());
				}
				if (_journey.getDestination())
				{
					const NamedPlace* place(dynamic_cast<const NamedPlace*>(_journey.getDestination()->getHub()));
					map.insert(PARAMETER_DESTINATION_CITY, place->getCity()->getName());
					map.insert(PARAMETER_DESTINATION_PLACE, place->getName());
				}
				if (!_journey.getDepartureTime().isUnknown())
				{
					map.insert(PARAMETER_DATE_TIME, _journey.getDepartureTime());
				}
			}
			return map;
		}

		void BookReservationAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Right control
			if (_request->isAuthorized<ResaRight>(WRITE, WRITE))
			{
				_createCustomer = map.getBool(PARAMETER_CREATE_CUSTOMER, false, false, FACTORY_KEY);

				if (_createCustomer)
				{
					_customer.reset(new User);
					_customer->setName(map.getString(PARAMETER_CUSTOMER_NAME, true, FACTORY_KEY));
					if (_customer->getName().empty())
						throw ActionException("Le nom du client doit �tre rempli");

					_customer->setSurname(map.getString(PARAMETER_CUSTOMER_SURNAME, true, FACTORY_KEY));
					if (_customer->getSurname().empty())
						throw ActionException("Le pr�nom du client doit �tre rempli");

					_customer->setPhone(map.getString(PARAMETER_CUSTOMER_PHONE, true, FACTORY_KEY));
					if (_customer->getPhone().empty())
						throw ActionException("Le num�ro de t�l�phone doit �tre rempli");

					// Integrity test : the key is name + surname + phone
					Env env;
					UserTableSync::Search(env, "%",_customer->getName(), _customer->getSurname(), _customer->getPhone(), UNKNOWN_VALUE, logic::indeterminate, 0, 1);
					if (!env.getRegistry<User>().empty())
						throw ActionException("Un utilisateur avec les m�mes nom, pr�nom, t�l�phone existe d�j�.");
					
					_customer->setEMail(map.getString(PARAMETER_CUSTOMER_EMAIL, false, FACTORY_KEY));
					_customer->setProfile(ResaModule::GetBasicResaCustomerProfile().get());
				}
				else
				{
					// Customer ID
					uid id(map.getUid(PARAMETER_CUSTOMER_ID, false, FACTORY_KEY));
					if (id != UNKNOWN_VALUE)
						_customer = UserTableSync::GetEditable(id, _env);
				}
			}
			else if (_request->isAuthorized<ResaRight>(FORBIDDEN, WRITE))
			{
				_customer = const_pointer_cast<User, const User>(_request->getUser());

				// Password control
				string password(map.getString(PARAMETER_PASSWORD, true, FACTORY_KEY));
				if (password.empty())
					throw ActionException("Le mot de passe doit �tre fourni");

				if (password != _customer->getPassword())
				{
					throw ActionException("Mot de passe erronn�");
				}
			}
			else
				throw ActionException("Not authorized");

			assert(_customer.get());

			// Deduce naming fields from the customer if already recognized
			if (_customer->getName().empty())
				throw ActionException("Client sans nom. R�servation impossible");

			if (_customer->getPhone().empty())
				throw ActionException("Client sans num�ro de t�l�phone. Veuillez renseigner ce champ dans la fiche client et recommencer la r�servation.");

			// Site
			uid id(map.getUid(PARAMETER_SITE, false, FACTORY_KEY));
			shared_ptr<const Site> site;
			if (id > 0 && Env::GetOfficialEnv().getRegistry<Site>().contains(id))
				site = Env::GetOfficialEnv().getRegistry<Site>().get(id);

			// Seats number
			_seatsNumber = map.getInt(PARAMETER_SEATS_NUMBER, true, FACTORY_KEY);
			if (_seatsNumber < 1 || _seatsNumber > 99)
				throw ActionException("Invalid seats number");

			// Journey
			const Place* originPlace(site.get() 
				? site->fetchPlace(
					map.getString(PARAMETER_ORIGIN_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_ORIGIN_PLACE, true, FACTORY_KEY)
				) : GeographyModule::FetchPlace(
					map.getString(PARAMETER_ORIGIN_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_ORIGIN_PLACE, true, FACTORY_KEY)
				)
			);
			const Place* destinationPlace(site.get()
				? site->fetchPlace(
					map.getString(PARAMETER_DESTINATION_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_DESTINATION_PLACE, true, FACTORY_KEY)
				) : GeographyModule::FetchPlace(
					map.getString(PARAMETER_DESTINATION_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_DESTINATION_PLACE, true, FACTORY_KEY)
				)
			);
			
			// Departure date time
			DateTime departureDateTime(map.getDateTime(PARAMETER_DATE_TIME, true, FACTORY_KEY));

			// Accessibility
			AccessParameters ap;

			
			if (site.get())
			{			
				AccessibilityParameter accessibility(static_cast<AccessibilityParameter>(
					map.getInt(PARAMETER_ACCESSIBILITY, false, string()))
				);
				ap = site->getAccessParameters(accessibility);
			}
			else
			{
				ap = AccessParameters(false, NULL, _disabledCustomer, true, _drtOnly, false);
			}

			RoutePlanner rp(
				originPlace
				, destinationPlace
				, ap
				, PlanningOrder()
				, departureDateTime
				, departureDateTime
				, 1
			);
			const RoutePlanner::Result& jr(rp.computeJourneySheetDepartureArrival());

			if (jr.journeys.empty())
				throw ActionException("The route planning does not find a journey to book");

			_journey = *jr.journeys.front();

		}

		void BookReservationAction::run()
		{
			// Save customer if necessary
			if (_createCustomer)
				UserTableSync::Save(_customer.get());

			// New ReservationTransaction
			const DateTime now(TIME_CURRENT);
			ReservationTransaction rt;
			rt.setBookingTime(now);
			rt.setBookingUserId(_request->getUser()->getKey());
			rt.setCustomerName(_customer->getName() + " " + _customer->getSurname());
			rt.setCustomerPhone(_customer->getPhone());
			rt.setCustomerEMail(_customer->getEMail());
			rt.setCustomerUserId(_customer->getKey());
			rt.setSeats(_seatsNumber);
			ReservationTransactionTableSync::Save(&rt);

			// New reservation for each journey leg
			BOOST_FOREACH(const ServiceUse& su, _journey.getServiceUses())
			{
				assert(su.getService() != NULL);
				assert(su.getDepartureEdge() != NULL);
				assert(su.getDepartureEdge()->getHub() != NULL);
				assert(su.getArrivalEdge() != NULL);
				assert(su.getArrivalEdge()->getHub() != NULL);

				shared_ptr<Reservation> r(rt.newReservation());
				r->setDeparturePlaceId(
					dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(
						su.getDepartureEdge()->getHub()
					)->getKey()
				);
				r->setDeparturePlaceName(
					dynamic_cast<const NamedPlace*>(
						su.getDepartureEdge()->getHub()
					)->getFullName()
				);
				r->setDepartureTime(su.getDepartureDateTime());
				r->setOriginDateTime(su.getOriginDateTime());
				r->setArrivalPlaceId(
					dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(
						su.getArrivalEdge()->getHub()
					)->getKey()
				);
				r->setArrivalPlaceName(
					dynamic_cast<const NamedPlace*>(
						su.getArrivalEdge()->getHub()
					)->getFullName()
				);
				r->setArrivalTime(su.getArrivalDateTime());
				
				const Line* line(dynamic_cast<const Line*>(su.getService()->getPath()));
				if (line)
				{
					assert(line->getCommercialLine() != NULL);

					r->setLineCode(line->getCommercialLine()->getName());
					r->setLineId(line->getCommercialLine()->getKey());
				}
				const Road* road(dynamic_cast<const Road*>(su.getService()->getPath()));
				if (road)
				{
					r->setLineCode(road->getRoadPlace()->getName());
					r->setLineId(road->getKey());
				}


				if(	su.getUseRule().isReservationPossible(
					su.getOriginDateTime()
					, now
					, su.getDepartureDateTime()
				))
				{
					r->setReservationRuleId(su.getService()->getActualRulesId());
					r->setReservationDeadLine(su.getUseRule().getReservationDeadLine(
						su.getOriginDateTime()
						, su.getDepartureDateTime()
					));
				}
				r->setServiceId(su.getService()->getKey());
				r->setServiceCode(Conversion::ToString(su.getService()->getServiceNumber()));
				ReservationTableSync::Save(r.get());
			}

			// Log
			ResaDBLog::AddBookReservationEntry(_request->getSession(), rt);

			// Redirect
			_request->setObjectId(rt.getKey());
		}

		BookReservationAction::BookReservationAction()
			: FactorableTemplate<Action, BookReservationAction>()
			, _journey()
			, _disabledCustomer(false)
			, _drtOnly(false)
		{

		}

		void BookReservationAction::setJourney( const Journey& journey )
		{
			_journey = journey;
		}



		bool BookReservationAction::_isAuthorized(
		) const {
			return
				_request->isAuthorized<ResaRight>(WRITE) ||
				_customer->getKey() == _request->getUser()->getKey() && _request->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE);
		}
	}
}
