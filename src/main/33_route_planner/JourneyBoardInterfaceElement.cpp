
/** JourneyBoardInterfaceElement class implementation.
	@file JourneyBoardInterfaceElement.cpp

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


#include "33_route_planner/JourneyBoardInterfaceElement.h"
#include "33_route_planner/JourneyBoardStopCellInterfacePage.h"
#include "33_route_planner/JourneyBoardServiceCellInterfacePage.h"
#include "33_route_planner/JourneyBoardJunctionCellInterfacePage.h"
#include "33_route_planner/RoutePlannerModule.h"

#include "30_server/Request.h"

#include "15_env/Edge.h"
#include "15_env/Vertex.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Road.h"
#include "15_env/ReservationRule.h"
#include "15_env/ServiceUse.h"
#include "15_env/Journey.h"
#include "15_env/Service.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"

#include "04_time/module.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace time;
	using namespace env;

	namespace routeplanner
	{

		string JourneyBoardInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			// Parameters
			const Journey* journey = static_cast<const Journey*>(object);
			int __FiltreHandicape = (Conversion::ToInt(_handicappedFilter->getValue(parameters, variables, object, request)) > 1);
			int __FiltreVelo = (Conversion::ToInt(_bikeFilter->getValue(parameters, variables, object, request)) > 1 );

			// Resources
			shared_ptr<const JourneyBoardStopCellInterfacePage> stopCellInterfacePage = _page->getInterface()->getPage<JourneyBoardStopCellInterfacePage>();
			shared_ptr<const JourneyBoardServiceCellInterfacePage> serviceCellInterfacePage = _page->getInterface()->getPage<JourneyBoardServiceCellInterfacePage>();
			shared_ptr<const JourneyBoardJunctionCellInterfacePage> junctionCellInterfacePage = _page->getInterface()->getPage<JourneyBoardJunctionCellInterfacePage>();
			const Hour unknownHour(TIME_UNKNOWN );
			const DateTime unknownDateTime(TIME_UNKNOWN );

			// Loop on lines of the board
			bool __Couleur = false;

			for (Journey::ServiceUses::const_iterator it = journey->getServiceUses().begin(); it != journey->getServiceUses().end(); ++it)
			{
				const ServiceUse& leg(*it);

				// LIGNE ARRET MONTEE Si premier point d'arrêt et si alerte
				if (it == journey->getServiceUses().begin())
				{
					DateTime debutPrem(leg.getDepartureDateTime());
					DateTime finPrem(debutPrem);
					if (journey->getContinuousServiceRange () )
						finPrem += journey->getContinuousServiceRange ();

/*					if (leg->getOrigin()->getFromVertex ()->getConnectionPlace()
						->hasApplicableAlarm ( debutPrem, finPrem ) )
					{
						stopCellInterfacePage->display( stream, false
							, __ET->getOrigin() ->getConnectionPlace() ->getAlarm()
							, false, __ET->getOrigin() ->getConnectionPlace() ->getName()
							, __Couleur, unknownHour, unknownHour
							, request );
					}
*/				}

				if ( dynamic_cast<const synthese::env::Road*> (leg.getService()->getPath ()) == 0 )
				{
					// LIGNE CIRCULATIONS
					DateTime debutLigne(leg.getDepartureDateTime());
					DateTime finLigne(leg.getArrivalDateTime());
					DateTime lastDepartureTime(TIME_UNKNOWN);
					DateTime lastArrivalTime(TIME_UNKNOWN);

					if (journey->getContinuousServiceRange () )
					{
						lastDepartureTime = leg.getDepartureDateTime();
						lastDepartureTime += journey->getContinuousServiceRange ();
					}
					if ( journey->getContinuousServiceRange () )
					{
						lastArrivalTime = leg.getArrivalDateTime ();
						lastArrivalTime += journey->getContinuousServiceRange ();
						finLigne = lastArrivalTime;
					}

					// 12/18 Reservation
					DateTime maintenant(TIME_CURRENT);
					const ReservationRule* reservationRule(leg.getService()->getReservationRule ());

					bool openedCompulsoryReservation( 
						(reservationRule->isCompliant() == true)
						&& (reservationRule->isReservationPossible(leg.getOriginDateTime(), maintenant, leg.getDepartureDateTime() )) 
					);
					bool openedOptionalReservation(
						(reservationRule->isCompliant() == boost::logic::indeterminate) &&
						(reservationRule->isReservationPossible(leg.getOriginDateTime(), maintenant, leg.getDepartureDateTime() )) 
					);
					bool openedReservation = openedCompulsoryReservation || openedOptionalReservation;
					std::string syntheseOnlineBookingURL;
//					if (openedReservation && site->onlineBookingAllowed() ) /// @todo implement && __ET->getLigne() ->GetResa() ->ReservationEnLigne()
					{	/** @todo implement this
						synthese::server::Request request;
						request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_RESERVATION_FORM );
						request.addParameter( synthese::server::PARAMETER_SITE, __Site->getClef() );
						request.addParameter( synthese::server::PARAMETER_LINE_CODE, __ET->getLigne() ->getCode() );
						request.addParameter( synthese::server::PARAMETER_SERVICE_NUMBER, ( __ET->getService() ->getNumero() ) );
						request.addParameter( synthese::server::PARAMETER_SERVICE_NUMBER, __ET->getLigne() ->GetResa() ->Index() );
						request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER, __ET->getOrigin() ->getConnectionPlace() ->getId() );
						request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER, __ET->getDestination() ->getConnectionPlace() ->getId() );
						request.addParameter( synthese::server::PARAMETER_DATE, __ET->getDepartureTime() );
						
						syntheseOnLineBookingURL = site->getClientURL() + "?" + request.toInternalString (); //18
						*/
					}

					serviceCellInterfacePage->display(
						stream 
						, leg
						, leg.getDepartureDateTime().getHour()
						, lastDepartureTime.getHour()
						, leg.getArrivalDateTime().getHour()
						, lastArrivalTime.getHour()
						, 0 /// @todo implement __ET->getLigne() ->Materiel() ->Code(); //4
						, "ligne" /// @todo implement __ET->getLigne() ->Materiel() ->getLibelleSimple() //5
						, "la ligne" /// @todo implement __ET->getLigne()->LibelleComplet(LibelleCompletMatosHTML);
						, "destination" /// @todo implement __ET->getLigne()->LibelleDestination(DestinationHTML);
						, __FiltreHandicape
						, __FiltreVelo
						, openedCompulsoryReservation
						, openedOptionalReservation
						, openedReservation ? reservationRule->getReservationDeadLine (leg.getOriginDateTime(), leg.getDepartureDateTime() ) : unknownDateTime
						, openedReservation ? reservationRule : NULL
						, openedReservation ? syntheseOnlineBookingURL : ""
						, NULL // leg->getService ()->getPath ()->hasApplicableAlarm ( debutLigne, finLigne ) ? __ET->getService()->getPath ()->getAlarm() : NULL
						, __Couleur
						, request
					);
					
					__Couleur = !__Couleur;


					// LIGNE ARRET DE DESCENTE

					DateTime debutArret(leg.getArrivalDateTime ());
					DateTime finArret(debutArret);
					DateTime tempMoment(TIME_UNKNOWN);
					if ( (it + 1) < journey->getServiceUses().end())
						finArret = (it + 1)->getDepartureDateTime();
					if ( journey->getContinuousServiceRange () )
						finArret += journey->getContinuousServiceRange ();

					
					if ( journey->getContinuousServiceRange () )
					{
						tempMoment = leg.getArrivalDateTime ();
						tempMoment += journey->getContinuousServiceRange ();
					}
					
					stopCellInterfacePage->display( stream, true
						, NULL // leg->getDestination() ->getConnectionPlace()->hasApplicableAlarm ( debutArret, finArret ) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
						, leg.getArrivalEdge()->getConnectionPlace() == leg.getService()->getPath ()->getEdges ().back()->getFromVertex ()->getConnectionPlace()
						, leg.getArrivalEdge()->getConnectionPlace()->getName()
						, __Couleur
						, leg.getArrivalDateTime().getHour(), tempMoment.getHour()
						, request);

					__Couleur = !__Couleur;

				}
				else
				{
					// 1/2 Alerte
					DateTime debutArret(leg.getArrivalDateTime ());
					DateTime finArret(debutArret);
					if ((it+1) < journey->getServiceUses().end())
						finArret = (it + 1)->getDepartureDateTime();
					if ( journey->getContinuousServiceRange () )
						finArret += journey->getContinuousServiceRange ();

					junctionCellInterfacePage->display( stream
						, leg.getArrivalEdge()->getConnectionPlace()
						, NULL // leg->getDestination()->getConnectionPlace()->hasApplicableAlarm(debutArret, finArret) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
						, __Couleur
						, request);
				
					// 3 Couleur du fond de case
					__Couleur = !__Couleur;
				}
			}
			return string();
		}

		void JourneyBoardInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{
			if (vel.size() != 2)
				throw InterfacePageException("Insufficient parameters number");
			_handicappedFilter = vel.front();
			_bikeFilter = vel.front();
		}
	}
}
