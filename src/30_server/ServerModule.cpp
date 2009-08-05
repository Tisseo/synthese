
/** ServerModule class implementation.
    @file ServerModule.cpp

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


#include "ServerModule.h"

#include "Log.h"
//#include "threads/ManagedThread.h"
//#include "CleanerThreadExec.h"

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#ifdef UNIX
  #define DEFAULT_TEMP_DIR "/tmp"
#endif
#ifdef WIN32
  #define DEFAULT_TEMP_DIR "c:/temp"
#endif

#include "HTTPReply.hpp"
#include "HTTPRequest.hpp"
#include "Log.h"
#include "Request.h"
#include "RequestException.h"
#include "ActionException.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	
	template<> const std::string util::FactorableTemplate<ModuleClass, ServerModule>::FACTORY_KEY("999_server");

    namespace server
    {
		ServerModule::SessionMap	ServerModule::_sessionMap;
		boost::asio::io_service ServerModule::_io_service;
		boost::asio::ip::tcp::acceptor ServerModule::_acceptor(ServerModule::_io_service);
		connection_ptr ServerModule::_new_connection(new HTTPConnection(ServerModule::_io_service));
		ServerModule::Threads ServerModule::_threads;
		size_t ServerModule::_waitingThreads(0);
		recursive_mutex ServerModule::_threadManagementMutex;


		const std::string ServerModule::MODULE_PARAM_PORT ("port");
		const std::string ServerModule::MODULE_PARAM_NB_THREADS ("nb_threads");
		const std::string ServerModule::MODULE_PARAM_LOG_LEVEL ("log_level");
		const std::string ServerModule::MODULE_PARAM_TMP_DIR ("tmp_dir");

		const std::string ServerModule::VERSION("3.1.7");

		template<> const string ModuleClassTemplate<ServerModule>::NAME("Server kernel");


		template<> void ModuleClassTemplate<ServerModule>::PreInit()
		{
			RegisterParameter(ServerModule::MODULE_PARAM_PORT, "8080", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_NB_THREADS, "5", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_LOG_LEVEL, "1", &ServerModule::ParameterCallback);
			RegisterParameter(ServerModule::MODULE_PARAM_TMP_DIR, DEFAULT_TEMP_DIR, &ServerModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<ServerModule>::Init()
		{
			try 
			{
	
				// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
				string address("0.0.0.0");
				string port(GetParameter(ServerModule::MODULE_PARAM_PORT));
				size_t threadsNumber(lexical_cast<size_t>(GetParameter(ServerModule::MODULE_PARAM_NB_THREADS)));
				
				
				asio::ip::tcp::resolver resolver(ServerModule::_io_service);
				asio::ip::tcp::resolver::query query(address, port);
				asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
				
				ServerModule::_acceptor.open(endpoint.protocol());
				ServerModule::_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
				ServerModule::_acceptor.bind(endpoint);
				ServerModule::_acceptor.listen();
				ServerModule::_acceptor.async_accept(
					ServerModule::_new_connection->socket(),
					bind(&ServerModule::HandleAccept, asio::placeholders::error)
				);
	
				// Create a pool of threads to run all of the io_services.
				for (std::size_t i = 0; i < threadsNumber; ++i)
				{
					ServerModule::AddHTTPThread();
				}
	
				Log::GetInstance ().info ("HTTP Server is listening on port " + port +" by at least "+ lexical_cast<string>(threadsNumber) + " threads ...");
			}
			
			catch (std::exception& ex)
			{
				Log::GetInstance ().fatal ("Unexpected exception", ex);
			} 
			catch (...)
			{
				Log::GetInstance ().fatal ("Unexpected exception");
			}
		}



		template<> void ModuleClassTemplate<ServerModule>::End()
		{
			ServerModule::_io_service.stop();
		}
		

		ServerModule::SessionMap& ServerModule::getSessions()
		{
			return _sessionMap;
		}


		void ServerModule::ParameterCallback(
			const std::string& name,
			const std::string& value
		){
			if (name == "port") 
			{
				// TODO : close and reopen service on the new port
			}
			if (name == "log_level") 
			{
				Log::GetInstance ().setLevel (static_cast<Log::Level>(lexical_cast<int>(value)));
			}
		}
		
		
		void ServerModule::HandleAccept(
			const boost::system::error_code& e
		){
			if (!e)
			{
				_new_connection->start();
				_new_connection.reset(new HTTPConnection(_io_service));
				_acceptor.async_accept(
					_new_connection->socket(),
					boost::bind(
						&ServerModule::HandleAccept,
						boost::asio::placeholders::error
					)
				);
			}
		}


		void ServerModule::HandleRequest(
			const HTTPRequest& req,
			HTTPReply& rep
		){
			try
			{
				Log::GetInstance ().debug ("Received request : " + 
					req.uri + " (" + lexical_cast<string>(req.uri.size()) + " bytes)" + (req.postData.empty() ? string() : " + "+ lexical_cast<string>(req.postData.size()) +" bytes of POST data : "+ req.postData.substr(0, 100) ) );

				SetCurrentThreadAnalysing(req.uri + (req.postData.empty() ? string() : " + "+ req.postData.substr(0, 100)));
				Request request(req);

				stringstream output;
				request.run(output);
				rep.status = HTTPReply::ok;
				rep.content.append(output.str());
				rep.headers.insert(make_pair("Content-Length", lexical_cast<string>(rep.content.size())));
				rep.headers.insert(make_pair("Content-Type", request.getOutputMimeType()));
			}
			catch(Request::ForbiddenRequestException& e)
			{
				Log::GetInstance().debug("Forbidden request");
				rep = HTTPReply::stock_reply(HTTPReply::forbidden);
			}
			catch (RequestException& e)
			{
				Log::GetInstance().debug("Request error", e);
				rep = HTTPReply::stock_reply(HTTPReply::bad_request);
			}
			catch (ActionException& e)
			{
				Log::GetInstance().debug("Action error", e);
				rep = HTTPReply::stock_reply(HTTPReply::bad_request);
			}
			catch(util::Exception& e)
			{
				Log::GetInstance().debug("Exception", e);
				rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
			}
			catch(thread_interrupted)
			{
				Log::GetInstance().debug("Current thread externally interrupted");
				rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
				throw thread_interrupted();
			}
			catch(std::exception& e)
			{
				Log::GetInstance().debug("An unhandled exception has occured : " + std::string (e.what ()));
				rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
			}
			catch(...)
			{
				Log::GetInstance().debug("An unhandled exception has occured.");
				rep = HTTPReply::stock_reply(HTTPReply::internal_server_error);
			}

			SetCurrentThreadWaiting();
		}



		bool ServerModule::URLDecode(
			const std::string& in,
			std::string& out
		){
		  out.clear();
		  out.reserve(in.size());
		  for (std::size_t i = 0; i < in.size(); ++i)
		  {
			if (in[i] == '%')
			{
			  if (i + 3 <= in.size())
			  {
				int value;
				std::istringstream is(in.substr(i + 1, 2));
				if (is >> std::hex >> value)
				{
				  out += static_cast<char>(value);
				  i += 2;
				}
				else
				{
				  return false;
				}
			  }
			  else
			  {
				return false;
			  }
			}
			else if (in[i] == '+')
			{
			  out += ' ';
			}
			else
			{
			  out += in[i];
			}
		  }
		  return true;
		}



		const ServerModule::Threads& ServerModule::GetThreads()
		{
			return _threads;
		}



		void ServerModule::KillThread(const string& key)
		{
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			Threads::iterator it(_threads.find(key));
			if(it == _threads.end()) return;
			shared_ptr<thread> theThread(it->second.theThread);
			_threads.erase(it);
			theThread->interrupt();
			Log::GetInstance ().info ("Attempted to kill the thread "+ key);
			if(_threads.size() < lexical_cast<size_t>(GetParameter(ServerModule::MODULE_PARAM_NB_THREADS)))
			{
				thread::id newId(AddHTTPThread());
				Log::GetInstance ().info ("Create the thread "+ lexical_cast<string>(newId) +" because the minimum threads number was reached");
			}
		}



		boost::thread::id ServerModule::AddHTTPThread()
		{
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			shared_ptr<thread> theThread(
				new thread(
					bind(&asio::io_service::run, &ServerModule::_io_service)
			)	);
			ThreadInfo info;
			info.status = ThreadInfo::THREAD_WAITING;
			info.theThread = theThread;
			info.lastChangeTime = posix_time::microsec_clock::local_time();
			_threads.insert(make_pair(lexical_cast<string>(theThread->get_id()), info));
			++_waitingThreads;
			return theThread->get_id();
		}



		void ServerModule::SetCurrentThreadAnalysing( const std::string& queryString )
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetCurrentThreadInfo());
				if(info.status != ThreadInfo::THREAD_WAITING) return;
				info.status = ThreadInfo::THREAD_ANALYSING_REQUEST;
				info.queryString = queryString;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
				--_waitingThreads;
				if(_waitingThreads == 0)
				{
					AddHTTPThread();
					Log::GetInstance ().info ("Raised HTTP threads number to "+ lexical_cast<string>(_threads.size()) +" due to pool saturation.");
				}
			}
			catch (ThreadInfo::Exception& e)
			{
			}
		}



		ServerModule::ThreadInfo& ServerModule::GetCurrentThreadInfo()
		{
			recursive_mutex::scoped_lock lock(_threadManagementMutex);
			Threads::iterator it(_threads.find(lexical_cast<string>(this_thread::get_id())));
			if(it == _threads.end()) throw ThreadInfo::Exception();
			return it->second;
		}



		void ServerModule::SetCurrentThreadRunningAction()
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetCurrentThreadInfo());
				info.status = ThreadInfo::THREAD_RUNNING_ACTION;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
			}
			catch (ThreadInfo::Exception& e)
			{
			}
		}



		void ServerModule::SetCurrentThreadRunningFunction()
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetCurrentThreadInfo());
				info.status = ThreadInfo::THREAD_RUNNING_FUNCTION;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
			}
			catch (ThreadInfo::Exception& e)
			{
			}
		}



		void ServerModule::SetCurrentThreadWaiting()
		{
			try
			{
				recursive_mutex::scoped_lock lock(_threadManagementMutex);
				ThreadInfo& info(GetCurrentThreadInfo());
				if(info.status == ThreadInfo::THREAD_WAITING) return;
				info.status = ThreadInfo::THREAD_WAITING;
				info.lastChangeTime = posix_time::microsec_clock::local_time();
				++_waitingThreads;
			}
			catch (ThreadInfo::Exception& e)
			{
			}
		}



		boost::recursive_mutex& ServerModule::GetThreadManagementMutex()
		{
			return _threadManagementMutex;
		}




		const char* ServerModule::ThreadInfo::Exception::what() const
		{
			return "Current thread is unregistered. Cannot retrieve thread info.";
		}
	}
}

