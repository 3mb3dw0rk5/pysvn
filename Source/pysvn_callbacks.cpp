//
// ====================================================================
// Copyright (c) 2003 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
//
//	pysvn_callbacks.cpp
//
#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "pysvn.hpp"

static bool get_string( Py::Object &fn, std::string & _msg );

pysvn_callbacks::pysvn_callbacks()
	: pyfn_GetLogin()
	, pyfn_Notify()
	, pyfn_GetLogMessage()
	, pyfn_SslServerPrompt()
	, pyfn_SslServerTrustAnswer()
	, pyfn_SslClientCertPrompt()
	, pyfn_SslClientCertPwPrompt()
	, permission( NULL )
	{ }


pysvn_callbacks::~pysvn_callbacks()
	{ }

void pysvn_callbacks::setPermission( PythonAllowThreads &_permission )
	{
	permission = &_permission;
	}

void pysvn_callbacks::clearPermission()
	{
	permission = NULL;
	}

//
// this method will be called to retrieve
// authentication information
//
// WORKAROUND FOR apr_xlate PROBLEM: 
// STRINGS ALREADY HAVE TO BE UTF8!!!
//
// @retval true continue
//
bool pysvn_callbacks::contextGetLogin( const std::string & _realm,
		std::string & _username, 
		std::string & _password )
	{
	PythonDisallowThreads callback_permission( permission );

	// make sure we can call the users object
	if( !pyfn_GetLogin.isCallable() )
		return false;

	Py::Callable callback( pyfn_GetLogin );

	Py::Tuple args( 2 );
	args[0] = Py::String( _realm );
	args[1] = Py::String( _username );

	// bool, username, password
	Py::Tuple results;
	Py::Int retcode;
	Py::String username;
	Py::String password;

	try
		{
		results = callback.apply( args );
		retcode = results[0];
		username = results[1];
		password = results[2];

		// true returned
		if( long( retcode ) != 0 )
			{
			// copy out the answers
			_username = username.as_std_string();
			_password = password.as_std_string();

			return true;
			}
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();

		return false;
		}

	return false;
	}

// 
// this method will be called to notify about
// the progress of an ongoing action
//
void pysvn_callbacks::contextNotify (const char *path,
		svn_wc_notify_action_t action,
		svn_node_kind_t kind,
		const char *mime_type,
		svn_wc_notify_state_t content_state,
		svn_wc_notify_state_t prop_state,
		svn_revnum_t revnum )
	{
	PythonDisallowThreads callback_permission( permission );

	// make sure we can call the users object
	if( !pyfn_Notify.isCallable() )
		return;

	Py::Callable callback( pyfn_Notify );

	Py::Tuple args( 1 );
	Py::Dict info;
	args[0] = info;

	info["path"] = Py::String( path );
	info["action"] = toEnumValue( action );
	info["kind"] = toEnumValue( kind );
	info["content_state"] = toEnumValue( content_state );
	info["prop_state"] = toEnumValue( prop_state );
	info["revision"] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );

	Py::Int( revision );

	// bool, username, password
	Py::Object results;

	try
		{
		results = callback.apply( args );
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();
		}
	}

//
//	Return true to cancel a long running operation
//
bool pysvn_callbacks::contextCancel()
	{
	PythonDisallowThreads callback_permission( permission );

	// make sure we can call the users object
	if( !pyfn_Cancel.isCallable() )
		return false;

	Py::Callable callback( pyfn_Cancel );

	Py::Tuple args( 0 );

	// bool
	Py::Object result;

	Py::Int retcode;

	try
		{
		result = callback.apply( args );
		retcode = result;

		return long( retcode ) != 0;
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();

		return false;
		}
	}

//
// this method will be called to retrieve
// a log message
//
// WORKAROUND FOR apr_xlate PROBLEM: 
// STRINGS ALREADY HAVE TO BE UTF8!!!
//
bool pysvn_callbacks::contextGetLogMessage( std::string & _msg )
	{
	PythonDisallowThreads callback_permission( permission );

	return get_string( pyfn_GetLogMessage, _msg );
	}

//
// this method is called if there is ssl server
// information, that has to be confirmed by the user
//
// @retval false prompt was cancelled
//
bool pysvn_callbacks::contextSslServerTrustPrompt (SslServerTrustData & data)
	{
	PythonDisallowThreads callback_permission( permission );

	return false;
	}

//
// this method is called if there is ssl server
// information, that has to be confirmed by the user
//
// @param data 
// @return @a SslServerTrustAnswer
//
svn::ContextListener::SslServerTrustAnswer pysvn_callbacks::contextSslServerTrustPrompt
		( 
		const SslServerTrustData & data, 
		apr_uint32_t & acceptedFailures
		)
	{
	PythonDisallowThreads callback_permission( permission );

	return DONT_ACCEPT;
	}



//
// this method is called to retrieve client side
// information
//
bool pysvn_callbacks::contextSslClientCertPrompt( std::string & certFile )
	{
	PythonDisallowThreads callback_permission( permission );

	return get_string( pyfn_SslClientCertPrompt, certFile );
	}

//
// this method is called to retrieve the password
// for the certificate
//
// @param password
//
bool pysvn_callbacks::contextSslClientCertPwPrompt( std::string &password )
	{
	PythonDisallowThreads callback_permission( permission );

	return get_string( pyfn_SslClientCertPwPrompt, password );
	}

// common get a string implementation
static bool get_string( Py::Object &fn, std::string & msg )
	{
	// make sure we can call the users object
	if( !fn.isCallable() )
		return false;

	Py::Callable callback( fn );
	Py::Tuple args( 0 );

	Py::Tuple results;
	Py::Int retcode;
	Py::String message;

	try
		{
		results = callback.apply( args );
		retcode = results[0];
		message = results[1];

		// true returned
		if( long( retcode ) != 0 )
			{
			// copy out the answers
			msg = message.as_std_string();

			return true;
			}
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();
		}

	return false;
	}
