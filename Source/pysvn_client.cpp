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

static const char empty_string[] = "";
static const char name___members__[] = "__members__";
static const char name_action[] = "action";
static const char name_author[] = "author";
static const char name_callback_cancel[] = "callback_cancel";
static const char name_callback_get_log_message[] = "callback_get_log_message";
static const char name_callback_get_login[] = "callback_get_login";
static const char name_callback_notify[] = "callback_notify";
static const char name_callback_ssl_client_cert_password_prompt[] = "callback_ssl_client_cert_password_prompt";
static const char name_callback_ssl_client_cert_prompt[] = "callback_ssl_client_cert_prompt";
static const char name_callback_ssl_server_prompt[] = "callback_ssl_server_prompt";
static const char name_callback_ssl_server_trust_prompt[] = "callback_ssl_server_trust_prompt";
static const char name_changed_paths[] = "changed_paths";
static const char name_copyfrom_path[] = "copyfrom_path";
static const char name_copyfrom_revision[] = "copyfrom_revision";
static const char name_created_rev[] = "created_rev";
static const char name_date[] = "date";
static const char name_dest_path[] = "dest_path";
static const char name_dest_url_or_path[] = "dest_url_or_path";
static const char name_diff_deleted[] = "diff_deleted";
static const char name_discover_changed_paths[] = "discover_changed_paths";
static const char name_dry_run[] = "dry_run";
static const char name_enable[] = "enable";
static const char name_force[] = "force";
static const char name_from_url[] = "from_url";
static const char name_get_all[] = "get_all";
static const char name_ignore[] = "ignore";
static const char name_ignore_ancestry[] = "ignore_ancestry";
static const char name_kind[] = "kind";
static const char name_last_author[] = "last_author";
static const char name_line[] = "line";
static const char name_local_path[] = "local_path";
static const char name_log_message[] = "log_message";
static const char name_message[] = "message";
static const char name_name[] = "name";
static const char name_notice_ancestry[] = "notice_ancestry";
static const char name_number[] = "number";
static const char name_path[] = "path";
static const char name_prop_name[] = "prop_name";
static const char name_prop_value[] = "prop_value";
static const char name_recurse[] = "recurse";
static const char name_revision[] = "revision";
static const char name_revision_end[] = "revision_end";
static const char name_revision_start[] = "revision_start";
static const char name_revision1[] = "revision1";
static const char name_revision2[] = "revision2";
static const char name_size[] = "size";
static const char name_src_revision[] = "src_revision";
static const char name_src_url_or_path[] = "src_url_or_path";
static const char name_strict_node_history[] = "strict_node_history";
static const char name_time[] = "time";
static const char name_tmp_path[] = "tmp_path";
static const char name_to_url[] = "to_url";
static const char name_update[] = "update";
static const char name_url[] = "url";
static const char name_url_or_path[] = "url_or_path";
static const char name_url_or_path1[] = "url_or_path1";
static const char name_url_or_path2[] = "url_or_path2";
static const char name_utf8[] = "UTF-8";


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
	
	if( name == name___members__ )
		{
		Py::List members;

		members.append( Py::String( name_callback_get_login ) );
		members.append( Py::String( name_callback_notify ) );
		members.append( Py::String( name_callback_cancel ) );
		members.append( Py::String( name_callback_get_log_message ) );
		members.append( Py::String( name_callback_ssl_server_prompt ) );
		members.append( Py::String( name_callback_ssl_server_trust_prompt ) );
		members.append( Py::String( name_callback_ssl_client_cert_prompt ) );
		members.append( Py::String( name_callback_ssl_client_cert_password_prompt ) );

		return members;
		}

	if( name == name_callback_get_login )
		return m_context.m_pyfn_GetLogin;
	if( name == name_callback_notify )
		return m_context.m_pyfn_Notify;
	if( name == name_callback_cancel )
		return m_context.m_pyfn_Cancel;
	if( name == name_callback_get_log_message )
		return m_context.m_pyfn_GetLogMessage;
	if( name == name_callback_ssl_server_prompt )
		return m_context.m_pyfn_SslServerPrompt;
	if( name == name_callback_ssl_server_trust_prompt )
		return m_context.m_pyfn_SslServerTrustPrompt;
	if( name == name_callback_ssl_client_cert_prompt )
		return m_context.m_pyfn_SslClientCertPrompt;
	if( name == name_callback_ssl_client_cert_password_prompt )
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
	if( name == name_callback_get_login )
		set_callable( m_context.m_pyfn_GetLogin, value );
	else if( name == name_callback_notify )
		set_callable( m_context.m_pyfn_Notify, value );
	else if( name == name_callback_cancel )
		set_callable( m_context.m_pyfn_Cancel, value );
	else if( name == name_callback_get_log_message )
		set_callable( m_context.m_pyfn_GetLogMessage, value );
	else if( name == name_callback_ssl_server_prompt )
		set_callable( m_context.m_pyfn_SslServerPrompt, value );
	else if( name == name_callback_ssl_server_trust_prompt )
		set_callable( m_context.m_pyfn_SslServerTrustPrompt, value );
	else if( name == name_callback_ssl_client_cert_prompt )
		set_callable( m_context.m_pyfn_SslClientCertPrompt, value );
	else if( name == name_callback_ssl_client_cert_password_prompt )
		set_callable( m_context.m_pyfn_SslClientCertPwPrompt, value );
	else
		{
		std::string msg( "Unknown attribute: " );
		msg += name;
		throw Py::AttributeError( msg );
		}
	return 0;
	}


Py::Object pysvn_client::cmd_add( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "add", args_desc, a_args, a_kws );
	args.check();

	Py::List path_list( toListOfStrings( args.getArg( name_path ) ) );

	bool recurse = args.getBoolean( name_recurse, true );

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


Py::Object pysvn_client::cmd_annotate( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url_or_path },
		{ false, name_revision_start },
		{ false, name_revision_end },
		{ false, NULL }
		};
	FunctionArguments args( "annotate", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_url_or_path, empty_string ) );
	svn_opt_revision_t revision_start = args.getRevision( name_revision_start, svn_opt_revision_number );
	svn_opt_revision_t revision_end = args.getRevision( name_revision_end, svn_opt_revision_head );

	SvnPool pool( m_context );
	std::list<AnnotatedLineInfo> all_entries;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

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
		entry_dict[name_author] = Py::String( entry.m_author, name_utf8 );
		entry_dict[name_date] = Py::String( entry.m_date );
		entry_dict[name_line] = Py::String( entry.m_line, name_utf8 );
		entry_dict[name_number] = Py::Int( long( entry.m_line_no ) );
		entry_dict[name_revision] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, entry.m_revision ) );

		entries_list.append( entry_dict );
		}

	return entries_list;
	}

Py::Object pysvn_client::cmd_cat( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url_or_path },
		{ false, name_revision },
		{ false, NULL }
		};
	FunctionArguments args( "cat", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_url_or_path ) );
	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );

	SvnPool pool( m_context );

	svn_stringbuf_t * stringbuf = svn_stringbuf_create( empty_string, pool );
	svn_stream_t * stream = svn_stream_from_stringbuf( stringbuf, pool );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t *error = svn_client_cat
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

	return Py::String( stringbuf->data, stringbuf->len, name_utf8 );
	}

Py::Object pysvn_client::cmd_checkout( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url },
		{ true,  name_path },
		{ false, name_recurse },
		{ false, name_revision },
		{ false, NULL }
		};
	FunctionArguments args( "checkout", args_desc, a_args, a_kws );
	args.check();

	std::string url( args.getUtf8String( name_url ) );
	std::string path( args.getUtf8String( name_path ) );
	bool recurse = args.getBoolean( name_recurse, true );
	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_revnum_t revnum = 0;
		svn_error_t * error = svn_client_checkout
			(
			&revnum,
			url.c_str(),
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

Py::Object pysvn_client::cmd_cleanup( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ false, NULL }
		};
	FunctionArguments args( "cleanup", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_path ) );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

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

Py::Object pysvn_client::cmd_checkin( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ true,  name_log_message },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "checkin", args_desc, a_args, a_kws );
	args.check();

	SvnPool pool( m_context );
	svn_client_commit_info_t *commit_info = NULL;

	apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_path ), pool );

	std::string type_error_message;
	try
		{
		type_error_message = "expecting string for message (arg 2)";

		std::string message( args.getUtf8String( name_log_message ) );

		type_error_message = "expecting boolean for recurse keyword arg";
		bool recurse = args.getBoolean( name_recurse, true );


		try
			{
			checkThreadPermission();

			PythonAllowThreads permission( m_context );

			m_context.setLogMessage( message );

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

Py::Object pysvn_client::cmd_copy( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_src_url_or_path },
		{ true,  name_dest_url_or_path },
		{ false, name_src_revision },
		{ false, NULL }
		};
	FunctionArguments args( "copy", args_desc, a_args, a_kws );
	args.check();

	SvnPool pool( m_context );
	svn_client_commit_info_t *commit_info = NULL;

	std::string type_error_message;
	try
		{
		type_error_message = "expecting string for src_path (arg 1)";
		Py::String src_path( args.getUtf8String( name_src_url_or_path ) );

		type_error_message = "expecting string for dest_path (arg 2)";
		Py::String dest_path( args.getUtf8String( name_dest_url_or_path ) );

		type_error_message = "expecting revision for keyword src_revision";
		svn_opt_revision_t revision = args.getRevision( name_src_revision, svn_opt_revision_head );

		type_error_message = "expecting boolean for keyword force";
		bool force = args.getBoolean( name_force, false );

		try
			{
			std::string norm_src_path( svnNormalisedIfPath( src_path, pool ) );
			std::string norm_dest_path( svnNormalisedIfPath( dest_path, pool ) );

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

// TODO: add path2 to allow diff of any to any
Py::Object pysvn_client::cmd_diff( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_tmp_path },
		{ true,  name_url_or_path },
		{ false, name_recurse },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, name_revision1 },
		{ false, name_revision2 },
		{ false, name_ignore_ancestry },
		{ false, name_diff_deleted },
		{ false, NULL }
		};
	FunctionArguments args( "diff", args_desc, a_args, a_kws );
	args.check();

	std::string tmp_path( args.getUtf8String( name_tmp_path ) );
	std::string path( args.getUtf8String( name_url_or_path ) );
	svn_opt_revision_t revision1 = args.getRevision( name_revision1, svn_opt_revision_base );
	svn_opt_revision_t revision2 = args.getRevision( name_revision2, svn_opt_revision_working );
	bool recurse = args.getBoolean( name_recurse, true );
	bool ignore_ancestry = args.getBoolean( name_ignore_ancestry, false );
	bool diff_deleted = args.getBoolean( name_diff_deleted, true );

	SvnPool pool( m_context );

	svn_stringbuf_t *stringbuf = NULL;

	try
		{
		std::string norm_tmp_path( svnNormalisedIfPath( tmp_path, pool ) );
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

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
			diff_deleted,
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

	return Py::String( stringbuf->data, stringbuf->len, name_utf8 );
	}

Py::Object pysvn_client::cmd_export( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_src_url_or_path },
		{ true,  name_dest_path },
		{ false, name_force },
		{ false, name_revision },
		{ false, NULL }
		};
	FunctionArguments args( "export", args_desc, a_args, a_kws );
	args.check();

	std::string src_path( args.getUtf8String( name_src_url_or_path ) );
	std::string dest_path( args.getUtf8String( name_dest_path ) );
	bool is_url = is_svn_url( src_path );

	bool force = args.getBoolean( name_force, false );
	svn_opt_revision_t revision;
	if( is_url )
		 revision = args.getRevision( name_revision, svn_opt_revision_head );
	else
		 revision = args.getRevision( name_revision, svn_opt_revision_working );

	svn_revnum_t revnum = 0;

	SvnPool pool( m_context );

	try
		{
		std::string norm_src_path( svnNormalisedIfPath( src_path, pool ) );
		std::string norm_dest_path( svnNormalisedIfPath( dest_path, pool ) );

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

Py::Object pysvn_client::cmd_info( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ false, NULL }
		};
	FunctionArguments args( "info", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_path ) );

	SvnPool pool( m_context );
	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

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

Py::Object pysvn_client::cmd_import( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ true,  name_url },
		{ true,  name_log_message },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "import_", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_path ) );
	std::string url( args.getUtf8String( name_url ) );
	std::string message( args.getUtf8String( name_log_message ) );

	bool recurse = args.getBoolean( name_recurse, true );

	SvnPool pool( m_context );
	svn_client_commit_info_t *commit_info = NULL;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );


		m_context.setLogMessage( message.c_str() );

		svn_error_t *error = svn_client_import
			(
			&commit_info,
			norm_path.c_str(),
			url.c_str(),
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

Py::Object pysvn_client::cmd_log( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url_or_path },
		{ false, name_revision_start },
		{ false, name_revision_end },
		{ false, name_discover_changed_paths },
		{ false, name_strict_node_history },
		{ false, NULL }
		};
	FunctionArguments args( "log", args_desc, a_args, a_kws );
	args.check();

	svn_opt_revision_t revision_start = args.getRevision( name_revision_start, svn_opt_revision_head );
	svn_opt_revision_t revision_end = args.getRevision( name_revision_end, svn_opt_revision_number );
	bool discover_changed_paths = args.getBoolean( name_discover_changed_paths, false );
	bool strict_node_history = args.getBoolean( name_strict_node_history, true );

	SvnPool pool( m_context );
	std::list<LogEntryInfo> all_entries;

	apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_url_or_path ), pool );

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
		entry_dict[name_author] = Py::String( entry.m_author, name_utf8 );
		entry_dict[name_date] = toObject( convertStringToTime( entry.m_date, now, pool ) );
		entry_dict[name_message] = Py::String( entry.m_message, name_utf8 );
		entry_dict[name_revision] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, entry.m_revision ) );

		Py::List changed_paths_list;
		std::list<LogChangePathInfo>::const_iterator changed_paths_it = entry.m_changed_paths.begin();
		while( changed_paths_it != entry.m_changed_paths.end() )
			{
			const LogChangePathInfo &change_entry = *changed_paths_it;
			++changed_paths_it;

			Py::Dict changed_entry_dict;
			changed_entry_dict[name_path] = Py::String( change_entry.m_path, name_utf8 );
			changed_entry_dict[name_action] = Py::String( &change_entry.m_action, 1 );
			changed_entry_dict[name_copyfrom_path] = utf8_string_or_none( change_entry.m_copy_from_path );

			if( SVN_IS_VALID_REVNUM( change_entry.m_copy_from_revision ) )
				changed_entry_dict[name_copyfrom_revision] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, change_entry.m_copy_from_revision ) );
			else
				changed_entry_dict[name_copyfrom_revision] = Py::Nothing();

			changed_paths_list.append( changed_entry_dict );
			}

		entry_dict[name_changed_paths] = changed_paths_list;

		entries_list.append( entry_dict );
		}

	return entries_list;
	}

static int compare_items_as_paths( const svn_sort__item_t *a, const svn_sort__item_t *b)
	{
	return svn_path_compare_paths ((const char *)a->key, (const char *)b->key);
	}


Py::Object pysvn_client::cmd_ls( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url_or_path },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "ls", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_url_or_path ) );
	bool recurse = args.getBoolean( name_recurse, false );
	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );

	SvnPool pool( m_context );
	apr_hash_t *hash = NULL;
	std::string norm_path( svnNormalisedIfPath( path, pool ) );

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
		entry_dict[name_name] = Py::String( full_name, name_utf8 );
		entry_dict[name_kind] = toEnumValue( dirent->kind );
		entry_dict[name_size] = Py::Long( Py::Float( double( static_cast<signed_int64>( dirent->size ) ) ) );
		entry_dict[name_created_rev] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, dirent->created_rev ) );
		entry_dict[name_time] = toObject( dirent->time );
		entry_dict[name_last_author] = utf8_string_or_none( dirent->last_author );

		entries_list.append( entry_dict );
		}

	return entries_list;
	}

Py::Object pysvn_client::cmd_merge( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url_or_path1 },
		{ true,  name_revision1 },
		{ true,  name_url_or_path2 },
		{ true,  name_revision2 },
		{ true,  name_local_path },
		{ false, name_force },
		{ false, name_recurse },
		{ false, name_notice_ancestry },
		{ false, name_dry_run },
		{ false, NULL }
		};
	FunctionArguments args( "merge", args_desc, a_args, a_kws );
	args.check();

	std::string path1( args.getUtf8String( name_url_or_path1 ) );
	svn_opt_revision_t revision1 = args.getRevision( name_revision1, svn_opt_revision_head );
	std::string path2( args.getUtf8String( name_url_or_path2 ) );
	svn_opt_revision_t revision2 = args.getRevision( name_revision2, svn_opt_revision_head );
	std::string local_path( args.getUtf8String( name_local_path ) );

	bool force = args.getBoolean( name_force, false );
	bool recurse = args.getBoolean( name_recurse, true );
	bool notice_ancestry = args.getBoolean( name_notice_ancestry, false );
	bool dry_run = args.getBoolean( name_dry_run, true );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path1( svnNormalisedIfPath( path1, pool ) );
		std::string norm_path2( svnNormalisedIfPath( path2, pool ) );
		std::string norm_local_path( svnNormalisedIfPath( local_path, pool ) );

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

Py::Object pysvn_client::cmd_mkdir( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url_or_path },
		{ false, name_log_message },
		{ false, NULL }
		};
	FunctionArguments args( "mkdir", args_desc, a_args, a_kws );
	args.check();

	// message that explains to the user the type error that may be thrown next
	std::string type_error_message;

	// args to the mkdir call
	std::string message;

	SvnPool pool( m_context );

	apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_url_or_path ), pool );

	try
		{
		type_error_message = "expecting string message (arg 2)";
		message = args.getUtf8String( name_log_message );

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

Py::Object pysvn_client::cmd_move( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_src_url_or_path },
		{ true,  name_dest_url_or_path },
		{ false, name_src_revision },
		{ false, name_force },
		{ false, NULL }
		};
	FunctionArguments args( "move", args_desc, a_args, a_kws );
	args.check();

	SvnPool pool( m_context );
	svn_client_commit_info_t *commit_info = NULL;

	std::string type_error_message;
	try
		{
		type_error_message = "expecting string for src_url_or_path (arg 1)";
		Py::String src_path( args.getUtf8String( name_src_url_or_path ) );

		type_error_message = "expecting string for dest_url_or_path (arg 2)";
		Py::String dest_path( args.getUtf8String( name_dest_url_or_path ) );

		type_error_message = "expecting revision for keyword src_revision";
		svn_opt_revision_t revision = args.getRevision( name_src_revision, svn_opt_revision_head );

		type_error_message = "expecting boolean for keyword force";
		bool force = args.getBoolean( name_force, false );

		try
			{
			std::string norm_src_path( svnNormalisedIfPath( src_path, pool ) );
			std::string norm_dest_path( svnNormalisedIfPath( dest_path, pool ) );

			checkThreadPermission();

			PythonAllowThreads permission( m_context );

			svn_error_t *error = svn_client_move
				(
				&commit_info,
				norm_src_path.c_str(),
				&revision,
				norm_dest_path.c_str(),
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
		}
	catch( Py::TypeError & )
		{
		throw Py::TypeError( type_error_message );
		}

	return toObject( commit_info );
	}

Py::Object pysvn_client::cmd_propdel( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_prop_name },
		{ true,  name_url_or_path },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "propdel", args_desc, a_args, a_kws );
	args.check();

	std::string propname( args.getUtf8String( name_prop_name ) );
	std::string path( args.getUtf8String( name_url_or_path ) );

	svn_opt_revision_t revision;
	if( is_svn_url( path ) )
		revision = args.getRevision( name_revision, svn_opt_revision_head );
	else
		revision = args.getRevision( name_revision, svn_opt_revision_working );

	bool recurse = args.getBoolean( name_recurse, false );

	SvnPool pool( m_context );
	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t *error = svn_client_propset
			(
			propname.c_str(),
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

Py::Object pysvn_client::cmd_propget( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_prop_name },
		{ true,  name_url_or_path },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "propget", args_desc, a_args, a_kws );
	args.check();

	std::string propname( args.getUtf8String( name_prop_name ) );
	std::string path( name_prop_name );

	bool recurse = args.getBoolean( name_recurse, false );
	svn_opt_revision_t revision;
	if( is_svn_url( path ) )
		revision = args.getRevision( name_revision, svn_opt_revision_head );
	else
		revision = args.getRevision( name_revision, svn_opt_revision_working );

	SvnPool pool( m_context );
	apr_hash_t *props = NULL;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t *error = svn_client_propget
			(
			&props,
			propname.c_str(),
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

extern int elapse_time();

Py::Object pysvn_client::cmd_proplist( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url_or_path },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	int t0 = elapse_time();
	FunctionArguments args( "proplist", args_desc, a_args, a_kws );
	args.check();

	Py::List path_list( toListOfStrings( args.getArg( name_url_or_path ) ) );

	bool recurse = args.getBoolean( name_recurse, false );
	svn_opt_revision_t revision;

//	int t1 = elapse_time();

	bool is_revision_setup = false;
	bool is_url = false;

	SvnPool pool( m_context );

	Py::List list_of_proplists;

	for( int i=0; i<path_list.length(); i++ )
		{
		Py::String path_str( asUtf8String( path_list[i] ) );
		std::string path( path_str.as_std_string() );
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		if( !is_revision_setup )
			if( is_svn_url( path ) )
				{
				revision = args.getRevision( name_revision, svn_opt_revision_head );
				is_url = true;
				}
			else
				{
				revision = args.getRevision( name_revision, svn_opt_revision_working );
				}
		else
			if( is_svn_url( path ) && !is_url )
				{
				throw Py::AttributeError( "cannot mix URL and PATH in name_path" );
				}

		apr_array_header_t *props = NULL;
		try
			{
			
			const char *norm_path_c_str = norm_path.c_str();
			checkThreadPermission();

			PythonAllowThreads permission( m_context );
			int t8 = elapse_time();
			svn_error_t *error = svn_client_proplist
				(
				&props,
				norm_path_c_str,
				&revision,
				recurse,
				m_context,
				pool
				);
			int t9 = elapse_time();
			if( error != NULL )
				std::cout << "Error in proplist" << std::endl;
			std::cout << "proplist prop list loop " << (t9-t8) << "ms" << std::endl;
			}
		catch( SvnException &e )
			{
			throw Py::Exception( m_module.client_error, e.message() );
			}

		proplistToObject( list_of_proplists, props, pool );
		}
	
	return list_of_proplists;
	}

Py::Object pysvn_client::cmd_propset( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_prop_name },
		{ true,  name_prop_value },
		{ true,  name_url_or_path },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "propset", args_desc, a_args, a_kws );
	args.check();

	std::string propname( args.getUtf8String( name_prop_name ) );
	std::string propval( args.getUtf8String( name_prop_value ) );
	std::string path( args.getUtf8String( name_url_or_path ) );

	svn_opt_revision_t revision;
	if( is_svn_url( path ) )
		revision = args.getRevision( name_revision, svn_opt_revision_head );
	else
		revision = args.getRevision( name_revision, svn_opt_revision_working );

	bool recurse = args.getBoolean( name_recurse, false );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		const svn_string_t *svn_propval = svn_string_create( propval.c_str(), pool );

		svn_error_t *error = svn_client_propset
			(
			propname.c_str(),
			svn_propval,
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

Py::Object pysvn_client::cmd_relocate( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_from_url },
		{ true,  name_to_url },
		{ true,  name_path },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "relocate", args_desc, a_args, a_kws );
	args.check();

	std::string from_url( args.getUtf8String( name_from_url ) );
	std::string to_url( args.getUtf8String( name_to_url ) );
	std::string path( args.getUtf8String( name_path ) );

	bool recurse = args.getBoolean( name_recurse, true );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );
		svn_error_t * error = svn_client_relocate
			(
			norm_path.c_str(),
			from_url.c_str(),
			to_url.c_str(),
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

Py::Object pysvn_client::cmd_remove( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url_or_path },
		{ false, name_force },
		{ false, NULL }
		};
	FunctionArguments args( "remove", args_desc, a_args, a_kws );
	args.check();

	bool force = args.getBoolean( name_force, false );

	SvnPool pool( m_context );
	apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_url_or_path ), pool );

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


Py::Object pysvn_client::cmd_resolved( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "resolved", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_path ) );
	bool recurse = args.getBoolean( name_recurse, false );

	SvnPool pool( m_context );

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

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

Py::Object pysvn_client::cmd_revert( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "revert", args_desc, a_args, a_kws );
	args.check();

	std::string type_error_message;

	SvnPool pool( m_context );
	apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_path ), pool );

	try
		{
		bool recurse = args.getBoolean( name_recurse, false );

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

Py::Object pysvn_client::cmd_revpropdel( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_prop_name },
		{ true,  name_url },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "revpropdel", args_desc, a_args, a_kws );
	args.check();

	std::string propname( args.getUtf8String( name_prop_name ) );
	std::string path( args.getUtf8String( name_url ) );

	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );

	bool force = args.getBoolean( name_force, false );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_revprop_set
			(
			propname.c_str(),
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

Py::Object pysvn_client::cmd_revpropget( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_prop_name },
		{ true,  name_url },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "revpropget", args_desc, a_args, a_kws );
	args.check();

	std::string propname( args.getUtf8String( name_prop_name ) );
	std::string path( args.getUtf8String( name_url ) );
	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );

	SvnPool pool( m_context );

	svn_string_t *propval = NULL;
	svn_revnum_t revnum = 0;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t * error = svn_client_revprop_get
			(
			propname.c_str(),
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
	result[1] = Py::String( propval->data, propval->len, name_utf8 );

	return result;
	}

Py::Object pysvn_client::cmd_revproplist( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "revproplist", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_url ) );
	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );

	SvnPool pool( m_context );

	apr_hash_t *props = NULL;
	svn_revnum_t revnum = 0;

	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

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

Py::Object pysvn_client::cmd_revpropset( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_prop_name },
		{ true,  name_prop_value },
		{ true,  name_url },
		{ false, name_revision },
		{ false, name_recurse },
		{ false, NULL }
		};
	FunctionArguments args( "revpropset", args_desc, a_args, a_kws );
	args.check();

	std::string propname( args.getUtf8String( name_prop_value ) );
	std::string propval( args.getUtf8String( name_prop_value ) );
	std::string path( args.getUtf8String( name_url ) );
	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );

	bool force = args.getBoolean( name_force, false );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		const svn_string_t *svn_propval = svn_string_create( propval.c_str(), pool );
		svn_error_t *error = svn_client_revprop_set
			(
			propname.c_str(),
			svn_propval,
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

Py::Object pysvn_client::cmd_status( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ false, name_recurse },
		{ false, name_get_all },
		{ false, name_update },
		{ false, name_ignore },
		{ false, NULL }
		};
	FunctionArguments args( "status", args_desc, a_args, a_kws );
	args.check();

	Py::String path( args.getUtf8String( name_path ) );
	bool recurse = args.getBoolean( name_recurse, true );
	bool get_all = args.getBoolean( name_get_all, true );
	bool update = args.getBoolean( name_update, false );
	bool no_ignore = args.getBoolean( name_ignore, true );

	SvnPool pool( m_context );
	apr_hash_t *status_hash = NULL;

	Py::List entries_list;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

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
			&baton,			    // status baton
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

Py::Object pysvn_client::cmd_switch( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ true,  name_url },
		{ false, name_recurse },
		{ false, name_revision },
		{ false, NULL }
		};
	FunctionArguments args( "switch", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_path ) );
	std::string url( args.getUtf8String( name_url ) );
	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );
	bool recurse = args.getBoolean( name_recurse, true );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

		checkThreadPermission();

		PythonAllowThreads permission( m_context );

		svn_error_t *error = svn_client_switch
			(
			&revnum,
			norm_path.c_str(),
			url.c_str(),
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

Py::Object pysvn_client::cmd_update( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_path },
		{ false, name_recurse },
		{ false, name_revision },
		{ false, NULL }
		};
	FunctionArguments args( "update", args_desc, a_args, a_kws );
	args.check();

	std::string path( args.getUtf8String( name_path ) );
	svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );
	bool recurse = args.getBoolean( name_recurse, true );

	SvnPool pool( m_context );

	svn_revnum_t revnum = 0;
	try
		{
		std::string norm_path( svnNormalisedIfPath( path, pool ) );

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

Py::Object pysvn_client::set_auth_cache( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_enable },
		{ false, NULL }
		};
	FunctionArguments args( "set_auth_cache", args_desc, a_args, a_kws );
	args.check();

	bool enable( args.getBoolean( name_enable ) );
	try
		{
		void *param = 0;
		if( enable )
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

Py::Object pysvn_client::set_auto_props( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_enable },
		{ false, NULL }
		};
	FunctionArguments args( "set_auto_props", args_desc, a_args, a_kws );
	args.check();

	bool enable( args.getBoolean( name_enable ) );
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
			enable
			);
		}
	catch( SvnException &e )
		{
		throw Py::Exception( m_module.client_error, e.message() );
		}

	return Py::Nothing();
	}

Py::Object pysvn_client::is_url( const Py::Tuple &a_args, const Py::Dict &a_kws )
	{
	static argument_description args_desc[] =
		{
		{ true,  name_url },
		{ false, NULL }
		};
	FunctionArguments args( "is_url", args_desc, a_args, a_kws );
	args.check();

	Py::String path( args.getUtf8String( name_url ) );

	Py::Int result( is_svn_url( path ) );
	return result;
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
	add_keyword_method("cleanup", &pysvn_client::cmd_cleanup, SVN_CLEANUP_DOC );
	add_keyword_method("mkdir", &pysvn_client::cmd_mkdir, SVN_MKDIR_DOC );
	add_keyword_method("set_auth_cache", &pysvn_client::set_auth_cache, SET_AUTH_CACHE_DOC );
	add_keyword_method("set_auto_props", &pysvn_client::set_auto_props, SET_AUTO_PROPS_DOC );
	add_keyword_method("is_url", &pysvn_client::is_url, IS_URL_DOC );
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
	behaviors().doc("wc_notify_action enumeration");
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
