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
	// init APR once globally - rather then on demand
	// to avoid life time issues with pools
	apr_initialize();
	apr_pool_initialize();

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

	add_keyword_method("Client", &pysvn_module::new_client, class_client_doc);
	add_keyword_method("Revision", &pysvn_module::new_revision, class_revision_doc);

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

static const char name_config_dir[] = "config_dir";

Py::Object pysvn_module::new_client( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ false, name_config_dir },
		{ false, NULL }
		};

	FunctionArguments args( "Client", args_desc, a_args, a_kws );
	args.check();

	std::string config_dir = args.getUtf8String( name_config_dir, "" );

	return Py::asObject( new pysvn_client( *this, config_dir ) );
	}

static const char name_kind[] = "kind";
static const char name_number[] = "number";
static const char name_date[] = "date";

Py::Object pysvn_module::new_revision( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	//
	// support only one of the following:
	// revision( kind )
	// revision( kind, number )
	// revision( kind, date )
	//
	static argument_description args_desc[] =
		{
		{ true, name_kind },
		{ false, name_date },
		{ false, name_number },
		{ false, NULL }
		};
	FunctionArguments args( "Revision", args_desc, a_args, a_kws );
	args.check();

	Py::ExtensionObject< pysvn_enum_value<svn_opt_revision_kind> > py_kind( args.getArg( name_kind ) );

	svn_opt_revision_kind kind = svn_opt_revision_kind( py_kind.extensionObject()->m_value );

	pysvn_revision *rev = NULL;
	switch( kind )
		{
	case svn_opt_revision_date:
		{
		static argument_description args_desc[] =
			{
			{ true, name_kind },
			{ true, name_date },
			{ false, NULL }
			};
		FunctionArguments args( "Revision", args_desc, a_args, a_kws );
		args.check();
		Py::Float date( args.getArg( name_date ) );

		rev = new pysvn_revision( kind, double(date) );
		}
		break;

	case svn_opt_revision_number:
		{
		static argument_description args_desc[] =
			{
			{ true, name_kind },
			{ true, name_number },
			{ false, NULL }
			};
		FunctionArguments args( "Revision", args_desc, a_args, a_kws );
		args.check();
		Py::Int revnum( args.getArg( name_number ) );

		rev = new pysvn_revision( kind, 0, long( revnum ) );
		}
		break;
	default:
		static argument_description args_desc[] =
			{
			{ true, name_kind },
			{ false, NULL }
			};
		FunctionArguments args( "Revision", args_desc, a_args, a_kws );
		args.check();

		rev = new pysvn_revision( kind );
		}

	return Py::asObject( rev );
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
PythonAllowThreads::PythonAllowThreads( pysvn_context &_callbacks )
	: m_callbacks( _callbacks )
	, m_save( NULL )
	{
	m_callbacks.setPermission( *this );
	allowOtherThreads();
	}

PythonAllowThreads::~PythonAllowThreads()
	{
	if( m_save != NULL )
		allowThisThread();

	m_callbacks.clearPermission();
	}

void PythonAllowThreads::allowOtherThreads()
	{
#if defined( WITH_THREAD )
	assert( m_save == NULL );

	m_save = PyEval_SaveThread();
	assert( m_save != NULL );
#endif
	}

void PythonAllowThreads::allowThisThread()
	{
#if defined( WITH_THREAD )
	assert( m_save != NULL );

	PyEval_RestoreThread( m_save );
	m_save = NULL;
#endif
	}

PythonDisallowThreads::PythonDisallowThreads( PythonAllowThreads *_permission )
	: m_permission( _permission )
	{
	m_permission->allowThisThread();
	}

PythonDisallowThreads::~PythonDisallowThreads()
	{
	m_permission->allowOtherThreads();
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
