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
//	pysvn.cpp
//

#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "pysvn.hpp"
#include "pysvn_docs.hpp"
#include "pysvn_version.hpp"
#include "svn_version.h"


pysvn_module::pysvn_module()
	: Py::ExtensionModule<pysvn_module>( "pysvn" )
	, client_error()
	{
	client_error.init( *this, "ClientError" );

	pysvn_client::init_type();
	pysvn_revision::init_type();
	pysvn_status::init_type();
	pysvn_entry::init_type();

	pysvn_enum< svn_opt_revision_kind >::init_type();
	pysvn_enum< svn_wc_notify_action_t >::init_type();
	pysvn_enum< svn_wc_status_kind >::init_type();
	pysvn_enum< svn_wc_schedule_t >::init_type();
	pysvn_enum< svn_wc_merge_outcome_t >::init_type();
	pysvn_enum< svn_wc_notify_state_t >::init_type();
	pysvn_enum< svn_node_kind_t >::init_type();

	pysvn_enum_value< svn_opt_revision_kind >::init_type();
	pysvn_enum_value< svn_wc_notify_action_t >::init_type();
	pysvn_enum_value< svn_wc_status_kind >::init_type();
	pysvn_enum_value< svn_wc_merge_outcome_t >::init_type();
	pysvn_enum_value< svn_wc_notify_state_t >::init_type();
	pysvn_enum_value< svn_node_kind_t >::init_type();

	add_varargs_method("Client", &pysvn_module::new_client, class_client_doc);
	add_varargs_method("Revision", &pysvn_module::new_revision, class_revision_doc);

	initialize( module_doc );

	Py::Dict d( moduleDictionary() );

	d["ClientError"] = client_error;

	d["copyright"] = Py::String( copyright );
	Py::Tuple version(4);
	version[0] = Py::Int( version_major );
	version[1] = Py::Int( version_minor );
	version[2] = Py::Int( version_patch );
	version[3] = Py::Int( version_build );

	d["version"] = version;

	Py::Tuple svn_version(4);
	svn_version[0] = Py::Int( SVN_VER_MAJOR );
	svn_version[1] = Py::Int( SVN_VER_MINOR );
	svn_version[2] = Py::Int( SVN_VER_MICRO );
	svn_version[3] = Py::String( SVN_VER_TAG );

	d["svn_version"] = svn_version;

	d["opt_revision_kind"] = Py::asObject( new pysvn_enum< svn_opt_revision_kind >() );
	d["wc_notify_action"] = Py::asObject( new pysvn_enum< svn_wc_notify_action_t >() );
	d["wc_status_kind"] = Py::asObject( new pysvn_enum< svn_wc_status_kind >() );
	d["wc_schedule"] = Py::asObject( new pysvn_enum< svn_wc_schedule_t >() );
	d["wc_merge_outcome"] = Py::asObject( new pysvn_enum< svn_wc_merge_outcome_t >() );
	d["wc_notify_state"] = Py::asObject( new pysvn_enum< svn_wc_notify_state_t >() );
	d["node_kind"] = Py::asObject( new pysvn_enum< svn_node_kind_t >() );
	}

pysvn_module::~pysvn_module()
	{}

Py::Object pysvn_module::new_client(const Py::Tuple &args)
	{
	check_arguments( 0, 1, args );

	std::string config_dir;

	if( args.size() == 1 )
		{
		Py::String py_config_dir( args[0] );
		config_dir = py_config_dir.as_std_string();
		}

	return Py::asObject( new pysvn_client( *this, config_dir ) );
	}

Py::Object pysvn_module::new_revision(const Py::Tuple &args)
	{
	//
	// support only one of the following:
	// revision( kind )
	// revision( kind, number )
	// revision( kind, date )
	//
	check_arguments( 1, 2, args );

	Py::ExtensionObject< pysvn_enum_value<svn_opt_revision_kind> > py_kind( args[0] );

	svn_opt_revision_kind kind = svn_opt_revision_kind( py_kind.extensionObject()->value );

	pysvn_revision *rev = NULL;
	switch( kind )
		{
	case svn_opt_revision_date:
		{
		check_arguments( 2, 2, args );
		Py::Float date( args[1] );

		rev = new pysvn_revision( kind, double(date) );
		}
		break;

	case svn_opt_revision_number:
		{
		check_arguments( 2, 2, args );
		Py::Int revnum( args[1] );

		rev = new pysvn_revision( kind, 0, long( revnum ) );
		}
		break;
	default:
		check_arguments( 1, 1, args );

		rev = new pysvn_revision( kind );
		}

	return Py::asObject( rev );
	}

void check_arguments( int min_args, int max_args, const Py::Tuple &args )
	{
	if( args.size() < min_args )
		throw Py::TypeError( "Too few arguments to function" );

	if( args.size() > max_args )
		throw Py::TypeError( "Too many arguments to function" );
	}

void check_arguments( int min_args, int max_args, const Py::Tuple &args,
		const char **allowed_keywords, const Py::Dict &kws )
	{
	check_arguments( min_args, max_args, args );

	Py::List names( kws.keys() );
	for( int i=0; i< names.length(); i++ )
		{
		bool found = false;
		Py::String py_name( names[i] );
		std::string name( py_name );

		const char **allowed_keywords_it = allowed_keywords;
		while( *allowed_keywords_it != NULL )
			{
			if( name == *allowed_keywords_it )
				{
				found = true;
				break;
				}

			++allowed_keywords_it;
			}

		if( !found )
			{
			std::string msg( "Unknown keyword argument to function: " );
			msg += name;
			throw Py::TypeError( msg );
			}
		}
	}

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
PythonAllowThreads::PythonAllowThreads( pysvn_callbacks &_callbacks )
	: callbacks( _callbacks )
	, _save( NULL )
	{
	callbacks.setPermission( *this );
	allowOtherThreads();
	}

PythonAllowThreads::~PythonAllowThreads()
	{
	if( _save != NULL )
		allowThisThread();

	callbacks.clearPermission();
	}

void PythonAllowThreads::allowOtherThreads()
	{
#if defined( WITH_THREAD )
	assert( _save == NULL );

	_save = PyEval_SaveThread();
#endif
	}

void PythonAllowThreads::allowThisThread()
	{
#if defined( WITH_THREAD )
	assert( _save != NULL );

	PyEval_RestoreThread(_save);
	_save = NULL;
#endif
	}

PythonDisallowThreads::PythonDisallowThreads( PythonAllowThreads *_permission )
	: permission( _permission )
	{
	permission->allowThisThread();
	}

PythonDisallowThreads::~PythonDisallowThreads()
	{
	permission->allowOtherThreads();
	}

//--------------------------------------------------------------------------------


extern "C" void init_pysvn()
	{
	static pysvn_module* pysvn = new pysvn_module;
	}

// symbol required for the debug version
extern "C" void init_pysvn_d()
	{
	init_pysvn();
	}
