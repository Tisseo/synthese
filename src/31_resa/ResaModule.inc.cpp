
#include "31_resa/ResaModule.h"

#include "31_resa/ReservationTableSync.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/OnlineReservationRuleTableSync.h"

#include "31_resa/ResaRight.h"

#include "31_resa/ResaDBLog.h"

#include "31_resa/BookReservationAction.h"
#include "31_resa/CancelReservationAction.h"
#include "31_resa/CallBeginAction.h"
#include "31_resa/CallEndAction.h"
#include "31_resa/ResaLogEntryUpdateAction.h"

#include "31_resa/ReservationsListFunction.h"
#include "31_resa/ResaCustomerHtmlOptionListFunction.h"

#include "31_resa/ReservationItemInterfacePage.h"

#include "31_resa/ResaCallButtonInterfaceElement.h"

#include "31_resa/BookableCommercialLineAdmin.h"
#include "31_resa/BookableCommercialLinesAdmin.h"
#include "31_resa/ResaCustomerAdmin.h"
#include "31_resa/ResaCustomersAdmin.h"
#include "31_resa/ReservationRoutePlannerAdmin.h"
#include "31_resa/ResaLogAdmin.h"
#include "31_resa/ResaEditLogEntryAdmin.h"

// Registries
#include "Reservation.h"
#include "ReservationTransaction.h"
#include "OnlineReservationRule.h"
