#include <boost/iostreams/stream.hpp>

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"

#include "01_util/Log.h"
#include "01_util/Conversion.h"
#include "01_util/Factory.h"
#include "01_util/FactoryException.h"

#include "30_server/ServerModule.h"
#include "30_server/ActionException.h"
#include "30_server/RequestException.h"
#include "30_server/Action.h"
#include "30_server/Request.h"
#include "30_server/ServerThreadExec.h"

using synthese::util::Log;
using synthese::util::Conversion;
using synthese::util::Factory;
using synthese::util::FactoryException;

namespace synthese
{
	namespace server
	{

	    ServerThreadExec::ServerThreadExec (synthese::tcp::TcpService* tcpService) 
		: _tcpService (tcpService)
	    {
	    }
	    


	    void 
	    ServerThreadExec::loop ()
	    {
		// No need to lock, TcpService methods are thread-safe.
		synthese::tcp::TcpServerSocket* serverSocket =
		    _tcpService->acceptConnection ();
		
		// Non-blocking mode (default)
		if (serverSocket == 0) return;
		
		boost::iostreams::stream<synthese::tcp::TcpServerSocket> 
		    tcpStream (*serverSocket);
		
		
			char buffer[1024*512]; // 512K buffer max
			tcpStream.getline (buffer, 1024*512);
		    
			std::string requestString (buffer);
			// tcpStream >> requestString;
			
			Log::GetInstance ().debug ("Received request : " + 
						requestString + " (" + Conversion::ToString (requestString.size ()) + 
						" bytes)");
			
			// Parse request
			try
			{
				Request request(requestString);
				request.run(tcpStream);
			}
			catch (RequestException e)
			{
				Log::GetInstance().debug("Request error", e);
			}
			catch (ActionException e)
			{
				Log::GetInstance().debug("Action error", e);
			}
			catch(util::Exception e)
			{
				Log::GetInstance().debug("Exception", e);
			}
			catch(...)
			{
				Log::GetInstance().debug("An unhandled exception has occured.");
			}
			tcpStream.flush();
			_tcpService->closeConnection (serverSocket);
		}
	}
}

