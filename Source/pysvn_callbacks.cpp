//
// ====================================================================
// Copyright (c) 2003-2004 Barry A Scott.  All rights reserved.
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

static bool get_string( Py::Object &fn, Py::Tuple &args, std::string & _msg );

pysvn_callbacks::pysvn_callbacks()
	: m_pyfn_GetLogin()
	, m_pyfn_Notify()
	, m_pyfn_GetLogMessage()
	, m_pyfn_SslServerPrompt()
	, m_pyfn_SslServerTrustPrompt()
	, m_pyfn_SslClientCertPrompt()
	, m_pyfn_SslClientCertPwPrompt()
	, m_permission( NULL )
	, m_error_message()
	{ }


pysvn_callbacks::~pysvn_callbacks()
	{ }

bool pysvn_callbacks::hasPermission()
	{
	return m_permission != NULL;
	}

void pysvn_callbacks::setPermission( PythonAllowThreads &_permission )
	{
	assert( m_permission == NULL );
	m_permission = &_permission;
	m_error_message = "";
	}

void pysvn_callbacks::clearPermission()
	{
	m_permission = NULL;
	}

void pysvn_callbacks::checkForError( Py::ExtensionExceptionType &exception_for_error )
	{
	// see if any errors occurred in the callbacks
	if( !m_error_message.empty() )
		{
		throw Py::Exception( exception_for_error, m_error_message );
		}
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
		std::string & _password,
		bool &_may_save )
	{
	PythonDisallowThreads callback_permission( m_permission );

	// make sure we can call the users object
	if( !m_pyfn_GetLogin.isCallable() )
		{
		m_error_message = "callback_get_login required";
		return false;
		}

	Py::Callable callback( m_pyfn_GetLogin );

	Py::Tuple args( 3 );
	args[0] = Py::String( _realm );
	args[1] = Py::String( _username );
	args[2] = Py::Int( (long)_may_save );

	// bool, username, password
	Py::Tuple results;
	Py::Int retcode;
	Py::String username;
	Py::String password;
	Py::Int may_save_out;

	try
		{
		results = callback.apply( args );
		retcode = results[0];
		username = results[1];
		password = results[2];
		may_save_out = results[3];

		// true returned
		if( long( retcode ) != 0 )
			{
			// copy out the answers
			_username = username.as_std_string();
			_password = password.as_std_string();
			_may_save = long( may_save_out ) != 0;

			return true;
			}
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();

		m_error_message = "unhandled exception in callback_get_login";

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
	PythonDisallowThreads callback_permission( m_permission );

	// make sure we can call the users object
	if( !m_pyfn_Notify.isCallable() )
		return;

	Py::Callable callback( m_pyfn_Notify );

	Py::Tuple args( 1 );
	Py::Dict info;
	args[0] = info;

	info["path"] = Py::String( path );
	info["action"] = toEnumValue( action );
	info["kind"] = toEnumValue( kind );
	if( mime_type == NULL )
		info["mime_type"] = Py::Nothing();
	else
		info["mime_type"] = Py::String( mime_type );
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

		m_error_message = "unhandled exception in callback_notify";
		}
	}

//
//	Return true to cancel a long running operation
//
bool pysvn_callbacks::contextCancel()
	{
	PythonDisallowThreads callback_permission( m_permission );

	// make sure we can call the users object
	if( !m_pyfn_Cancel.isCallable() )
		return false;

	Py::Callable callback( m_pyfn_Cancel );

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

		m_error_message = "unhandled exception in callback_cancel";

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
	PythonDisallowThreads callback_permission( m_permission );
	
	if( !m_pyfn_GetLogMessage.isCallable() )
		{
		m_error_message = "callback_get_log_message required";
		return false;
		}

	Py::Tuple args( 0 );
	try
		{
		return get_string( m_pyfn_GetLogMessage, args, _msg );
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();

		m_error_message = "unhandled exception in callback_get_log_message";
		}

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
	PythonDisallowThreads callback_permission( m_permission );

	// make sure we can call the users object
	if( !m_pyfn_SslServerTrustPrompt.isCallable() )
		{
		m_error_message = "callback_ssl_server_trust_prompt required";

		return DONT_ACCEPT;
		}

	Py::Callable callback( m_pyfn_SslServerTrustPrompt );

	Py::Dict trust_data;
	trust_data[Py::String("failures")] = Py::Int( long(data.failures) );
	trust_data[Py::String("hostname")] = Py::String( data.hostname );
	trust_data[Py::String("finger_print")] = Py::String( data.fingerprint );
	trust_data[Py::String("valid_from")] = Py::String( data.validFrom );
	trust_data[Py::String("valid_until")] = Py::String( data.validUntil );
	trust_data[Py::String("issuer_dname")] = Py::String( data.issuerDName );
	trust_data[Py::String("realm")] = Py::String( data.realm );

	Py::Tuple args( 1 );
	args[0] = trust_data;

	Py::Tuple result_tuple;
	Py::Int retcode;
	Py::Int accepted_failures;
	Py::Int may_save;

	try
		{
		result_tuple = callback.apply( args );
		retcode = result_tuple[0];
		accepted_failures = result_tuple[1];
		may_save = result_tuple[2];

		acceptedFailures = long(accepted_failures);
		if( long( retcode ) != 0 )
			if( long( may_save ) != 0 )
				return ACCEPT_PERMANENTLY;
			else
				return ACCEPT_TEMPORARILY;
		else
			return DONT_ACCEPT;
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();

		m_error_message = "unhandled exception in callback_ssl_server_trust_prompt";
		}

	return DONT_ACCEPT;
	}


//
// this method is called to retrieve client side
// information
//
bool pysvn_callbacks::contextSslClientCertPrompt( std::string & certFile )
	{
	PythonDisallowThreads callback_permission( m_permission );

	if( !m_pyfn_SslClientCertPrompt.isCallable() )
		{
		m_error_message = "callback_ssl_client_cert_prompt required";

		return false;
		}

	Py::Tuple args( 0 );
	try
		{
		return get_string( m_pyfn_SslClientCertPrompt, args, certFile );
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();

		m_error_message = "unhandled exception in callback_ssl_client_cert_prompt";
		}

	return false;
	}

//
// this method is called to retrieve the password
// for the certificate
//
// @param password
//
bool pysvn_callbacks::contextSslClientCertPwPrompt( std::string &_password, 
		const std::string &_realm, bool &_may_save )
	{
	PythonDisallowThreads callback_permission( m_permission );

	// make sure we can call the users object
	if( !m_pyfn_SslClientCertPwPrompt.isCallable() )
		{
		m_error_message = "callback_ssl_client_cert_password_prompt required";

		return false;
		}

	Py::Callable callback( m_pyfn_SslClientCertPwPrompt );

	Py::Tuple args( 2 );
	args[0] = Py::String( _realm );
	args[1] = Py::Int( (long)_may_save );

	// bool, username, password
	Py::Tuple results;
	Py::Int retcode;
	Py::String username;
	Py::String password;
	Py::Int may_save_out;

	try
		{
		results = callback.apply( args );
		retcode = results[0];
		password = results[1];
		may_save_out = results[2];

		// true returned
		if( long( retcode ) != 0 )
			{
			// copy out the answers
			_password = password.as_std_string();
			_may_save = long( may_save_out ) != 0;

			return true;
			}
		}
	catch( Py::Exception &e )
		{
		PyErr_Print();
		e.clear();

		m_error_message = "unhandled exception in callback_ssl_client_cert_password_prompt";

		return false;
		}

	return false;
	}

// common get a string implementation
static bool get_string( Py::Object &fn, Py::Tuple &args, std::string & msg )
	{
	// make sure we can call the users object
	if( !fn.isCallable() )
		return false;

	Py::Callable callback( fn );

	Py::Tuple results;
	Py::Int retcode;
	Py::String message;

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

	return false;
	}
