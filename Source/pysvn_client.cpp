//
// ====================================================================
// (c) 2003 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
//
//	pysvn_client.cpp
//

#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "pysvn.hpp"
#include "pysvn_docs.hpp"
#include "targets.hpp"
#include "status.hpp"
#include "svn_path.h"

Py::Object toObject( apr_time_t t )
	{
	return Py::Float( double( t )/1000000 );
	}

Py::Object toObject( svn::PathPropertiesMapList &prop_list )
	{
	Py::List py_path_propmap_list;

	svn::PathPropertiesMapList::iterator prop_it = prop_list.begin();
	svn::PathPropertiesMapList::iterator prop_end_it = prop_list.end();
	while( prop_it != prop_end_it )
		{
		Py::Dict py_prop_dict;
		
		svn::PropertiesMap::iterator prop_map_it = (*prop_it).second.begin();
		svn::PropertiesMap::iterator prop_map_end_it = (*prop_it).second.end();
		while( prop_map_it != prop_map_end_it )
			{
			py_prop_dict[ Py::String( (*prop_map_it).first ) ] = Py::String( (*prop_map_it).second );
			++prop_map_it;
			}

		Py::Tuple py_path_proplist(2);
		py_path_proplist[0] = Py::String( osNormalisedPath( (*prop_it).first ) );
		py_path_proplist[1] = py_prop_dict;

		py_path_propmap_list.append( py_path_proplist );
		++prop_it;
		}

	return py_path_propmap_list;
	}

static bool get_optional_boolean( const char *name, bool default_value, const Py::Dict &kws )
	{
	bool boolean = default_value;
	if( kws.hasKey( name ) )
		{
		Py::Int boolean_val( kws[ name ] );
		boolean = boolean_val != 0;
		}

	return boolean;
	}

static void get_optional_revision( svn_opt_revision_t &revision,
	const char *name, svn_opt_revision_kind default_value,
	const Py::Dict &kws )
	{
	revision.kind = default_value;
	if( revision.kind == svn_opt_revision_number )
		revision.value.number = 1;

	if( kws.hasKey( name ) )
		{
		Py::Object obj( kws[ name ] );
		if( pysvn_revision::check( obj ) )
			{
			pysvn_revision *rev = static_cast<pysvn_revision *>( obj.ptr() );
			// copy out to caller
			revision = rev->getSvnRevision();
			}
		else
			{
			std::string msg( "expecting revision object for keyword " );
			msg += name;
			throw Py::AttributeError( msg );
			}
		}
	}

static void get_revision( svn_opt_revision_t &revision,
	const Py::Object &rev_obj )
	{
	revision.kind = svn_opt_revision_head;
	// TBD
	}

//--------------------------------------------------------------------------------
pysvn_client::pysvn_client( pysvn_module &_module )
	: apr_context()
	, svn_context()
	, client_callbacks()
	, svn_client()
	, module( _module )
	{
	svn_context.reset();
	svn_context.setListener( &client_callbacks );
	svn_client.setContext( &svn_context );
	}

pysvn_client::~pysvn_client()
	{ }

Py::Object pysvn_client::getattr( const char *_name )
	{
	std::string name( _name );

	// std::cout << "getattr( " << name << " )" << std::endl << std::flush;
	
	if( name == "__members__" )
		{
		Py::List members;

		members.append( Py::String( "callback_get_login" ) );
		members.append( Py::String( "callback_notify" ) );
		members.append( Py::String( "callback_cancel" ) );
		members.append( Py::String( "callback_get_log_message" ) );
		members.append( Py::String( "callback_ssl_server_prompt" ) );
		members.append( Py::String( "callback_ssl_client_cert_prompt" ) );
		members.append( Py::String( "callback_ssl_client_cert_password_prompt" ) );

		return members;
		}

	if( name == "callback_get_login" )
		return client_callbacks.pyfn_GetLogin;
	if( name == "callback_notify" )
		return client_callbacks.pyfn_Notify;
	if( name == "callback_cancel" )
		return client_callbacks.pyfn_Cancel;
	if( name == "callback_get_log_message" )
		return client_callbacks.pyfn_GetLogMessage;
	if( name == "callback_ssl_server_prompt" )
		return client_callbacks.pyfn_SslServerPrompt;
	if( name == "callback_ssl_client_cert_prompt" )
		return client_callbacks.pyfn_SslClientCertPrompt;
	if( name == "callback_ssl_client_cert_password_prompt" )
		return client_callbacks.pyfn_SslClientCertPwPrompt;

	return getattr_default( _name );
	}

static void set_callable( Py::Object &callback, const Py::Object &value )
	{
	if( value.is( Py::Nothing() ) || value.isCallable() )
		callback = value;
	else
		throw Py::AttributeError( "expecting None or a callable object" );
	}

int pysvn_client::setattr( const char *_name, const Py::Object &value )
	{
	std::string name( _name );
	if( name == "callback_get_login" )
		set_callable( client_callbacks.pyfn_GetLogin, value );
	else if( name == "callback_notify" )
		set_callable( client_callbacks.pyfn_Notify, value );
	else if( name == "callback_cancel" )
		set_callable( client_callbacks.pyfn_Cancel, value );
	else if( name == "callback_get_log_message" )
		set_callable( client_callbacks.pyfn_GetLogMessage, value );
	else if( name == "callback_ssl_server_prompt" )
		set_callable( client_callbacks.pyfn_SslServerPrompt, value );
	else if( name == "callback_ssl_client_cert_prompt" )
		set_callable( client_callbacks.pyfn_SslClientCertPrompt, value );
	else if( name == "callback_ssl_client_cert_password_prompt" )
		set_callable( client_callbacks.pyfn_SslClientCertPwPrompt, value );
	else
		{
		std::string msg( "Unknown attribute: " );
		msg += name;
		throw Py::AttributeError( msg );
		}
	return 0;
	}


const char *no_keywords[] = {
	NULL
	};

const char *revision_only_keywords[] = {
	"revision",
	NULL
	};

const char *force_only_keywords[] = {
	"force",
	NULL
	};

const char *src_revision_only_keywords[] = {
	"src_revision",
	NULL
	};

const char *recurse_only_keywords[] = {
	"recurse",
	NULL
	};

const char *revision_recurse_keywords[] = {
	"recurse",
	"revision",
	NULL
	};

const char *revision_force_keywords[] = {
	"force",
	"revision",
	NULL
	};

Py::Object pysvn_client::cmd_add(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, recurse_only_keywords, kws );
	Py::List path_list;
	if( args[0].isList() )
		path_list = args[0];
	else
		path_list.append( args[0] );

	// check all members of the list are strings
	for( int i=0; i<path_list.length(); i++ )
		{
		Py::String path_str( path_list[i] );
		}

		
	bool recurse = get_optional_boolean( "recurse", true, kws );

	try
		{
		for( int i=0; i<path_list.length(); i++ )
			{
			Py::String path_str( path_list[i] );
			std::string norm_path( svnNormalisedIfPath( path_str.as_std_string() ) );

			PythonAllowThreads permission( client_callbacks );
			svn_client.add( norm_path.c_str(), recurse );
			}
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

const char *annotate_keywords[] = {
	"revision_start",
	"revision_end",
	NULL
	};


Py::Object pysvn_client::cmd_annotate(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, annotate_keywords, kws );

	Py::String path( args[0] );
	svn_opt_revision_t revision_start;
	get_optional_revision( revision_start, "revision_start", svn_opt_revision_number, kws );
	svn_opt_revision_t revision_end;
	get_optional_revision( revision_end, "revision_end", svn_opt_revision_head, kws );

	const svn::AnnotatedFile *all_entries = NULL;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		all_entries = svn_client.annotate( norm_path.c_str(),
			svn::Revision( &revision_start ),
			svn::Revision( &revision_end ) );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	// convert the entries into python objects
	Py::List entries_list;
	svn::AnnotatedFile::const_iterator entry_it = all_entries->begin();
	while( entry_it != all_entries->end() )
		{
		const svn::AnnotateLine &entry = *entry_it;
		++entry_it;

		Py::Dict entry_dict;
		entry_dict["author"] = Py::String( entry.author() );
		entry_dict["date"] = Py::String( entry.date() );
		entry_dict["line"] = Py::String( entry.line() );
		entry_dict["number"] = Py::Int( long( entry.lineNumber() ) );
		entry_dict["revision"] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, entry.revision() ) );

		entries_list.append( entry_dict );
		}

	delete all_entries;

	return entries_list;
	}

Py::Object pysvn_client::cmd_cat(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, revision_only_keywords, kws );
	Py::String path( args[0] );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );

	std::string text;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		text = svn_client.cat( norm_path.c_str(), svn::Revision( &revision ) );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::String( text );
	}

Py::Object pysvn_client::cmd_checkout(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_recurse_keywords, kws );

	Py::String url( args[0] );
	Py::String path( args[1] );
	bool recurse = get_optional_boolean( "recurse", true, kws );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		revnum = svn_client.checkout( url.as_std_string().c_str(),
					norm_path.c_str(),
					svn::Revision( &revision ), recurse );
		
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_cleanup(const Py::Tuple& args )
	{
	check_arguments( 1, 1, args );

	Py::String path( args[0] );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.cleanup( norm_path.c_str() );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_checkin(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, recurse_only_keywords, kws );

	std::string type_error_message;
	try
		{
		type_error_message = "expecting string for message (arg 2)";

		Py::String message( args[1] );

		type_error_message = "expecting boolean for recurse keyword arg";
		bool recurse = get_optional_boolean( "recurse", true, kws );

		std::vector<svn::Path> vec_targets;
		if( args[0].isList() )
			{
			Py::List path_list( args[0] );

			for( int i=0; i<path_list.length(); i++ )
				{
				type_error_message = "expecting path list members to be strings (arg 1)";

				Py::String path_str( path_list[i] );
				std::string norm_path( svnNormalisedIfPath( path_str.as_std_string() ) );
				svn::Path svn_path( norm_path );
				vec_targets.push_back( svn_path );
				}
			}
		else
			{
			type_error_message = "expecting path to be a string (arg 1)";
			Py::String path_str( args[0] );

			std::string norm_path( svnNormalisedIfPath( path_str.as_std_string() ) );
			svn::Path svn_path( norm_path );
			vec_targets.push_back( svn_path );
			}

		svn::Targets targets( vec_targets );

		svn_revnum_t revnum;
		try
			{
			PythonAllowThreads permission( client_callbacks );
			revnum = svn_client.commit( targets,
						message.as_std_string().c_str(),
						recurse );
			}
		catch( svn::ClientException &e )
			{
			throw Py::Exception( module.client_error, e.message() );
			}

		if( revnum < 0 )
			return Py::Nothing();
		else
			return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
		}
	}

Py::Object pysvn_client::cmd_copy(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, src_revision_only_keywords, kws );

	Py::String src_path( args[0] );
	Py::String dest_path( args[1] );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "src_revision", svn_opt_revision_head, kws );

	try
		{
		std::string norm_src_path( svnNormalisedIfPath( src_path.as_std_string() ) );
		std::string norm_dest_path( svnNormalisedIfPath( dest_path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.copy( norm_src_path.c_str(),
				svn::Revision( &revision ),
				norm_dest_path.c_str() );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

const char *diff_keywords[] = {
	"recurse",
	"revision1",
	"revision2",
	"ignore_ancestry",
	"no_diff_deleted",
	NULL
	};

Py::Object pysvn_client::cmd_diff(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, diff_keywords, kws );

	Py::String tmp_path( args[0] );
	Py::String path( args[1] );
	svn_opt_revision_t revision1;
	get_optional_revision( revision1, "revision1", svn_opt_revision_base, kws );
	svn_opt_revision_t revision2;
	get_optional_revision( revision2, "revision2", svn_opt_revision_working, kws );
	bool recurse = get_optional_boolean( "recurse", true, kws );
	bool ignore_ancestry = get_optional_boolean( "ignore_ancestry", false, kws );
	bool no_diff_deleted = get_optional_boolean( "no_diff_deleted", false, kws );

	std::string text;
	try
		{
		std::string norm_tmp_path( svnNormalisedIfPath( tmp_path.as_std_string() ) );
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		text = svn_client.diff( norm_tmp_path.c_str(),
					norm_path.c_str(),
					svn::Revision( &revision1 ),
					svn::Revision( &revision2 ),
					recurse, ignore_ancestry, no_diff_deleted );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::String( text );
	}

const char *export_keywords[] = {
	"revision",
	"force",
	NULL
	};

Py::Object pysvn_client::cmd_export(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, export_keywords, kws );

	Py::String src_path( args[0] );
	Py::String dest_path( args[1] );
	bool is_url = is_svn_url( src_path.as_std_string() );

	bool force = get_optional_boolean( "force", false, kws );
	svn_opt_revision_t revision;
	if( is_url )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_src_path( svnNormalisedIfPath( src_path.as_std_string() ) );
		std::string norm_dest_path( svnNormalisedIfPath( dest_path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		revnum = svn_client.doExport( norm_src_path.c_str(),
				norm_dest_path.c_str(),
				svn::Revision( &revision ), force );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_info(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, no_keywords, kws );

	Py::String path( args[0] );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn::Entry info = svn_client.info( norm_path.c_str() );
		return Py::asObject( new pysvn_entry( info ) );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}
	}

Py::Object pysvn_client::cmd_import(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 3, 3, args, recurse_only_keywords, kws );

	Py::String path( args[0] );
	Py::String url( args[1] );
	Py::String message( args[2] );

	bool recurse = get_optional_boolean( "recurse", true, kws );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.import( norm_path.c_str(),
				url.as_std_string().c_str(),
				message.as_std_string().c_str(),
				recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}


const char *log_keywords[] = {
	"revision_start",
	"revision_end",
	NULL
	};

Py::Object pysvn_client::cmd_log(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, log_keywords, kws );

	Py::String path( args[0] );
	svn_opt_revision_t revision_start;
	get_optional_revision( revision_start, "revision_start", svn_opt_revision_head, kws );
	svn_opt_revision_t revision_end;
	get_optional_revision( revision_end, "revision_end", svn_opt_revision_number, kws );

	const svn::LogEntries *all_entries = NULL;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		all_entries = svn_client.log( norm_path.c_str(),
			svn::Revision( &revision_start ),
			svn::Revision( &revision_end ) );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	// convert the entries into python objects
	Py::List entries_list;
	svn::LogEntries::const_iterator entry_it = all_entries->begin();
	while( entry_it != all_entries->end() )
		{
		const svn::LogEntry &entry = *entry_it;
		++entry_it;

		Py::Dict entry_dict;
		entry_dict["author"] = Py::String( entry.author );
		entry_dict["date"] = Py::String( entry.date );
		entry_dict["message"] = Py::String( entry.message );
		entry_dict["revision"] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, entry.revision ) );

		entries_list.append( entry_dict );
		}

	delete all_entries;

	return entries_list;
	}

Py::Object pysvn_client::cmd_ls(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, revision_recurse_keywords, kws );

	Py::String path( args[0] );
	bool recurse = get_optional_boolean( "recurse", false, kws );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );

	svn::DirEntries all_entries;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		all_entries = svn_client.ls( norm_path.c_str(), &revision, recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	// convert the entries into python objects
	Py::List entries_list;
	svn::DirEntries::iterator entry_it = all_entries.begin();
	while( entry_it != all_entries.end() )
		{
		svn::DirEntry &entry = *entry_it;
		++entry_it;

		Py::Dict entry_dict;
		entry_dict["name"] = Py::String( osNormalisedPath( entry.name() ) );
		entry_dict["kind"] = Py::Int( entry.kind() );
		entry_dict["size"] = Py::Long( Py::Float( double( static_cast<signed_int64>( entry.size() ) ) ) );
		entry_dict["created_rev"] = Py::Int( entry.createdRev() );
		entry_dict["time"] = toObject( entry.time() );
		entry_dict["last_author"] = Py::String( entry.lastAuthor() );

		entries_list.append( entry_dict );
		}

	return entries_list;
	}

const char *merge_keywords[] = {
	"force",
	"recurse",
	NULL
	};

Py::Object pysvn_client::cmd_merge(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 5, 5, args, merge_keywords, kws );

	Py::String path1( args[0] );
	svn_opt_revision_t revision1;
	get_revision( revision1, args[1] );
	Py::String path2( args[2] );
	svn_opt_revision_t revision2;
	get_revision( revision2, args[3] );
	Py::String local_path( args[4] );

	bool force = get_optional_boolean( "force", false, kws );
	bool recurse = get_optional_boolean( "recurse", true, kws );
	bool notice_ancestry = !get_optional_boolean( "notice_ancestry", false, kws );
	bool dry_run = get_optional_boolean( "dry_run", true, kws );

	try
		{
		std::string norm_path1( svnNormalisedIfPath( path1.as_std_string() ) );
		std::string norm_path2( svnNormalisedIfPath( path2.as_std_string() ) );
		std::string norm_local_path( svnNormalisedIfPath( local_path.as_std_string() ) );

		svn_client.merge( norm_path1.c_str(), svn::Revision( &revision1 ),
				norm_path2.c_str(), svn::Revision( &revision2 ),
				norm_local_path.c_str(),
				force, recurse, notice_ancestry, dry_run );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_mkdir(const Py::Tuple& args )
	{
	check_arguments( 2, 2, args );

	// message that explains to the user the type error that may be thrown next
	std::string type_error_message;

	// args to the mkdir call
	std::string message;
	std::vector<svn::Path> vec_targets;

	try
		{
		type_error_message = "expecting string message (arg 2)";
		Py::String py_message( args[1] );
		message = py_message.as_std_string();

		if( args[0].isList() )
			{
			Py::List path_list( args[0] );

			for( int i=0; i<path_list.length(); i++ )
				{
				type_error_message = "expecting path list members to be strings (arg 1)";

				Py::String path_str( path_list[i] );
				std::string norm_path( svnNormalisedIfPath( path_str.as_std_string() ) );
				svn::Path svn_path( norm_path );
				vec_targets.push_back( svn_path );
				}
			}
		else
			{
			type_error_message = "expecting path to be a string (arg 1)";
			Py::String path_str( args[0] );

			std::string norm_path( svnNormalisedIfPath( path_str.as_std_string() ) );
			svn::Path svn_path( norm_path );
			vec_targets.push_back( svn_path );
			}
		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
		}

	svn::Targets targets( vec_targets );

	try
		{
		PythonAllowThreads permission( client_callbacks );
		svn_client.mkdir( targets, message.c_str() );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_move(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, src_revision_only_keywords, kws );

	Py::String src_path( args[0] );
	Py::String dest_path( args[1] );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "src_revision", svn_opt_revision_head, kws );
	bool force = get_optional_boolean( "force", false, kws );

	try
		{
		std::string norm_src_path( svnNormalisedIfPath( src_path.as_std_string() ) );
		std::string norm_dest_path( svnNormalisedIfPath( dest_path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.move( norm_src_path.c_str(),
				svn::Revision( &revision ),
				norm_dest_path.c_str(),
				force );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_proplist( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 0, 1, args, revision_recurse_keywords, kws );

	Py::String path( args[0] );
	bool recurse = get_optional_boolean( "recurse", false, kws );
	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	svn::PathPropertiesMapList all_entries;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		all_entries = svn_client.proplist( norm_path.c_str(), &revision, recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return toObject( all_entries );
	}

Py::Object pysvn_client::cmd_propget( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_recurse_keywords, kws );

	Py::String propname( args[0] );
	Py::String path( args[1] );

	bool recurse = get_optional_boolean( "recurse", false, kws );
	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	svn::PathPropertiesMapList all_entries;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		all_entries = svn_client.propget( propname.as_std_string().c_str(), 
					norm_path.c_str(), &revision, recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return toObject( all_entries );
	}

Py::Object pysvn_client::cmd_propset( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 3, 3, args, revision_recurse_keywords, kws );

	Py::String propname( args[0] );
	Py::String propval( args[1] );
	Py::String path( args[2] );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	bool recurse = get_optional_boolean( "recurse", false, kws );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.propset( propname.as_std_string().c_str(), propval.as_std_string().c_str(),
					norm_path.c_str(), &revision,
					recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_propdel( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_recurse_keywords, kws );

	Py::String propname( args[0] );
	Py::String path( args[1] );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	bool recurse = get_optional_boolean( "recurse", false, kws );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.propdel( propname.as_std_string().c_str(),
				norm_path.c_str(), &revision,
				recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_revproplist( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, revision_only_keywords, kws );

	Py::String path( args[0] );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	std::pair<svn_revnum_t,svn::PropertiesMap> revnum_map_pair;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		revnum_map_pair = svn_client.revproplist( norm_path.c_str(), &revision );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	Py::Dict py_prop_dict;
	
	svn::PropertiesMap::iterator prop_map_it = revnum_map_pair.second.begin();
	svn::PropertiesMap::iterator prop_map_end_it = revnum_map_pair.second.end();
	while( prop_map_it != prop_map_end_it )
		{
		py_prop_dict[ Py::String( (*prop_map_it).first ) ] = Py::String( (*prop_map_it).second );
		++prop_map_it;
		}

	Py::Tuple result(2);
	result[0] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum_map_pair.first ) );
	result[1] = py_prop_dict;

	return result;
	}

Py::Object pysvn_client::cmd_revpropget( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_only_keywords, kws );

	Py::String propname( args[0] );
	Py::String path( args[1] );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	std::pair<svn_revnum_t,std::string> revnum_value_pair( 0, "" );
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		revnum_value_pair = svn_client.revpropget( propname.as_std_string().c_str(),
				norm_path.c_str(), &revision );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	Py::Tuple result(2);
	result[0] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum_value_pair.first ) );
	result[1] = Py::String( revnum_value_pair.second );

	return result;
	}

Py::Object pysvn_client::cmd_revpropset( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 3, 3, args, revision_force_keywords, kws );

	Py::String propname( args[0] );
	Py::String propval( args[1] );
	Py::String path( args[2] );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	bool force = get_optional_boolean( "force", false, kws );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		revnum = svn_client.revpropset( propname.as_std_string().c_str(),
				propval.as_std_string().c_str(),
				norm_path.c_str(), &revision,
				force );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_revpropdel( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_force_keywords, kws );

	Py::String propname( args[0] );
	Py::String path( args[1] );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	bool force = get_optional_boolean( "force", false, kws );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		revnum = svn_client.revpropdel( propname.as_std_string().c_str(),
				norm_path.c_str(), &revision,
				force );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}


Py::Object pysvn_client::cmd_relocate(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 3, 3, args, recurse_only_keywords, kws );

	Py::String from_url( args[0] );
	Py::String to_url( args[1] );
	Py::String path( args[2] );

	bool recurse = get_optional_boolean( "recurse", true, kws );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.relocate( from_url.as_std_string().c_str(),
				to_url.as_std_string().c_str(),
				norm_path.c_str(),
				recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_remove(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, force_only_keywords, kws );

	bool force = get_optional_boolean( "force", false, kws );

	std::vector<svn::Path> vec_targets;
	if( args[0].isList() )
		{
		Py::List path_list( args[0] );

		for( int i=0; i<path_list.length(); i++ )
			{
			Py::String path_str( path_list[i] );
			std::string norm_path( svnNormalisedIfPath( path_str.as_std_string() ) );
			svn::Path svn_path( norm_path );
			vec_targets.push_back( svn_path );
			}
		}
	else
		{
		Py::String path_str( args[0] );

		std::string norm_path( svnNormalisedIfPath( path_str.as_std_string() ) );
		svn::Path svn_path( norm_path );
		vec_targets.push_back( svn_path );
		}

	svn::Targets targets( vec_targets );

	try
		{
		PythonAllowThreads permission( client_callbacks );
		svn_client.remove( targets, force );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}


Py::Object pysvn_client::cmd_resolved(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, recurse_only_keywords, kws );

	Py::String path( args[0] );
	bool recurse = get_optional_boolean( "recurse", false, kws );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.resolved( norm_path.c_str(), recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_revert(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, recurse_only_keywords, kws );

	Py::String path( args[0] );
	bool recurse = get_optional_boolean( "recurse", false, kws );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		svn_client.revert( norm_path.c_str(), recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

const char *status_keywords[] = {
	"recurse",
	"get_all",
	"update",
	"no_ignore",

	NULL
	};

Py::Object pysvn_client::cmd_status(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, status_keywords, kws );

	Py::String path( args[0] );
	bool recurse = get_optional_boolean( "recurse", true, kws );
	bool get_all = get_optional_boolean( "get_all", true, kws );
	bool update = get_optional_boolean( "update", false, kws );
	bool no_ignore = get_optional_boolean( "no_ignore", false, kws );

	svn::StatusEntries all_status;
	Py::List entries_list;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		if( is_path_dir( norm_path ) )
			{
			PythonAllowThreads permission( client_callbacks );
			all_status = svn_client.status( norm_path.c_str(),
							recurse, get_all, update, no_ignore );

			permission.allowThisThread(); // get access to python env

			// convert the entries into python objects
			svn::StatusEntries::const_iterator entry_it = all_status.begin();
			while( entry_it != all_status.end() )
				{
				const svn::Status &file_status = *entry_it;
				++entry_it;

				entries_list.append( Py::asObject( new pysvn_status( file_status ) ) );
				}
			}
		else
			{
			PythonAllowThreads permission( client_callbacks );
			svn::Status file_status;

			file_status = svn_client.singleStatus( norm_path.c_str() );

			permission.allowThisThread(); // get access to python env

			entries_list.append( Py::asObject( new pysvn_status( file_status ) ) );
			}
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}


	return entries_list;
	}

Py::Object pysvn_client::cmd_switch(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_recurse_keywords, kws );

	Py::String path( args[0] );
	Py::String url( args[1] );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	bool recurse = get_optional_boolean( "recurse", true, kws );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		revnum = svn_client.doSwitch( norm_path.c_str(),
				url.as_std_string().c_str(),
				svn::Revision( &revision ), recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_update(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, revision_recurse_keywords, kws );

	Py::String path( args[0] );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	bool recurse = get_optional_boolean( "recurse", true, kws );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string() ) );

		PythonAllowThreads permission( client_callbacks );
		revnum = svn_client.update( norm_path.c_str(),
					svn::Revision( &revision ), recurse );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::is_url( const Py::Tuple& args )
	{
	check_arguments( 1, 1, args );

	Py::String path( args[0] );

	Py::Int result( is_svn_url( path.as_std_string() ) );
	return result;
	}

Py::Object pysvn_client::set_auth_cache( const Py::Tuple& args )
	{
	check_arguments( 1, 1, args );

	Py::Int enable( args[0] );
	try
		{
		svn_context.setAuthCache( long( enable ) != 0 );
		}
	catch( svn::ClientException &e )
		{
		throw Py::Exception( module.client_error, e.message() );
		}

	return Py::Nothing();
	}

void pysvn_client::init_type()
	{
	behaviors().name("Client");
	behaviors().doc( class_client_doc );
	behaviors().supportGetattr();
	behaviors().supportSetattr();

	add_varargs_method("set_auth_cache", &pysvn_client::set_auth_cache, SET_AUTH_CACHE_DOC );
	add_varargs_method("is_url", &pysvn_client::is_url, IS_URL_DOC );

	add_keyword_method("add", &pysvn_client::cmd_add, SVN_ADD_DOC );
	add_keyword_method("annotate", &pysvn_client::cmd_annotate, SVN_ANNOTATE_DOC );
	add_keyword_method("cat", &pysvn_client::cmd_cat, SVN_CAT_DOC );
	add_keyword_method("checkout", &pysvn_client::cmd_checkout, SVN_CHECKOUT_DOC );
	add_varargs_method("cleanup", &pysvn_client::cmd_cleanup, SVN_CLEANUP_DOC );
	add_keyword_method("checkin", &pysvn_client::cmd_checkin, SVN_CHECKIN_DOC );
	add_keyword_method("copy", &pysvn_client::cmd_copy, SVN_COPY_DOC );
	add_keyword_method("diff", &pysvn_client::cmd_diff, SVN_DIFF_DOC );
	add_keyword_method("export", &pysvn_client::cmd_export, SVN_EXPORT_DOC );
	add_keyword_method("info", &pysvn_client::cmd_info, SVN_INFO_DOC );
	add_keyword_method("import_", &pysvn_client::cmd_import, SVN_IMPORT_DOC );
	add_keyword_method("log", &pysvn_client::cmd_log, SVN_LOG_DOC );
	add_keyword_method("ls", &pysvn_client::cmd_ls, SVN_LS_DOC );
	add_keyword_method("merge", &pysvn_client::cmd_merge, SVN_MERGE_DOC );
	add_varargs_method("mkdir", &pysvn_client::cmd_mkdir, SVN_MKDIR_DOC );
	add_keyword_method("move", &pysvn_client::cmd_move, SVN_MOVE_DOC );
	add_keyword_method("proplist", &pysvn_client::cmd_proplist, SVN_PROPLIST_DOC );
	add_keyword_method("propget", &pysvn_client::cmd_propget, SVN_PROPGET_DOC );
	add_keyword_method("propset", &pysvn_client::cmd_propset, SVN_PROPSET_DOC );
	add_keyword_method("propdel", &pysvn_client::cmd_propdel, SVN_PROPDEL_DOC );
	add_keyword_method("revproplist", &pysvn_client::cmd_revproplist, SVN_REVPROPLIST_DOC );
	add_keyword_method("revpropget", &pysvn_client::cmd_revpropget, SVN_REVPROPGET_DOC );
	add_keyword_method("revpropset", &pysvn_client::cmd_revpropset, SVN_REVPROPSET_DOC );
	add_keyword_method("revpropdel", &pysvn_client::cmd_revpropdel, SVN_REVPROPDEL_DOC );
	add_keyword_method("remove", &pysvn_client::cmd_remove, SVN_REMOVE_DOC );
	add_keyword_method("resolved", &pysvn_client::cmd_resolved, SVN_RESOLVED_DOC );
	add_keyword_method("revert", &pysvn_client::cmd_revert, SVN_REVERT_DOC );
	add_keyword_method("status", &pysvn_client::cmd_status, SVN_STATUS_DOC );
	add_keyword_method("switch", &pysvn_client::cmd_switch, SVN_SWITCH_DOC );
	add_keyword_method("update", &pysvn_client::cmd_update, SVN_UPDATE_DOC );
	}

//--------------------------------------------------------------------------------
void pysvn_enum< svn_opt_revision_kind >::init_type(void)
	{
	behaviors().name("opt_revision_kind");
	behaviors().doc("opt_revision_kind enumeration");
	behaviors().supportGetattr();
	}

void pysvn_enum< svn_wc_notify_action_t >::init_type(void)
	{
	behaviors().name("wc_notify_action");
	behaviors().doc("`wc_notify_action enumeration");
	behaviors().supportGetattr();
	}

void pysvn_enum< svn_wc_status_kind >::init_type(void)
	{
	behaviors().name("wc_status_kind");
	behaviors().doc("wc_status_kind enumeration");
	behaviors().supportGetattr();
	}

void pysvn_enum< svn_wc_schedule_t >::init_type(void)
	{
	behaviors().name("wc_schedule");
	behaviors().doc("wc_schedule enumeration");
	behaviors().supportGetattr();
	}

void pysvn_enum< svn_wc_merge_outcome_t >::init_type(void)
	{
	behaviors().name("wc_merge_outcome");
	behaviors().doc("wc_merge_outcome enumeration");
	behaviors().supportGetattr();
	}

void pysvn_enum< svn_wc_notify_state_t >::init_type(void)
	{
	behaviors().name("wc_notify_state");
	behaviors().doc("wc_notify_state enumeration");
	behaviors().supportGetattr();
	}

void pysvn_enum< svn_node_kind_t >::init_type(void)
	{
	behaviors().name("node_kind");
	behaviors().doc("node_kind enumeration");
	behaviors().supportGetattr();
	}

//--------------------------------------------------------------------------------
void pysvn_enum_value< svn_opt_revision_kind >::init_type(void)
	{
	behaviors().name("opt_revision_kind");
	behaviors().doc("opt_revision_kind value");
	behaviors().supportCompare();
	behaviors().supportRepr();
	behaviors().supportStr();
	behaviors().supportHash();
	}

void pysvn_enum_value< svn_wc_notify_action_t >::init_type(void)
	{
	behaviors().name("wc_notify_action");
	behaviors().doc("wc_notify_action value");
	behaviors().supportCompare();
	behaviors().supportRepr();
	behaviors().supportStr();
	behaviors().supportHash();
	}

void pysvn_enum_value< svn_wc_status_kind >::init_type(void)
	{
	behaviors().name("wc_status_kind");
	behaviors().doc("wc_status_kind value");
	behaviors().supportCompare();
	behaviors().supportRepr();
	behaviors().supportStr();
	behaviors().supportHash();
	}

void pysvn_enum_value< svn_wc_merge_outcome_t >::init_type(void)
	{
	behaviors().name("wc_merge_outcome");
	behaviors().doc("wc_merge_outcome value");
	behaviors().supportCompare();
	behaviors().supportRepr();
	behaviors().supportStr();
	behaviors().supportHash();
	}

void pysvn_enum_value< svn_wc_notify_state_t >::init_type(void)
	{
	behaviors().name("wc_notify_state");
	behaviors().doc("wc_notify_state value");
	behaviors().supportCompare();
	behaviors().supportRepr();
	behaviors().supportStr();
	behaviors().supportHash();
	}

void pysvn_enum_value< svn_node_kind_t >::init_type(void)
	{
	behaviors().name("node_kind");
	behaviors().doc("node_kind value");
	behaviors().supportCompare();
	behaviors().supportRepr();
	behaviors().supportStr();
	behaviors().supportHash();
	}
