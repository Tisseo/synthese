
#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "30_server/ServerModule.h"

#include "32_admin/AdminRequest.h"

#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"

#include "71_vinci_bike_rental/RentABikeAction.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"
#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciUpdateCustomerAction.h"
#include "71_vinci_bike_rental/VinciAddGuaranteeAction.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace security;
	using namespace accounts;

	namespace vinci
	{
		VinciCustomerAdminInterfaceElement::VinciCustomerAdminInterfaceElement()
			: AdminInterfaceElement("vincicustomers", AdminInterfaceElement::DISPLAYED_IF_CURRENT) {}


		std::string VinciCustomerAdminInterfaceElement::getTitle() const
		{
			return "Client";
		}

		void VinciCustomerAdminInterfaceElement::display( std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			// Current request
			AdminRequest* currentRequest = (AdminRequest*) request;

			// Current contract
			VinciContract* contract = VinciContractTableSync::get(ServerModule::getSQLiteThread(), currentRequest->getObjectId());
			User* user = UserTableSync::get(ServerModule::getSQLiteThread(), contract->getUserId());
			
			// Update user request
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			updateRequest->setAction(Factory<Action>::create<VinciUpdateCustomerAction>());

			// Add guarantee request
			AdminRequest* addGuaranteeRequest = Factory<Request>::create<AdminRequest>();
			addGuaranteeRequest->copy(request);
			addGuaranteeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			addGuaranteeRequest->setAction(Factory<Action>::create<VinciAddGuaranteeAction>());

			// Add Rent request
			AdminRequest* addRentRequest = Factory<Request>::create<AdminRequest>();
			addRentRequest->copy(request);
			addRentRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());
			addRentRequest->setAction(Factory<Action>::create<RentABikeAction>());

			// Personal Data
			stream
				<< "<h1>Coordonn�es</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><td>Nom</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_NAME << "\" value=\"" << user->getName() << "\" /></td></tr>"
				<< "<tr><td>Pr�nom</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_SURNAME << "\" value=\"" << user->getSurname() << "\" /></td></tr>"
				<< "<tr><td>Adresse</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_ADDRESS << "\" value=\"" << user->getAddress() << "\" /></td></tr>"
				<< "<tr><td>Code postal</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_POST_CODE << "\" value=\"" << user->getPostCode() << "\" /></td></tr>"
				<< "<tr><td>Commune</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_CITY << "\" value=\"" << user->getCityText() << "\" /></td></tr>"
				<< "<tr><td>Pays</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_COUNTRY << "\" value=\"" << user->getCountry() << "\" /></td></tr>"
				<< "<tr><td>E-mail</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_EMAIL << "\" value=\"" << user->getEMail() << "\" /></td></tr>"
				<< "<tr><td>T�l�phone</td><td><input name=\"" << VinciUpdateCustomerAction::PARAMETER_PHONE << "\" value=\"" << user->getPhone() << "\" /></td></tr>"
				<< "<tr><td colspan=\"2\"><input type=\"submit\" value=\"Modifier\" /></td>"
				<< "</table></form>"
				;

			// Guarantees
			vector<TransactionPart*> guarantees = TransactionPartTableSync::searchTransactionParts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getGuaranteeAccount(), user);

			stream
				<< "<h1>Cautions</h1>"
				<< addGuaranteeRequest->getHTMLFormHeader("addguarantee")
				<< "<input type=\"hidden\" name=\"" << VinciAddGuaranteeAction::PARAMETER_CONTRACT_ID << "\" value=\"" << contract->getKey() << "\" />"
				<< "<table>"
				<< "<tr><th>Date</th><th>Montant</th><th>Actions</th></tr>"
				;
			for (vector<TransactionPart*>::iterator it = guarantees.begin(); it != guarantees.end(); ++it)
			{
				Transaction* transaction = TransactionTableSync::get(ServerModule::getSQLiteThread(), (*it)->getTransactionId());

				stream
					<< "<tr>"
					<< "<td>" << transaction->getStartDateTime().toSQLiteString(false) << "</td>"
					<< "<td>" << Conversion::ToString((*it)->getRightCurrencyAmount()) << "</td>"
					<< "<td>"
					;
				if (transaction->getEndDateTime().isUnknown())
					stream << "Rendre la caution";
				else
					stream << "Caution rendue le " << transaction->getEndDateTime().toSQLiteString(false);

				stream
					<< "</td>"
					<< "</tr>";

				delete transaction;
				delete *it;
			}

			
			stream
				<< "<tr><td></td>"
				<< "<td><input name=\"" << VinciAddGuaranteeAction::PARAMETER_AMOUNT << "\" value=\"260\" /></td>"
				<< "<td><input type=\"submit\" value=\"Ajouter\" /></td></tr>"
				<< "</table></form>"
				;

			// Rents
			vector<TransactionPart*> rents = TransactionPartTableSync::searchTransactionParts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE), user);
			vector<VinciRate*> rates = VinciRateTableSync::searchVinciRates();
			stream
				<< "<h1>Locations</h1>"
				<< addRentRequest->getHTMLFormHeader("addrent")
				<< "<input type=\"hidden\" name=\"" << RentABikeAction::PARAMETER_CONTRACT_ID << "\" value=\"" << contract->getKey() << "\" />"
				<< "<table>"
				<< "<tr><th>Date</th><th>V�lo</th><th>Tarif</th></tr>"
				;
			for (vector<TransactionPart*>::iterator it = rents.begin(); it != rents.end(); ++it)
			{
				Transaction* transaction = TransactionTableSync::get(ServerModule::getSQLiteThread(), (*it)->getTransactionId());
								
				VinciRate* rate = NULL;
				if ((*it)->getRateId() > 0)
					rate = VinciRateTableSync::get(ServerModule::getSQLiteThread(), (*it)->getRateId());

				stream
					<< "<tr>"
					<< "<td>" << transaction->getStartDateTime().toSQLiteString(false) << "</td>"
					<< "<td>" << Conversion::ToString((*it)->getTradedObjectId()) << "</td>"
					<< "<td>" << ((rate == NULL) ? "" : rate->getName())  << "</td>"
					<< "</tr>"
					;
			}
			stream
				<< "<tr>"
				<< "<td><input type=\"submit\" value=\"Nouvelle location\" /></td>"
				<< "<td><input size=\"10\" name=\"" << RentABikeAction::PARAMETER_BIKE_ID << "\" /></td>"
				<< "<td><select name=\"" << RentABikeAction::PARAMETER_RATE_ID << "\">"
				;
			for (vector<VinciRate*>::iterator it = rates.begin(); it != rates.end(); ++it)
				stream << "<option value=\"" << Conversion::ToString((*it)->getKey()) << "\">" << (*it)->getName() << "</option>";
			stream
				<< "</select></td>"
				<< "</tr>";
				;



			stream
				<< "</table></form>"
				;


			// Change
			stream
				<< "<h1>Compte client</h1>"
				;

			// Cleaning
			delete contract;
			delete updateRequest;
		}
	}
}
