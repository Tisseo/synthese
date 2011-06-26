
//////////////////////////////////////////////////////////////////////////////////////////
/// XMLReservationFunction class header.
///	@file XMLReservationFunction.h
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_XMLReservationFunction_H__
#define SYNTHESE_XMLReservationFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace resa
	{
		class ReservationTransaction;

		/** XMLReservationFunction Function class.
			@author Hugues Romain
			@date 2009
			@ingroup m31Functions refFunctions

			<h3>Prérequis</h3>

			<ul>
			<li>Disposer d’une session valide dont l’ID est connu.</li>
			<li>Disposer de l’ID de l’utilisateur courant</li>
			<li>Disposer des paramètres de lancement du calculateur d’itinéraires ayant conduit à la proposition de la solution à réserver</li>
			<li>Disposer de l’heure de départ de la solution à réserver</li>
			</ul>

			<h3>Request</h3>

			<pre>
			fonction=XMLReservationFunction
			sid=<session id> : ID of the current session
			roid=<id reservation> : Reservation ID (can be the BookReservationAction created id)
			</pre>

			<h3>Réponse</h3>

			Description

			Une réservation est décrite comme suit :

			@image html reservation.png

				- id : identificateur de la réservation dans la base de données de SYNTHESE (vide si
			réservation non effectuée ou non trouvée)
				- customerId : identificateur du client dans la base de données de SYNTHESE
				- cancellationDeadLine : date/heure limite d’annulation de la réservation
				- departureStop : nom complet (commune + nom) de l’arrêt de départ du trajet
			réservé
				- arrivalStop : nom complet (commune + nom) de l’arrêt d’arrivée du trajet réservé
				- travelDate : date/heure du départ du trajet réservé
				- customerName : nom complet (prénom + nom) du client ayant réservé
				- customerPhone : numéro de téléphone du client ayant réservé
				- status : statut de la réservation (texte)
				- canBeCancelled (booléen) : indique si la réservation peut être annulée
				- seats : nombre de places réservées
				- cancellationDateTime (optionnel) : sa présence indique que la réservation a été
			annulée. Sa valeur indique la date et l’heure de l’annulation
				- le détail du trajet réservé est décrit par une succession d’objets chunk décrivant
			chaque étape du trajet :
					- departurePlaceName : nom complet de l’arrêt de départ du tronçon
					- departureDateTime : date/heure de départ du tronçon
					- arrivalPlaceName : nom complet de l’arrêt d’arrivée du tronçon
					- arrivalDateTime : date/heure d’arrivée du tronçon
					- lineNumber : numéro de la ligne empruntée

			<h3>Attachments</h3>

			<ul>
			<li><a href="include/51_resa/xml_reservation_function.xsd">Response XSD Schema</a></li>
			<li><a href="include/51_resa/XMLReservationFunctionSample.xml">Example of XML response</a></li>
			</ul>
		*/
		class XMLReservationFunction:
			public util::FactorableTemplate<server::Function,XMLReservationFunction>
		{
		public:

		protected:
			boost::shared_ptr<const ReservationTransaction> _resa;


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2009
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2009
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			XMLReservationFunction();



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @author Hugues
			/// @date 2009
			virtual void run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_XMLReservationFunction_H__
