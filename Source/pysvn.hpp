//
// ====================================================================
// Copyright (c) 2003 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
#include "Python.h"
#include "CXX/Objects.hxx"
#include "CXX/Extensions.hxx"
#include <iostream>

// svncpp includes
#include "apr.hpp"
#include "client.hpp"
#include "dirent.hpp"
#include "exception.hpp"
#include "revision.hpp"
#include "status.hpp"
#include "context.hpp"
#include "context_listener.hpp"
#include "property.hpp"

#include <string>
#include <list>
#include <map>

//--------------------------------------------------------------------------------
class pysvn_module : public Py::ExtensionModule<pysvn_module>
	{
public:
	pysvn_module();
	virtual ~pysvn_module();
private:// functions
	Py::Object new_client( const Py::Tuple &args );
	Py::Object new_revision( const Py::Tuple &args );

public:// variables
	Py::ExtensionExceptionType client_error;
	};


//--------------------------------------------------------------------------------
class PythonAllowThreads;

class pysvn_callbacks : public svn::ContextListener
	{
public:
	pysvn_callbacks();
	virtual ~pysvn_callbacks();

	void setPermission( PythonAllowThreads &_permission );
	void clearPermission();
public:	// data

	//
	// Python class that has implements the callback methods
	//
	Py::Object pyfn_GetLogin;
	Py::Object pyfn_Notify;
	Py::Object pyfn_GetLogMessage;
	Py::Object pyfn_SslServerPrompt;
	Py::Object pyfn_SslServerTrustAnswer;
	Py::Object pyfn_SslClientCertPrompt;
	Py::Object pyfn_SslClientCertPwPrompt;

private:// methods

	//
	// this method will be called to retrieve
	// authentication information
	//
	// WORKAROUND FOR apr_xlate PROBLEM: 
	// STRINGS ALREADY HAVE TO BE UTF8!!!
	//
	// @retval true continue
	//
	bool contextGetLogin (const std::string & realm,
                     std::string & username, 
                     std::string & password);

	// 
	// this method will be called to notify about
	// the progress of an ongoing action
	//
	void contextNotify (const char *path,
		       svn_wc_notify_action_t action,
		       svn_node_kind_t kind,
		       const char *mime_type,
		       svn_wc_notify_state_t content_state,
		       svn_wc_notify_state_t prop_state,
		       svn_revnum_t revision);

	//
	// this method will be called to retrieve
	// a log message
	//
	// WORKAROUND FOR apr_xlate PROBLEM: 
	// STRINGS ALREADY HAVE TO BE UTF8!!!
	//
	bool contextGetLogMessage( std::string & msg );

	//
	// this method is called if there is ssl server
	// information, that has to be confirmed by the user
	//
	// @retval false prompt was cancelled
	//
	bool contextSslServerTrustPrompt( SslServerTrustData & data );

	//
	// this method is called if there is ssl server
	// information, that has to be confirmed by the user
	//
	// @param data 
	// @return @a SslServerTrustAnswer
	//
	SslServerTrustAnswer
	contextSslServerTrustPrompt(
		const SslServerTrustData & data, 
		long & acceptedFailures );

	//
	// this method is called to retrieve client side
	// information
	//
	bool contextSslClientCertPrompt( std::string & certFile );

	//
	// this method is called to retrieve the password
	// for the certificate
	//
	// @param password
	//
	bool contextSslClientCertPwPrompt( std::string & password );

private:// vaiables

	PythonAllowThreads *permission;
	};

class pysvn_client : public Py::PythonExtension<pysvn_client>
	{
public:
	pysvn_client( pysvn_module &module );
	virtual ~pysvn_client();

	// override functions from PythonExtension
	virtual Py::Object getattr( const char *name );
	virtual int setattr( const char *name, const Py::Object &value );
	static void init_type( void );

	// SVN commands
	Py::Object cmd_add( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_annotate( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_cat( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_checkout( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_cleanup( const Py::Tuple& args );
	Py::Object cmd_checkin( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_copy( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_diff( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_export( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_info( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_import( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_log( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_ls( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_merge( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_mkdir( const Py::Tuple& args );
	Py::Object cmd_move( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_propdel( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_propget( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_proplist( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_propset( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_relocate( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_remove( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_resolved( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_revert( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_revpropdel( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_revpropget( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_revproplist( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_revpropset( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_status( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_switch( const Py::Tuple& args, const Py::Dict &kws );
	Py::Object cmd_update( const Py::Tuple& args, const Py::Dict &kws );

	// SVN commands
	Py::Object is_url( const Py::Tuple& args );
	Py::Object set_auth_cache( const Py::Tuple& args );
private:
	pysvn_module &module;
	pysvn_callbacks client_callbacks;
	svn::Apr apr_context;
	svn::Context svn_context;
	svn::Client svn_client;
	};

class pysvn_properties : public Py::PythonExtension<pysvn_properties>
	{
public:
	pysvn_properties();
	virtual ~pysvn_properties();

	// override functions from PythonExtension
	static void init_type( void );

	// support mapping type
	Py::Object has_key( const Py::Tuple &args );
	Py::Object keys( const Py::Tuple &args );
	Py::Object items( const Py::Tuple &args );

	virtual int mapping_length();
	virtual Py::Object mapping_subscript( const Py::Object & );
	virtual int mapping_ass_subscript( const Py::Object &, const Py::Object & );

private:
	svn::Property svn_properties;
	};

class pysvn_revision : public Py::PythonExtension<pysvn_revision>
	{
public:
	pysvn_revision( svn_opt_revision_kind kind, double date=0.0, int revnum=0 );

	virtual ~pysvn_revision();

	virtual Py::Object getattr( const char *name );
	virtual int setattr( const char *name, const Py::Object &value );

	const svn_opt_revision_t &getSvnRevision() const;

	static void init_type(void);

private:
	pysvn_revision();	// not defined
	svn_opt_revision_t svn_revision;
	};

class pysvn_status : public Py::PythonExtension<pysvn_status>
	{
public:
	pysvn_status( const svn::Status &svn_status );

	virtual ~pysvn_status();

	virtual Py::Object getattr( const char *name );

	static void init_type(void);
private:
	const svn::Status svn_status;
	};



class pysvn_entry : public Py::PythonExtension<pysvn_entry>
	{
public:
	pysvn_entry( const svn::Entry &svn_entry );

	virtual ~pysvn_entry();

	virtual Py::Object getattr( const char *name );

	static void init_type(void);
private:
	const svn::Entry svn_entry;
	};


// help functions to check the arguments pass and throw AttributeError
void check_arguments( int min_args, int max_args, const Py::Tuple &args );
void check_arguments( int min_args, int max_args, const Py::Tuple &args,
		const char **allowed_keywords, const Py::Dict &kws );


extern Py::Object toObject( apr_time_t time );

//------------------------------------------------------------
template<class T> class EnumString
	{
public:
	EnumString();
	~EnumString() {}

	const std::string &toTypeName( T )
		{
		return type_name;
		}

	const std::string &toString( T value )
		{
		static std::string not_found( "-unknown-" );
		EXPLICIT_TYPENAME std::map<T,std::string>::iterator it = enum_to_string.find( value );
		if( it != enum_to_string.end() )
			return (*it).second;

		return not_found;
		}

	bool toEnum( const std::string &string, T &value )
		{
		EXPLICIT_TYPENAME std::map<std::string,T>::iterator it = string_to_enum.find( string );
		if( it != string_to_enum.end() )
			{
			value = (*it).second;
			return true;
			}

		return false;
		}

	EXPLICIT_TYPENAME std::map<std::string,T>::iterator begin()
		{
		return string_to_enum.begin();
		}

	EXPLICIT_TYPENAME std::map<std::string,T>::iterator end()
		{
		return string_to_enum.end();
		}

private:
	void add( T value, std::string string )
		{
		string_to_enum[string] = value;
		enum_to_string[value] = string;
		}
 
	std::string type_name;
	std::map<std::string,T> string_to_enum;
	std::map<T,std::string> enum_to_string;
	};

template<class T> const std::string &toTypeName( T value )
	{
	static EnumString< T > enum_map;

	return enum_map.toTypeName( value );
	}

template<class T> const std::string &toString( T value )
	{
	static EnumString< T > enum_map;

	return enum_map.toString( value );
	}

template<class T> bool toEnum( const std::string &string, T &value )
	{
	static EnumString< T > enum_map;

	return enum_map.toEnum( string, value );
	}

template<class T> Py::List memberList( T value )
	{
	static EnumString< T > enum_map;

	Py::List members;

	EXPLICIT_TYPENAME std::map<std::string,T>::iterator it = enum_map.begin();
	while( it != enum_map.end() )
		{
		members.append( Py::String( (*it).first ) );
		++it;
		}
	
	return members;
	}

//------------------------------------------------------------
template<TEMPLATE_TYPENAME T>
class pysvn_enum_value : public Py::PythonExtension<EXPLICIT_CLASS pysvn_enum_value>
	{
public:
	pysvn_enum_value( T _value)
		: Py::PythonExtension<pysvn_enum_value>()
		, value( _value )
		{ }

	virtual ~pysvn_enum_value()
		{ }

	virtual int compare( const Py::Object &other )
		{
		if( pysvn_enum_value::check( other ) )
			{
			pysvn_enum_value<T> *other_value = static_cast<pysvn_enum_value *>( other.ptr() );
			if( value == other_value->value )
				return 0;

			if( value > other_value->value )
				return 1;
			else
				return -1;
			}
		else
			{
			std::string msg( "expecting " );
			msg += toTypeName( value );
			msg += " object for compare ";
			throw Py::AttributeError( msg );
			}
		}

	virtual Py::Object repr()
		{
		std::string s("<");
		s += toTypeName( value );
		s += ".";
		s += toString( value );
		s += ">";

		return Py::String( s );
		}

	virtual Py::Object str()
		{
		return Py::String( toString( value ) );
		}

	// need a hash so that the enums can go into a map
	virtual long hash()
		{
		static Py::String type_name( toTypeName( value ) );

		// use the value plus the hash of the type name
		return long( value ) + type_name.hashValue();
		}

	static void init_type(void);

public:
	T value;
	};

//------------------------------------------------------------
template<class T>
class pysvn_enum : public Py::PythonExtension<EXPLICIT_CLASS pysvn_enum>
	{
public:
	pysvn_enum()
		: Py::PythonExtension<pysvn_enum>()
		{ }

	virtual ~pysvn_enum()
		{ }

	virtual Py::Object getattr( const char *_name )
		{
		std::string name( _name );
		T value;

		if( name == "__methods__" )
			{
			return Py::List();
			}

		if( name == "__members__" )
			{
			return memberList( static_cast<T>( 0 ) );
			}

		if( toEnum( name, value ) )
			{
			return Py::asObject( new pysvn_enum_value<T>( value ) );
			}

		return getattr_methods( _name );	
		}

	static void init_type(void);
	};

template<class T> Py::Object toEnumValue( const T &value )
	{
	return Py::asObject( new pysvn_enum_value<T>( value ) );
	}

// true if path exists and is a directory
extern bool is_path_dir( const std::string &path );

// true if the path_or_url is a svn url
extern bool is_svn_url( const std::string &path_or_url );

// convert a path to what SVN likes only if its not a URL
extern std::string svnNormalisedIfPath( const std::string &unnormalised );

// convert a path to what the native OS likes
extern std::string osNormalisedPath( const std::string &unnormalised );

// need a type to convert from apr_time_t to signed_int64 to double
#if defined(_MSC_VER)
typedef signed __int64 signed_int64;
#else
typedef signed long long signed_int64;
#endif


//--------------------------------------------------------------------------------
//
//	PythonAllowThreads provides a exception safe
//	wrapper for the C idiom:
//
//	Py_BEGIN_ALLOW_THREADS
//	...Do some blocking I/O operation...
//	Py_END_ALLOW_THREADS
//
//	IN C++ use PythonAllowThreads in main code:
//	{
//	PythonAllowThreads main_permission;
//	...Do some blocking I/O operation that may throw
//	} // allow d'tor grabs the lock
//
//	In C++ use PythonDisallowTheads in callback code:
//	{
//	PythonDisallowTheads permission( main_permission );
//	... Python operations that may throw
//	} // allow d'tor to release the lock
//
//--------------------------------------------------------------------------------
class PythonAllowThreads
	{
public:
	// calls allowOtherThreads()
	PythonAllowThreads( pysvn_callbacks &_callbacks );
	// calls allowThisThread() if necessary
	~PythonAllowThreads();

	void allowOtherThreads();
	void allowThisThread();
private:
	pysvn_callbacks &callbacks;
	PyThreadState *_save;
	};

class PythonDisallowThreads
	{
public:
	// calls allowThisThread()
	PythonDisallowThreads( PythonAllowThreads *_permission );
	// calls allowOtherThreads() if necessary
	~PythonDisallowThreads();
private:
	PythonAllowThreads *permission;
	};

//--------------------------------------------------------------------------------
