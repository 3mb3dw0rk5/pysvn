//
// ====================================================================
// (c) 2003-2004 Barry A Scott.  All rights reserved.
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
#include "pysvn_svnenv.hpp"
#include "svn_path.h"
#include "svn_config.h"
#include "svn_sorts.h"
#include "svn_time.h"

static const char g_utf_8[] = "UTF-8";

extern Py::Object utf8_string_or_none( const char *str );
Py::Object utf8_string_or_none( const std::string &str )
	{
	if( !str.empty() )
		return Py::String( str, g_utf_8 );
	else
		return Py::Nothing();
	}

apr_time_t convertStringToTime( const std::string &text, apr_time_t now, SvnPool &pool )
	{
	svn_boolean_t matched = 0;
	apr_time_t result = 0;

	svn_error_t *error = svn_parse_date
		(
		&matched,
		&result,
		text.c_str(),
                now,
		pool
		);
	if( error != NULL || !matched )
		{
		return 0;
		}

	return result;
	}


Py::Object toObject( apr_time_t t )
	{
	return Py::Float( double( t )/1000000 );
	}

Py::Object toObject( svn_client_commit_info_t *commit_info )
	{
	if( commit_info == NULL || !SVN_IS_VALID_REVNUM( commit_info->revision ) )
		return Py::Nothing();

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, commit_info->revision ) );
	}

Py::Object propsToObject( apr_hash_t *props, SvnPool &pool )
	{
	Py::Dict py_prop_dict;

	for( apr_hash_index_t *hi = apr_hash_first( pool, props ); hi; hi = apr_hash_next( hi ) )
		{
		const void *key = NULL;
		void *val = NULL;

		apr_hash_this (hi, &key, NULL, &val);
		const svn_string_t *propval = (const svn_string_t *)val;
		
		py_prop_dict[ Py::String( (const char *)key ) ] = Py::String( propval->data );
		}

	return py_prop_dict;
	}

Py::Object proplistToObject( apr_array_header_t *props, SvnPool &pool )
	{
	Py::List py_path_propmap_list;

	for (int j = 0; j < props->nelts; ++j)
		{

		svn_client_proplist_item_t *item = ((svn_client_proplist_item_t **)props->elts)[j];

		Py::Object py_prop_dict( propsToObject( item->prop_hash, pool ) );

		std::string node_name( item->node_name->data, item->node_name->len );

		Py::Tuple py_path_proplist( 2 );
		py_path_proplist[0] = Py::String( osNormalisedPath( node_name, pool ) );
		py_path_proplist[1] = py_prop_dict;

		py_path_propmap_list.append( py_path_proplist );
		}

	return py_path_propmap_list;
	}

static Py::String asUtf8String( Py::Object obj )
	{
	Py::String any( obj );
	Py::String utf8( any.encode( g_utf_8 ) );
	return utf8;
	}


static apr_array_header_t *targetsFromStringOrList( Py::Object arg, SvnPool &pool )
	{
	int num_targets = 1;
	if( arg.isList() )
		{
		Py::List paths( arg );
		num_targets = paths.length();
		}

	apr_array_header_t *targets = apr_array_make( pool, num_targets, sizeof( const char * ) );

	std::string type_error_message;
	try
		{
		if( arg.isList() )
			{
			Py::List path_list( arg );

			for( int i=0; i<path_list.length(); i++ )
				{
				type_error_message = "expecting path list members to be strings (arg 1)";

				Py::String path_str( asUtf8String( path_list[i] ) );
				std::string norm_path( svnNormalisedIfPath( path_str.as_std_string(), pool ) );

				*(char **)apr_array_push( targets ) = apr_pstrdup( pool, norm_path.c_str() );
				}
			}
		else
			{
			type_error_message = "expecting path to be a string (arg 1)";
			Py::String path_str( asUtf8String( arg ) );

			std::string norm_path( svnNormalisedIfPath( path_str.as_std_string(), pool ) );

			*(char **)apr_array_push( targets ) = apr_pstrdup( pool, norm_path.c_str() );
			}
		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
		}

	return targets;
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
pysvn_client::pysvn_client( pysvn_module &_module, const std::string &config_dir )
	: m_context( config_dir )
	, m_module( _module )
	{
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
		members.append( Py::String( "callback_ssl_server_trust_prompt" ) );
		members.append( Py::String( "callback_ssl_client_cert_prompt" ) );
		members.append( Py::String( "callback_ssl_client_cert_password_prompt" ) );

		return members;
		}

	if( name == "callback_get_login" )
		return m_context.m_pyfn_GetLogin;
	if( name == "callback_notify" )
		return m_context.m_pyfn_Notify;
	if( name == "callback_cancel" )
		return m_context.m_pyfn_Cancel;
	if( name == "callback_get_log_message" )
		return m_context.m_pyfn_GetLogMessage;
	if( name == "callback_ssl_server_prompt" )
		return m_context.m_pyfn_SslServerPrompt;
	if( name == "callback_ssl_server_trust_prompt" )
		return m_context.m_pyfn_SslServerTrustPrompt;
	if( name == "callback_ssl_client_cert_prompt" )
		return m_context.m_pyfn_SslClientCertPrompt;
	if( name == "callback_ssl_client_cert_password_prompt" )
		return m_context.m_pyfn_SslClientCertPwPrompt;

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
		set_callable( m_context.m_pyfn_GetLogin, value );
	else if( name == "callback_notify" )
		set_callable( m_context.m_pyfn_Notify, value );
	else if( name == "callback_cancel" )
		set_callable( m_context.m_pyfn_Cancel, value );
	else if( name == "callback_get_log_message" )
		set_callable( m_context.m_pyfn_GetLogMessage, value );
	else if( name == "callback_ssl_server_prompt" )
		set_callable( m_context.m_pyfn_SslServerPrompt, value );
	else if( name == "callback_ssl_server_trust_prompt" )
		set_callable( m_context.m_pyfn_SslServerTrustPrompt, value );
	else if( name == "callback_ssl_client_cert_prompt" )
		set_callable( m_context.m_pyfn_SslClientCertPrompt, value );
	else if( name == "callback_ssl_client_cert_password_prompt" )
		set_callable( m_context.m_pyfn_SslClientCertPwPrompt, value );
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

const char *src_revision_force_keywords[] = {
	"src_revision",
	"force",
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

	SvnPool pool( m_context );
	try
		{
		for( int i=0; i<path_list.length(); i++ )
			{
			Py::String path_str( asUtf8String( path_list[i] ) );
			std::string norm_path( svnNormalisedIfPath( path_str.as_std_string(), pool ) );

			checkThreadPermission();

			PythonAllowThreads permission( m_context );

			SvnPool pool( m_context );

			svn_error_t * error = svn_client_add
				(
				norm_path.c_str(),
				recurse,
				m_context,
				pool
				);
			if( error != NULL )
				throw SvnException( error );
			}
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

class AnnotatedLineInfo
	{
public:
	AnnotatedLineInfo
		(
		apr_int64_t line_no,
		svn_revnum_t revision,
		const char *author,
		const char *date,
		const char *line
		)
	: m_line_no( line_no )
	, m_revision( revision )
	, m_author( author )
	, m_date( date )
	, m_line( line )
	{
	}
	
	~AnnotatedLineInfo()
	{
	}

	AnnotatedLineInfo( const AnnotatedLineInfo &other )
	: m_line_no( other.m_line_no )
	, m_revision( other.m_revision )
	, m_author( other.m_author )
	, m_date( other.m_date )
	, m_line( other.m_line )
	{
	}

	apr_int64_t m_line_no;
	svn_revnum_t m_revision;
	std::string m_author;
	std::string m_date;
	std::string m_line;
	};

static svn_error_t *annotate_receiver
	(
	void *baton,
	apr_int64_t line_no,
	svn_revnum_t revision,
	const char *author,
	const char *date,
	const char *line,
	apr_pool_t *pool
	)
	{
	std::list<AnnotatedLineInfo> *entries = (std::list<AnnotatedLineInfo> *)baton;
	entries->push_back( AnnotatedLineInfo( line_no, revision, author, date, line ) );

	return NULL;
	}

const char *annotate_keywords[] =
	{
	"revision_start",
	"revision_end",
	NULL
	};

Py::Object pysvn_client::cmd_annotate(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, annotate_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	svn_opt_revision_t revision_start;
	get_optional_revision( revision_start, "revision_start", svn_opt_revision_number, kws );
	svn_opt_revision_t revision_end;
	get_optional_revision( revision_end, "revision_end", svn_opt_revision_head, kws );

	SvnPool pool( m_context );
	std::list<AnnotatedLineInfo> all_entries;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_blame
			(
			norm_path.c_str(),
			&revision_start,
			&revision_end,
			annotate_receiver,
			&all_entries,
			m_context,
			pool
			);

		if( error != NULL )
			{
			throw SvnException( error );
			}
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	// convert the entries into python objects
	Py::List entries_list;
	std::list<AnnotatedLineInfo>::const_iterator entry_it = all_entries.begin();
	while( entry_it != all_entries.end() )
		{
		const AnnotatedLineInfo &entry = *entry_it;
		++entry_it;

		Py::Dict entry_dict;
		entry_dict["author"] = Py::String( entry.m_author, g_utf_8 );
		entry_dict["date"] = Py::String( entry.m_date );
		entry_dict["line"] = Py::String( entry.m_line, g_utf_8 );
		entry_dict["number"] = Py::Int( long( entry.m_line_no ) );
		entry_dict["revision"] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, entry.m_revision ) );

		entries_list.append( entry_dict );
		}

	return entries_list;
	}

Py::Object pysvn_client::cmd_cat(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, revision_only_keywords, kws );
	Py::String path( asUtf8String( args[0] ) );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );

	SvnPool pool( m_context );

	svn_stringbuf_t * stringbuf = svn_stringbuf_create ("", pool);
	svn_stream_t * stream = svn_stream_from_stringbuf (stringbuf, pool);

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t * error = svn_client_cat
			(
			stream,
			norm_path.c_str (),
			&revision,
			m_context,
			pool
			);

		if (error != 0)
			throw SvnException (error);
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::String( stringbuf->data, stringbuf->len, g_utf_8 );
	}

Py::Object pysvn_client::cmd_checkout(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_recurse_keywords, kws );

	Py::String url( asUtf8String( args[0] ) );
	Py::String path( asUtf8String( args[1] ) );
	bool recurse = get_optional_boolean( "recurse", true, kws );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_revnum_t revnum = 0;
		svn_error_t * error = svn_client_checkout
			(
			&revnum,
			url.as_std_string().c_str(),
			norm_path.c_str(),
			&revision,
			recurse,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_cleanup(const Py::Tuple& args )
	{
	check_arguments( 1, 1, args );

	Py::String path( asUtf8String( args[0] ) );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t * error = svn_client_cleanup( norm_path.c_str(), m_context, pool);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_checkin(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, recurse_only_keywords, kws );

	SvnPool pool( m_context );
	svn_client_commit_info_t *commit_info = NULL;

	apr_array_header_t *targets = targetsFromStringOrList( args[0], pool );

	std::string type_error_message;
	try
		{
		type_error_message = "expecting string for message (arg 2)";

		Py::String message( asUtf8String( args[1] ) );

		type_error_message = "expecting boolean for recurse keyword arg";
		bool recurse = get_optional_boolean( "recurse", true, kws );


		try
			{
			checkThreadPermission();

			PythonAllowThreads permission( m_context );

			m_context.setLogMessage( message.as_std_string() );

			svn_error_t *error = svn_client_commit
				(
				&commit_info,
				targets,
				!recurse,		// non recursive
				m_context,
				pool
				);
			if( error != NULL )
				throw SvnException( error );
			}
		catch( SvnException &e )
			{
			throw Py::Exception( m_module.client_error, e.message() );
			}
		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
		}

	return toObject( commit_info );
	}

Py::Object pysvn_client::cmd_copy(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, src_revision_only_keywords, kws );

	Py::String src_path( asUtf8String( args[0] ) );
	Py::String dest_path( asUtf8String( args[1] ) );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "src_revision", svn_opt_revision_head, kws );

	SvnPool pool( m_context );
	svn_client_commit_info_t *commit_info = NULL;

	std::string type_error_message;
	try
		{
		type_error_message = "expecting string for src_path (arg 1)";
		Py::String src_path( asUtf8String( args[0] ) );

		type_error_message = "expecting string for dest_path (arg 2)";
		Py::String dest_path( asUtf8String( args[1] ) );

		svn_opt_revision_t revision;
		type_error_message = "expecting revision for keyword src_revision";
		get_optional_revision( revision, "src_revision", svn_opt_revision_head, kws );

		type_error_message = "expecting boolean for keyword force";
		bool force = get_optional_boolean( "force", false, kws );

		try
			{
			std::string norm_src_path( svnNormalisedIfPath( src_path.as_std_string(), pool ) );
			std::string norm_dest_path( svnNormalisedIfPath( dest_path.as_std_string(), pool ) );

			checkThreadPermission();

			PythonAllowThreads permission( m_context );

			svn_error_t *error = svn_client_copy
				(
				&commit_info,
				norm_src_path.c_str(),
				&revision,
				norm_dest_path.c_str(),
				m_context,
				pool
				);

			if( error != NULL )
				throw SvnException( error );
			}
		catch( SvnException &e )
			{
			// use callback error over ClientException
			m_context.checkForError( m_module.client_error );

			throw Py::Exception( m_module.client_error, e.message() );
			}
		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
		}

	return toObject( commit_info );
	}

class pysvn_apr_file
	{
public:
	pysvn_apr_file( SvnPool &pool )
		: m_pool( pool )
		, m_apr_file( NULL )
		, m_filename( NULL )
		{
		}

	~pysvn_apr_file()
		{
		if( m_apr_file )
			apr_file_close( m_apr_file );
		if( m_filename )
			svn_error_clear( svn_io_remove_file( m_filename, m_pool ) );
		}

	void open_unique_file( const std::string &tmp_dir )
		{
		svn_error_t *error = svn_io_open_unique_file
			(
			&m_apr_file,
			&m_filename,
			tmp_dir.c_str(),
			".tmp",
                        false,
			m_pool
			);
		if( error != NULL )
			throw SvnException( error );
		}

	
	void open_tmp_file()
		{
		apr_status_t status = apr_file_open( &m_apr_file, m_filename, APR_READ, APR_OS_DEFAULT, m_pool );
		if( status )
			{
			std::string msg( "openning file " ); msg += m_filename;
			throw SvnException( svn_error_create( status, NULL, msg.c_str() ) );
			}
		}

	void close()
		{
		// must ensure that m_apr_file is NULL to prevent
		// closing a second time in the d'tor
		apr_file_t *apr_file = m_apr_file;
		m_apr_file = NULL;

		apr_status_t status = apr_file_close( apr_file );
		if( status )
			{
			std::string msg( "closing file " ); msg += m_filename;
			throw SvnException( svn_error_create( status, NULL, msg.c_str() ) );
			}
		}

	apr_file_t *file()
		{
		return m_apr_file;
		}

private:
	SvnPool &m_pool;
	apr_file_t *m_apr_file;
	const char *m_filename;
	};

const char *diff_keywords[] = {
	"recurse",
	"revision1",
	"revision2",
	"ignore_ancestry",
	"no_diff_deleted",
	NULL
	};

// TODO: add path2 to allow diff of any to any
Py::Object pysvn_client::cmd_diff(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, diff_keywords, kws );

	Py::String tmp_path( asUtf8String( args[0] ) );
	Py::String path( asUtf8String( args[1] ) );
	svn_opt_revision_t revision1;
	get_optional_revision( revision1, "revision1", svn_opt_revision_base, kws );
	svn_opt_revision_t revision2;
	get_optional_revision( revision2, "revision2", svn_opt_revision_working, kws );
	bool recurse = get_optional_boolean( "recurse", true, kws );
	bool ignore_ancestry = get_optional_boolean( "ignore_ancestry", false, kws );
	bool no_diff_deleted = get_optional_boolean( "no_diff_deleted", false, kws );

	SvnPool pool( m_context );

	svn_stringbuf_t *stringbuf = NULL;

	try
		{
		std::string norm_tmp_path( svnNormalisedIfPath( tmp_path.as_std_string(), pool ) );
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		pysvn_apr_file output_file( pool );
		pysvn_apr_file error_file( pool );

		output_file.open_unique_file( norm_tmp_path );
		error_file.open_unique_file( norm_tmp_path );

		apr_array_header_t *options = apr_array_make( pool, 0, 0 );

		svn_error_t *error = svn_client_diff
			(
			options,
			norm_path.c_str(), &revision1,
			norm_path.c_str(), &revision2,
			recurse,
			ignore_ancestry,
			no_diff_deleted,
			output_file.file(),
			error_file.file(),
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );

		output_file.open_tmp_file();
		error = svn_stringbuf_from_aprfile( &stringbuf, output_file.file(), pool );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::String( stringbuf->data, stringbuf->len, g_utf_8 );
	}

const char *export_keywords[] = {
	"revision",
	"force",
	NULL
	};

Py::Object pysvn_client::cmd_export(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, export_keywords, kws );

	Py::String src_path( asUtf8String( args[0] ) );
	Py::String dest_path( asUtf8String( args[1] ) );
	bool is_url = is_svn_url( src_path.as_std_string() );

	bool force = get_optional_boolean( "force", false, kws );
	svn_opt_revision_t revision;
	if( is_url )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	svn_revnum_t revnum = 0;

	SvnPool pool( m_context );

	try
		{
		std::string norm_src_path( svnNormalisedIfPath( src_path.as_std_string(), pool ) );
		std::string norm_dest_path( svnNormalisedIfPath( dest_path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t * error = svn_client_export
			(
			&revnum,
			norm_src_path.c_str(),
			norm_dest_path.c_str(),
			&revision,
			force,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_info(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, no_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );

	SvnPool pool( m_context );
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_wc_adm_access_t *adm_access = NULL;

		svn_error_t *error = svn_wc_adm_probe_open( &adm_access, NULL, norm_path.c_str(), false, false, pool );
		if( error != NULL )
			throw SvnException( error );

		const svn_wc_entry_t *entry = NULL;
		error = svn_wc_entry( &entry, norm_path.c_str(), adm_access, false, pool );
		if( error != NULL )
			throw SvnException( error );

		if( entry == NULL )
			return Py::Nothing();

		return Py::asObject( new pysvn_entry( entry, m_context ) );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}
	}

Py::Object pysvn_client::cmd_import(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 3, 3, args, recurse_only_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	Py::String url( asUtf8String( args[1] ) );
	Py::String message( asUtf8String( args[2] ) );

	bool recurse = get_optional_boolean( "recurse", true, kws );

	SvnPool pool( m_context );
	svn_client_commit_info_t *commit_info = NULL;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );


		m_context.setLogMessage( message.as_std_string().c_str() );

		svn_error_t *error = svn_client_import
			(
			&commit_info,
			norm_path.c_str(),
			url.as_std_string().c_str(),
			!recurse,		// non_recursive
			m_context,
			pool
			);

		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return toObject( commit_info );
	}


class LogChangePathInfo
	{
public:
	LogChangePathInfo( const char *path, svn_log_changed_path_t *info )
		: m_path( path )
		, m_action( info->action )
		, m_copy_from_path( info->copyfrom_path )
		, m_copy_from_revision( info->copyfrom_rev )
		{
		}

	LogChangePathInfo( const LogChangePathInfo &other )
		: m_path( other.m_path )
		, m_action( other.m_action )
		, m_copy_from_path( other.m_copy_from_path )
		, m_copy_from_revision( other.m_copy_from_revision )
		{
		}

	std::string	m_path;
	char		m_action;
	std::string	m_copy_from_path;
	svn_revnum_t	m_copy_from_revision;
	};

class LogEntryInfo
	{
public:
	LogEntryInfo
		(
		svn_revnum_t revision,
		const char *author,
		const char *date,
		const char *message
		)
		: m_revision( revision )
		, m_author( author )
		, m_date( date )
		, m_message( message )
		, m_changed_paths()
		{
		}
	
	~LogEntryInfo()
		{
		}

	LogEntryInfo( const LogEntryInfo &other )
		: m_revision( other.m_revision )
		, m_author( other.m_author )
		, m_date( other.m_date )
		, m_message( other.m_message )
		, m_changed_paths( other.m_changed_paths )
		{
		}

	svn_revnum_t m_revision;
	std::string m_author;
	std::string m_date;
	std::string m_message;
	std::list<LogChangePathInfo> m_changed_paths;
	};



static svn_error_t *logReceiver
	(
	void *baton,
	apr_hash_t *changedPaths,
	svn_revnum_t rev,
	const char *author,
	const char *date,
	const char *msg,
	apr_pool_t *pool
	)
	{
	std::list<LogEntryInfo> *entries = (std::list<LogEntryInfo> *)baton;

	entries->push_back( LogEntryInfo( rev, author, date, msg ) );

	if( changedPaths != NULL )
		{
		LogEntryInfo &entry = entries->back();

		for( apr_hash_index_t *hi = apr_hash_first( pool, changedPaths );
				hi != NULL;
					hi = apr_hash_next( hi ) )
			{
			char *path = NULL;
			void *val = NULL;
			apr_hash_this( hi, (const void **) &path, NULL, &val );

			svn_log_changed_path_t *log_item = reinterpret_cast<svn_log_changed_path_t *> (val);

			entry.m_changed_paths.push_back( LogChangePathInfo( path, log_item ) );
			}
		}

	return NULL;
	}

const char *log_keywords[] = {
	"discover_changed_paths",
	"strict_node_history",
	"revision_start",
	"revision_end",
	NULL
	};

Py::Object pysvn_client::cmd_log(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, log_keywords, kws );

	svn_opt_revision_t revision_start;
	get_optional_revision( revision_start, "revision_start", svn_opt_revision_head, kws );
	svn_opt_revision_t revision_end;
	get_optional_revision( revision_end, "revision_end", svn_opt_revision_number, kws );
	bool discover_changed_paths = get_optional_boolean( "discover_changed_paths", false, kws );
	bool strict_node_history = get_optional_boolean( "strict_node_history", true, kws );

	SvnPool pool( m_context );
	std::list<LogEntryInfo> all_entries;

	apr_array_header_t *targets = targetsFromStringOrList( args[0], pool );

	try
		{
		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_log
			(
			targets,
			&revision_start,
			&revision_end,
			discover_changed_paths,
			strict_node_history,
			logReceiver,
			&all_entries,
			m_context,
			pool
			);

		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}


	apr_time_t now = apr_time_now();

	// convert the entries into python objects
	Py::List entries_list;
	std::list<LogEntryInfo>::const_iterator entry_it = all_entries.begin();
	while( entry_it != all_entries.end() )
		{
		const LogEntryInfo &entry = *entry_it;
		++entry_it;

		Py::Dict entry_dict;
		entry_dict["author"] = Py::String( entry.m_author, g_utf_8 );
		entry_dict["date"] = toObject( convertStringToTime( entry.m_date, now, pool ) );
		entry_dict["message"] = Py::String( entry.m_message, g_utf_8 );
		entry_dict["revision"] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, entry.m_revision ) );

		Py::List changed_paths_list;
		std::list<LogChangePathInfo>::const_iterator changed_paths_it = entry.m_changed_paths.begin();
		while( changed_paths_it != entry.m_changed_paths.end() )
			{
			const LogChangePathInfo &change_entry = *changed_paths_it;
			++changed_paths_it;

			Py::Dict changed_entry_dict;
			changed_entry_dict["path"] = Py::String( change_entry.m_path, g_utf_8 );
			changed_entry_dict["action"] = Py::String( &change_entry.m_action, 1 );
			changed_entry_dict["copyfrom_path"] = utf8_string_or_none( change_entry.m_copy_from_path );

			if( SVN_IS_VALID_REVNUM( change_entry.m_copy_from_revision ) )
				changed_entry_dict["copyfrom_revision"] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, change_entry.m_copy_from_revision ) );
			else
				changed_entry_dict["copyfrom_revision"] = Py::Nothing();

			changed_paths_list.append( changed_entry_dict );
			}

		entry_dict["changed_paths"] = changed_paths_list;

		entries_list.append( entry_dict );
		}

	return entries_list;
	}

static int compare_items_as_paths (const svn_sort__item_t *a, const svn_sort__item_t *b)
	{
	return svn_path_compare_paths ((const char *)a->key, (const char *)b->key);
	}


Py::Object pysvn_client::cmd_ls(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, revision_recurse_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	bool recurse = get_optional_boolean( "recurse", false, kws );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );

	SvnPool pool( m_context );
	apr_hash_t *hash = NULL;
	std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

	try
		{
		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_ls
			(
			&hash,
			norm_path.c_str(),
			&revision,
			recurse,
			m_context,
			pool
			);

		if( error != 0 )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}


	apr_array_header_t *array = svn_sort__hash( hash, compare_items_as_paths, pool );

	std::string base_path;
	if( !norm_path.empty() )
		{
		base_path = norm_path;
		base_path += '/';
		}

	// convert the entries into python objects
	Py::List entries_list;

	for( int i = 0; i < array->nelts; ++i )
		{
		svn_sort__item_t *item = &APR_ARRAY_IDX( array, i, svn_sort__item_t );

		const char *utf8_entryname = static_cast<const char *>( item->key );
		svn_dirent_t *dirent = static_cast<svn_dirent_t *>( apr_hash_get( hash, utf8_entryname, item->klen ) );


		std::string full_name( base_path );
		full_name += utf8_entryname;

		Py::Dict entry_dict;
		entry_dict["name"] = Py::String( full_name, g_utf_8 );
		entry_dict["kind"] = toEnumValue( dirent->kind );
		entry_dict["size"] = Py::Long( Py::Float( double( static_cast<signed_int64>( dirent->size ) ) ) );
		entry_dict["created_rev"] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, dirent->created_rev ) );
		entry_dict["time"] = toObject( dirent->time );
		entry_dict["last_author"] = utf8_string_or_none( dirent->last_author );

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

	Py::String path1( asUtf8String( args[0] ) );
	svn_opt_revision_t revision1;
	get_revision( revision1, args[1] );
	Py::String path2( asUtf8String( args[2] ) );
	svn_opt_revision_t revision2;
	get_revision( revision2, args[3] );
	Py::String local_path( asUtf8String( args[4] ) );

	bool force = get_optional_boolean( "force", false, kws );
	bool recurse = get_optional_boolean( "recurse", true, kws );
	bool notice_ancestry = !get_optional_boolean( "notice_ancestry", false, kws );
	bool dry_run = get_optional_boolean( "dry_run", true, kws );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path1( svnNormalisedIfPath( path1.as_std_string(), pool ) );
		std::string norm_path2( svnNormalisedIfPath( path2.as_std_string(), pool ) );
		std::string norm_local_path( svnNormalisedIfPath( local_path.as_std_string(), pool ) );

		svn_error_t *error = svn_client_merge
			(
			norm_path1.c_str (),
			&revision1,
			norm_path2.c_str (),
			&revision2,
			norm_local_path.c_str (),
			recurse,
			!notice_ancestry,
			force,
			dry_run,
			m_context,
			pool
			);
		if( error != 0 )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
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

	SvnPool pool( m_context );

	apr_array_header_t *targets = targetsFromStringOrList( args[0], pool );

	try
		{
		type_error_message = "expecting string message (arg 2)";
		Py::String py_message( asUtf8String( args[1] ) );
		message = py_message.as_std_string();

		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
		}

	svn_client_commit_info_t *commit_info = NULL;

	try
		{
		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		m_context.setLogMessage( message.c_str() );

		svn_error_t *error = svn_client_mkdir
			(
			&commit_info,
			targets,
			m_context,
			pool
			);

		if( error != 0 )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return toObject( commit_info );
	}

Py::Object pysvn_client::cmd_move(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, src_revision_force_keywords, kws );

	SvnPool pool( m_context );
	svn_client_commit_info_t *commit_info = NULL;

	std::string type_error_message;
	try
		{
		type_error_message = "expecting string for src_path (arg 1)";
		Py::String src_path( asUtf8String( args[0] ) );

		type_error_message = "expecting string for dest_path (arg 2)";
		Py::String dest_path( asUtf8String( args[1] ) );

		svn_opt_revision_t revision;
		type_error_message = "expecting revision for keyword src_revision";
		get_optional_revision( revision, "src_revision", svn_opt_revision_head, kws );

		type_error_message = "expecting boolean for keyword force";
		bool force = get_optional_boolean( "force", false, kws );

		try
			{
			std::string norm_src_path( svnNormalisedIfPath( src_path.as_std_string(), pool ) );
			std::string norm_dest_path( svnNormalisedIfPath( dest_path.as_std_string(), pool ) );

			checkThreadPermission();

			PythonAllowThreads permission( m_context );

			svn_error_t *error = svn_client_copy
				(
				&commit_info,
				norm_src_path.c_str(),
				&revision,
				norm_dest_path.c_str(),
				m_context,
				pool
				);

			if( error != NULL )
				throw SvnException( error );
			}
		catch( SvnException &e )
			{
			throw Py::Exception( m_module.client_error, e.message() );
			}
		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
		}

	return toObject( commit_info );
	}

Py::Object pysvn_client::cmd_propdel( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_recurse_keywords, kws );

	Py::String propname( asUtf8String( args[0] ) );
	Py::String path( asUtf8String( args[1] ) );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	bool recurse = get_optional_boolean( "recurse", false, kws );

	SvnPool pool( m_context );
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t *error = svn_client_propset
			(
			propname.as_std_string().c_str(),
			NULL, // value = NULL
			norm_path.c_str (),
			recurse,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_propget( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_recurse_keywords, kws );

	Py::String propname( asUtf8String( args[0] ) );
	Py::String path( args[1] );

	bool recurse = get_optional_boolean( "recurse", false, kws );
	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	SvnPool pool( m_context );
	apr_hash_t *props = NULL;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t *error = svn_client_propget
			(
			&props,
			propname.as_std_string().c_str(),
			norm_path.c_str(),
			&revision,
			recurse,
			m_context,
			pool
			);
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return propsToObject( props, pool );
	}

Py::Object pysvn_client::cmd_proplist( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 0, 1, args, revision_recurse_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	bool recurse = get_optional_boolean( "recurse", false, kws );
	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	SvnPool pool( m_context );
	apr_array_header_t *props = NULL;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t *error = svn_client_proplist
			(
			&props,
			norm_path.c_str(),
			&revision,
			recurse,
			m_context,
			pool
			);
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return proplistToObject( props, pool );
	}

Py::Object pysvn_client::cmd_propset( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 3, 3, args, revision_recurse_keywords, kws );

	Py::String propname( asUtf8String( args[0] ) );
	Py::String py_propval( asUtf8String( args[1] ) );
	Py::String path( asUtf8String( args[2] ) );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	bool recurse = get_optional_boolean( "recurse", false, kws );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		const svn_string_t *propval = svn_string_create( py_propval.as_std_string().c_str(), pool );

		svn_error_t *error = svn_client_propset
			(
			propname.as_std_string().c_str(),
			propval,
			norm_path.c_str(),
			recurse,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_revpropdel( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_force_keywords, kws );

	Py::String propname( asUtf8String( args[0] ) );
	Py::String path( asUtf8String( args[1] ) );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	bool force = get_optional_boolean( "force", false, kws );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_revprop_set
			(
			propname.as_std_string().c_str(),
			NULL,			// value = NULL
			norm_path.c_str(),
			&revision,
			&revnum,
			force,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_revpropget( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_only_keywords, kws );

	Py::String propname( asUtf8String( args[0] ) );
	Py::String path( asUtf8String( args[1] ) );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	SvnPool pool( m_context );

	svn_string_t *propval = NULL;
	svn_revnum_t revnum = 0;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t * error = svn_client_revprop_get
			(
			propname.as_std_string().c_str(),
			&propval,
			norm_path.c_str(),
			&revision,
			&revnum,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	Py::Tuple result(2);
	result[0] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	result[1] = Py::String( propval->data, propval->len, g_utf_8 );

	return result;
	}

Py::Object pysvn_client::cmd_revproplist( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, revision_only_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	SvnPool pool( m_context );

	apr_hash_t *props = NULL;
	svn_revnum_t revnum = 0;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_revprop_list
			(
			&props,
			norm_path.c_str(),
			&revision,
			&revnum,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}


	Py::Tuple result(2);
	result[0] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	result[1] = propsToObject( props, pool );

	return result;
	}

Py::Object pysvn_client::cmd_revpropset( const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 3, 3, args, revision_force_keywords, kws );

	Py::String propname( asUtf8String( args[0] ) );
	Py::String py_propval( asUtf8String( args[1] ) );
	Py::String path( asUtf8String( args[2] ) );

	svn_opt_revision_t revision;
	if( is_svn_url( path.as_std_string() ) )
		get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	else
		get_optional_revision( revision, "revision", svn_opt_revision_working, kws );

	bool force = get_optional_boolean( "force", false, kws );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		const svn_string_t *propval = svn_string_create( py_propval.as_std_string().c_str(), pool );
		svn_error_t *error = svn_client_revprop_set
			(
			propname.as_std_string().c_str(),
			propval,
			norm_path.c_str(),
			&revision,
			&revnum,
			force,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_relocate(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 3, 3, args, recurse_only_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	Py::String from_url( asUtf8String( args[1] ) );
	Py::String to_url( asUtf8String( args[2] ) );

	bool recurse = get_optional_boolean( "recurse", true, kws );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t * error = svn_client_relocate
			(
			norm_path.c_str(),
			from_url.as_std_string().c_str(),
			to_url.as_std_string().c_str(),
			recurse,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_remove(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, force_only_keywords, kws );

	bool force = get_optional_boolean( "force", false, kws );

	SvnPool pool( m_context );
	apr_array_header_t *targets = targetsFromStringOrList( args[0], pool );

	svn_client_commit_info_t *commit_info = NULL;
	try
		{
		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_delete
			(
			&commit_info,
			targets,
			force,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return toObject( commit_info );
	}


Py::Object pysvn_client::cmd_resolved(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, recurse_only_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	bool recurse = get_optional_boolean( "recurse", false, kws );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t *error = svn_client_resolved
			(
			norm_path.c_str(),
			recurse,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::cmd_revert(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, recurse_only_keywords, kws );

	std::string type_error_message;

	SvnPool pool( m_context );
	apr_array_header_t *targets = targetsFromStringOrList( args[0], pool );

	try
		{
		bool recurse = get_optional_boolean( "recurse", false, kws );


		try
			{
			checkThreadPermission();

			PythonAllowThreads permission( m_context );

			svn_error_t *error = svn_client_revert
				(
				targets,
				recurse,
				m_context,
				pool
				);
			if( error != NULL )
				throw SvnException( error );
			}
		catch( SvnException &e )
			{
			throw Py::Exception( m_module.client_error, e.message() );
			}
		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
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

struct StatusEntriesBaton
	{
	apr_pool_t* pool;
	apr_hash_t* hash;
	};

static void StatusEntriesFunc
	(
	void *baton,
	const char *path,
	svn_wc_status_t *status
	)
	{
	svn_wc_status_t* stat;
	StatusEntriesBaton* seb = (StatusEntriesBaton*)baton;

	path = apr_pstrdup( seb->pool, path );
	stat = svn_wc_dup_status( status, seb->pool );
	apr_hash_set( seb->hash, path, APR_HASH_KEY_STRING, stat );
	}

Py::Object pysvn_client::cmd_status(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, status_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	bool recurse = get_optional_boolean( "recurse", true, kws );
	bool get_all = get_optional_boolean( "get_all", true, kws );
	bool update = get_optional_boolean( "update", false, kws );
	bool no_ignore = get_optional_boolean( "no_ignore", false, kws );

	SvnPool pool( m_context );
	apr_hash_t *status_hash = NULL;

	Py::List entries_list;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_revnum_t revnum;
		svn_opt_revision_t rev = { svn_opt_revision_head, 0 };

		StatusEntriesBaton baton;

		status_hash = apr_hash_make (pool);
		baton.hash = status_hash;
		baton.pool = pool;

		svn_error_t *error = svn_client_status
			(
			&revnum,		    // revnum
			norm_path.c_str(),	    // path
			&rev,
			StatusEntriesFunc,	    // status func
			&baton,		    // status baton
			recurse,
			get_all,
			update,
			no_ignore,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	apr_array_header_t *statusarray = svn_sort__hash( status_hash, svn_sort_compare_items_as_paths, pool);

	// Loop over array, printing each name/status-structure
	for (int i = statusarray->nelts-1; i >= 0; i--)
		{
		const svn_sort__item_t *item = &APR_ARRAY_IDX( statusarray, i, const svn_sort__item_t );
		svn_wc_status_t *status = (svn_wc_status_t *)item->value;

		entries_list.append( Py::asObject( new pysvn_status( (const char *)item->key, status, m_context ) ) );
		}

	return entries_list;
	}

Py::Object pysvn_client::cmd_switch(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 2, 2, args, revision_recurse_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	Py::String url( asUtf8String( args[1] ) );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	bool recurse = get_optional_boolean( "recurse", true, kws );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_switch
			(
			&revnum,
			norm_path.c_str(),
			url.as_std_string().c_str(),
			&revision,
			recurse,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::cmd_update(const Py::Tuple& args, const Py::Dict &kws )
	{
	check_arguments( 1, 1, args, revision_recurse_keywords, kws );

	Py::String path( asUtf8String( args[0] ) );
	svn_opt_revision_t revision;
	get_optional_revision( revision, "revision", svn_opt_revision_head, kws );
	bool recurse = get_optional_boolean( "recurse", true, kws );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path.as_std_string(), pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_update
			(
			&revnum,
			norm_path.c_str (),
			&revision,
			recurse,
			m_context,
			pool
			);
		if( error != NULL )
			throw SvnException( error );
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
	}

Py::Object pysvn_client::is_url( const Py::Tuple& args )
	{
	check_arguments( 1, 1, args );

	Py::String path( asUtf8String( args[0] ) );

	Py::Int result( is_svn_url( path.as_std_string() ) );
	return result;
	}

Py::Object pysvn_client::set_auth_cache( const Py::Tuple& args )
	{
	check_arguments( 1, 1, args );

	Py::Int enable( args[0] );
	try
		{
		void *param = 0;
		if( long( enable ) == 0 )
			param = (void *)"1";

		svn_auth_set_parameter
			(
			m_context.ctx()->auth_baton,
			SVN_AUTH_PARAM_NO_AUTH_CACHE,
			param
			);
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::set_auto_props( const Py::Tuple& args )
	{
	check_arguments( 1, 1, args );

	Py::Int enable( args[0] );
	try
		{
                svn_config_t *cfg = (svn_config_t *)apr_hash_get
			(
			m_context.ctx()->config,
			SVN_CONFIG_CATEGORY_CONFIG,
			APR_HASH_KEY_STRING
			);
		svn_config_set_bool
			(
			cfg,
			SVN_CONFIG_SECTION_MISCELLANY,
			SVN_CONFIG_OPTION_ENABLE_AUTO_PROPS,
			long( enable ) != 0
			);
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

	// check that we are not in use on another thread
void pysvn_client::checkThreadPermission()
	{
	if( m_context.hasPermission() )
		{
		throw Py::Exception( m_module.client_error,
			"client in use on another thread" );
		}
	}

void pysvn_client::init_type()
	{
	behaviors().name("Client");
	behaviors().doc( class_client_doc );
	behaviors().supportGetattr();
	behaviors().supportSetattr();

	add_keyword_method("add", &pysvn_client::cmd_add, SVN_ADD_DOC );
	add_keyword_method("annotate", &pysvn_client::cmd_annotate, SVN_ANNOTATE_DOC );
	add_keyword_method("cat", &pysvn_client::cmd_cat, SVN_CAT_DOC );
	add_keyword_method("checkin", &pysvn_client::cmd_checkin, SVN_CHECKIN_DOC );
	add_keyword_method("checkout", &pysvn_client::cmd_checkout, SVN_CHECKOUT_DOC );
	add_keyword_method("copy", &pysvn_client::cmd_copy, SVN_COPY_DOC );
	add_keyword_method("diff", &pysvn_client::cmd_diff, SVN_DIFF_DOC );
	add_keyword_method("export", &pysvn_client::cmd_export, SVN_EXPORT_DOC );
	add_keyword_method("import_", &pysvn_client::cmd_import, SVN_IMPORT_DOC );
	add_keyword_method("info", &pysvn_client::cmd_info, SVN_INFO_DOC );
	add_keyword_method("log", &pysvn_client::cmd_log, SVN_LOG_DOC );
	add_keyword_method("ls", &pysvn_client::cmd_ls, SVN_LS_DOC );
	add_keyword_method("merge", &pysvn_client::cmd_merge, SVN_MERGE_DOC );
	add_keyword_method("move", &pysvn_client::cmd_move, SVN_MOVE_DOC );
	add_keyword_method("propdel", &pysvn_client::cmd_propdel, SVN_PROPDEL_DOC );
	add_keyword_method("propget", &pysvn_client::cmd_propget, SVN_PROPGET_DOC );
	add_keyword_method("proplist", &pysvn_client::cmd_proplist, SVN_PROPLIST_DOC );
	add_keyword_method("propset", &pysvn_client::cmd_propset, SVN_PROPSET_DOC );
	add_keyword_method("relocate", &pysvn_client::cmd_relocate, SVN_RELOCATE_DOC );
	add_keyword_method("remove", &pysvn_client::cmd_remove, SVN_REMOVE_DOC );
	add_keyword_method("resolved", &pysvn_client::cmd_resolved, SVN_RESOLVED_DOC );
	add_keyword_method("revert", &pysvn_client::cmd_revert, SVN_REVERT_DOC );
	add_keyword_method("revpropdel", &pysvn_client::cmd_revpropdel, SVN_REVPROPDEL_DOC );
	add_keyword_method("revpropget", &pysvn_client::cmd_revpropget, SVN_REVPROPGET_DOC );
	add_keyword_method("revproplist", &pysvn_client::cmd_revproplist, SVN_REVPROPLIST_DOC );
	add_keyword_method("revpropset", &pysvn_client::cmd_revpropset, SVN_REVPROPSET_DOC );
	add_keyword_method("status", &pysvn_client::cmd_status, SVN_STATUS_DOC );
	add_keyword_method("switch", &pysvn_client::cmd_switch, SVN_SWITCH_DOC );
	add_keyword_method("update", &pysvn_client::cmd_update, SVN_UPDATE_DOC );
	add_varargs_method("cleanup", &pysvn_client::cmd_cleanup, SVN_CLEANUP_DOC );
	add_varargs_method("mkdir", &pysvn_client::cmd_mkdir, SVN_MKDIR_DOC );
	add_varargs_method("set_auth_cache", &pysvn_client::set_auth_cache, SET_AUTH_CACHE_DOC );
	add_varargs_method("set_auto_props", &pysvn_client::set_auto_props, SET_AUTO_PROPS_DOC );
	add_varargs_method("is_url", &pysvn_client::is_url, IS_URL_DOC );
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
