
/** tridentexport class header.
	@file tridentexport.h

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


#ifndef SYNTHESE_IMPEX_TRIDENTEXPORT_H
#define SYNTHESE_IMPEX_TRIDENTEXPORT_H

#include "FileFormatTemplate.h"

#include <iostream>
#include <vector>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}
	
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Trident/Chouette file format.
		/// @ingroup m35File refFile
		///
		/// @todo Remove all running days of services of the datasource before Trident import.
		///
		///	<h3>The Trident file format</h3>
		///
		/// The Trident format is the French standard for public transport theoretical data exchange, based upon <a href="http://www.transmodel.org/fr/cadre1.html">Transmodel</a>.
		///
		/// The Trident format is defined by several XSD schemes :
		///	<ul>
		///		<li><a href="include/35_pt/chouette/Chouette.xsd">Chouette.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_Global_schema.xsd">trident_Global_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_Location_schema.xsd">trident_Location_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/trident/trident_PT_schema.xsd">trident_PT_schema.xsd</a></li>
		///		<li><a href="include/35_pt/chouette/chouette.zip">ZIP archive containing all Trident XSD files</a></li>
		/// </ul>
		///
		/// The Trident file format has been extended by <a href="http://www.tisseo.fr/">Tisséo</a>, the local public transportation authority of Toulouse, France. See details @ref trident2tisseo "below".
		///
		/// In most cases, the Trident objects correspond exactly to the SYNTHESE objects :
		///	<ul>
		///		<li>ConnectionLink = Junction</li>
		///	</ul>
		///
		/// <h3>Usage</h3>
		///
		/// Import parameters :
		///		- impstp (PARAMETER_IMPORT_STOPS / _importStops) : If true, the stops are imported from the Trident file. If false, the import function assumes that all stops linked in the Trident file exist already in the database.
		///		- impjun (PARAMETER_IMPORT_JUNCTIONS / _importJunctions) : If true, the junctions are imported from the Trident file. If false, they are ignored.
		///		- wod (PARAMETER_WITH_OLD_DATES / _startDate) : Number of past days to import (default 0).
		///		- dtd (PARAMETER_DEFAULT_TRANSFER_DURATION / _defaultTransferDuration) : Number of minutes for default transfer delay in case of connection place creation (default = 8).
		///
		/// Export parameters :
		///		- roid : id of the CommercialLine object to export
		///		- wte : If true, the generated file will contain "Tisséo Trident Extension". If false, the generated file will respect the pure Trident format.
		///		- wod (PARAMETER_WITH_OLD_DATES / _startDate) : Number of past days to export (default 0).
		///
		/// For the details about what is imported or exported, see the documentation of @ref TridentFileFormat::_parse
		///
		/// @anchor trident2tisseo
		/// <h3>Tisséo Trident Extension</h3>
		///
		/// The Tisséo extension adds some features to the original Trident format.
		///
		/// The Tisséo extended Trident format is defined by several XSD schemes :
		///	<ul>
		///		<li><a href="include/35_pt/trident2-tisseo/tisseo-chouette-extension.xsd">tisseo-chouette-extension.xsd</a></li>
		///		<li><a href="include/35_pt/trident2-tisseo/Chouette.xsd">Chouette.xsd</a></li>
		///		<li><a href="include/35_pt/trident2-tisseo/trident/trident_Global_schema.xsd">trident_Global_schema.xsd</a></li>
		///		<li><a href="include/35_pt/trident2-tisseo/trident/trident_Location_schema.xsd">trident_Location_schema.xsd</a></li>
		///		<li><a href="include/35_pt/trident2-tisseo/trident/trident_PT_schema.xsd">trident_PT_schema.xsd</a></li>
		///		<li><a href="include/35_pt/trident2-tisseo/trident2-tisseo.zip">ZIP archive containing all Trident XSD files</a></li>
		/// </ul>
		///
		/// <h4>Non concurrence entre lignes</h4>
		///
		/// Deux lignes peuvent donner lieu à une règle de non concurrence sur des origines-destinations desservies en commun :
		///	<ul>
		///		<li>Ligne à utiliser</li>
		///		<li>Ligne à ne pas utiliser</li>
		///		<li>Délai min entre les heures de départ des services (vide = non concurrence permanente)</li>
		///	</ul>
		///
		/// @image html line-conflict-type.png
		/// 
		/// <h4>Propriétés additionnelles du service</h4>
		///
		/// <ul>
		///		<li>PMR Oui/Non</li>
		///		<li>Réservation obligatoire pour PMR uniquement (pointe vers une modalité de réservation)</li>
		///		<li>Vélo Oui/Non</li>
		///		<li>Réservation obligatoire pour vélo (pointe vers une modalité de réservation)</li>
		///		<li>Renvoi : Le format Trident permet de préciser un commentaire au niveau de la course, mais ne fait pas référence à un renvoi. Il est donc nécessaire d'étendre le type de la course pour définir un ou plusieurs renvois.</li>
		///	</ul>
		///
		/// @image html tisseo-vehicle-journey.png
		///
		/// <h4>Sens aller-retour</h4>
		///
		/// Le champ wayBack est utilisé différemment par rapport à Trident : 1 indique le sens aller, 2 indique le sens retour.
		///
		/// <h4>Lignes de transport à la demande zonales</h4>
		///
		/// Les composants d'un itinéraire d'une ligne de transport à la demande zonale peuvent être de deux natures :
		///	<ul>
		///		<li>Un arrêt classique (couvert par Trident actuel)</li>
		///		<li>Une zone de transport à la demande, elle-même composée d'arrêts classiques</li>
		/// </ul>
		///
		/// Les zones de transport à la demande sont définies comme suit :
		/// <ul>
		///		<li>Composée de n arrêts</li>
		///		<li>Desserte interne à la zone autorisée ou non</li>
		///		<li>Durée de desserte interne</li>
		///	</ul>
		///
		/// Les règles classiques de gestion des composants d'itinéraire sont valables sur les zones, dont notamment :
		/// <ul>
		///		<li>Une heure d'arrivée par service</li>
		///		<li>Une heure de départ par service</li>
		///		<li>Possibilités d'ITL sur une série de zones</li>
		///	</ul>
		///
		/// Il est proposé d'utiliser les objets PTLink, standard, en faisant pointer les liens startOfLink et endOfLink vers des ID de nouveaux objets DRTStopArea, définis comme suit :
		///	<ul>
		///		<li>Les horaires sont stockés comme à l'accoutumée en faisant pointer les stopPointId vers les ID des DRTStopArea correspondants.</li>
		///		<li>Pour modéliser la desserte interne, on place deux fois la zone à la suite dans le descriptif d'itinéraire.</li>
		///	</ul>
		///
		/// @image html drt-stop-area.png
		///
		/// <h4>Modalités de réservation</h4>
		///
		/// Les modalités de réservation permettent de définir la plage temporelle pendant laquelle un service à réservation obligatoire peut être réservé.
		///
		/// Les caractéristiques des modalités de réservations sont les suivantes :
		/// <ul>
		///		<li>Caractère obligatoire de la réservation (Obligatoire, Facultatif au départ de certains arrêts et obligatoires aux autres, ou Facultatif)</li>
		///		<li>Choix T0 = départ du client ou départ de l'origine</li>
		///		<li>Durée minimale en minutes avant T0</li>
		///		<li>Durée minimale en jours avant T0</li>
		///		<li>Heure limite de réservation si la veille de T0</li>
		///		<li>Durée maximale en jours avant T0</li>
		///		<li>Numéro de téléphone à joindre (vide = pas de centre d'appels)</li>
		///		<li>Horaires d'ouverture du centre d'appels (en texte)</li>
		///		<li>URL du site Internet de réservation (vide = pas de site Internet de réservation)</li>
		///	</ul>
		///
		/// @image html reservation-rule.png
		///
		/// <h4>Caractéristiques particulières des services à réservation</h4>
		///
		/// Les services à réservation nécessitent d'ajouter les caractéristiques suivantes :
		/// <ul>
		///		<li>Modalité de réservation</li>
		///		<li>Liste des arrêts permettant la réservation facultative (cf modalités de réservation)</li>
		///		<li>Nombre de places limité (Si oui, valeur = nombre de places maximal)</li>
		/// </ul>
		///
		/// @image html drt-vehicle-journey.png
		///
		/// <h4>Arrêt principal de commune</h4>
		///
		/// Pouvoir définir un ou plusieurs arrêts principaux par commune, qui sont choisis par défaut dans les systèmes d'information lorsque l'utilisateur ne saisit par d'arrêt
		///	On définit pour chaque commune un objet de type Area incluant les arrêts considérés comme principaux.
		///
		/// @image html city-main-stops.png
		///
		/// <h4>Accessibilité PMR</h4>
		///
		/// <ul>
		///		<li>Oui/Non</li>
		/// </ul>
		///
		/// Il est proposé d'ajouter un champ au StopPoint :
		///
		/// @image html tisseo-stop-point.png
		///
		/// <h4>Horaire d'exploitation</h4>
		///
		/// Les horaires fournis à chaque franchissement d'arrêt peuvent provenir de l'exploitation (source logiciel métier) ou être interpolées. Dans certains cas il peut être utile de savoir d'où provient l'horaire. 
		///
		/// <ul>
		///		<li>Oui/Non</li>
		/// </ul>
		///
		/// Il est proposé d'ajouter deux champs à l'objet PTLink (un pour l'arrêt de départ, un pour l'arrêt d'arrivée). Les champs sont facultatifs : l'absence de valeur est à interpréter comme suit :
		/// <ul>
		///		<li>Oui si l'arrêt est une extrémité de l'itinéraire</li>
		///		<li>Non sinon</li>
		/// </ul>
		///
		/// @image html tisseo-ptlink.png
		///
		class TridentFileFormat
		:	public impex::FileFormatTemplate<TridentFileFormat>
		{
		public:
			static const std::string PARAMETER_IMPORT_STOPS;
			static const std::string PARAMETER_IMPORT_JUNCTIONS;
			static const std::string PARAMETER_WITH_OLD_DATES;
			static const std::string PARAMETER_DEFAULT_TRANSFER_DURATION;
		    
		private:
			//! @name Import parameters
			//@{
				bool		_importStops;
				bool		_importJunctions;
				boost::posix_time::time_duration	_defaultTransferDuration;
			//@}

			//! @name Import/Export parameters
			//@{
				boost::gregorian::date	_startDate;
			//@}

			//! @name Export parameters
			//@{
				boost::optional<util::RegistryKeyType>	_commercialLineId;
				const bool				_withTisseoExtension;
			//@}
		
		protected:
				


			//////////////////////////////////////////////////////////////////////////
			/// Trident file import.
			/// @param filePath path of the file to import
			/// @param os stream to write information messages on
			/// @param key unused parameter
			/// @author Hugues Romain
			///
			/// The Trident file import loads the following objects :
			///	<ul>
			///		<li>Commercial stop points (PublicTransportStopZoneConnectionPlace) : city, name, specific transfer delays (only if stop import mode)</li>
			///		<li>Physical stop points (PhysicalStop) : commercial stop point (only at physical stop creation), x, y, name (only if stop import mode)</li>
			///		<li>Scheduled services (ScheduledService) : all. Not imported if the service runs never.
			/// </ul>
			virtual void _parse(
				const boost::filesystem::path& filePath,
				std::ostream& os,
				std::string key = std::string()
			);



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@param import true if the parameters must be generated for an import, false for an export
			/// @return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			virtual server::ParametersMap _getParametersMap(bool import) const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// @param map Parameters map to interpret
			///	@param import true if the parameters must be read for an import, false for an export
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			virtual void _setFromParametersMap(const server::ParametersMap& map, bool import);
		
		public:

			TridentFileFormat(
				util::Env* env = NULL,
				boost::optional<util::RegistryKeyType> lineId = boost::optional<util::RegistryKeyType>(),
				bool withTisseoExtension = false
			);
			~TridentFileFormat();

			/** -> ChouettePTNetwork
			 */
			virtual void build(std::ostream& os);
			
			virtual void save(std::ostream& os) const;

			//! @name Setters
			//@{
				void setImportStops(bool value);
			//@}

			//! @name Getters
			//@{
				bool getImportStops() const;
			//@}

		private:
		    
			static std::string TridentId (const std::string& peer, const std::string clazz, const util::RegistryKeyType& id);
			static std::string TridentId (const std::string& peer, const std::string clazz, const std::string& s);
			static std::string TridentId (const std::string& peer, const std::string clazz, const util::Registrable& obj);
			static std::string GetCoordinate(const double value);
		};
	}
}

#endif
