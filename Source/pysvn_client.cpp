//
// ====================================================================
// (c) 2003-2006 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
//
//  pysvn_client.cpp
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
static const char name_callback_progress[] = "callback_progress";
static const char name_callback_ssl_client_cert_password_prompt[] = "callback_ssl_client_cert_password_prompt";
static const char name_callback_ssl_client_cert_prompt[] = "callback_ssl_client_cert_prompt";
static const char name_callback_ssl_server_prompt[] = "callback_ssl_server_prompt";
static const char name_callback_ssl_server_trust_prompt[] = "callback_ssl_server_trust_prompt";
static const char name_changed_paths[] = "changed_paths";
static const char name_comment[] = "comment";
static const char name_copyfrom_path[] = "copyfrom_path";
static const char name_copyfrom_revision[] = "copyfrom_revision";
static const char name_created_rev[] = "created_rev";
static const char name_date[] = "date";
static const char name_dest_path[] = "dest_path";
static const char name_dest_url_or_path[] = "dest_url_or_path";
static const char name_diff_deleted[] = "diff_deleted";
static const char name_diff_options[] = "diff_options";
static const char name_dirent_fields[] = "dirent_fields";
static const char name_discover_changed_paths[] = "discover_changed_paths";
static const char name_dry_run[] = "dry_run";
static const char name_enable[] = "enable";
static const char name_exception_style[] = "exception_style";
static const char name_fetch_locks[] = "fetch_locks";
static const char name_force[] = "force";
static const char name_from_url[] = "from_url";
static const char name_get_all[] = "get_all";
static const char name_has_props[] = "has_props";
static const char name_header_encoding[] = "header_encoding";
static const char name_ignore[] = "ignore";
static const char name_ignore_ancestry[] = "ignore_ancestry";
static const char name_ignore_content_type[] = "ignore_content_type";
static const char name_ignore_eol_style[] = "ignore_eol_style";
static const char name_ignore_externals[] = "ignore_externals";
static const char name_ignore_mime_type[] = "ignore_mime_type";
static const char name_ignore_space[] = "ignore_space";
static const char name_keep_locks[] = "keep_locks";
static const char name_kind[] = "kind";
static const char name_last_author[] = "last_author";
static const char name_limit[] = "limit";
static const char name_line[] = "line";
static const char name_local_path[] = "local_path";
static const char name_lock[] = "lock";
static const char name_log_message[] = "log_message";
static const char name_merge_options[] = "merge_options";
static const char name_message[] = "message";
static const char name_name[] = "name";
static const char name_native_eol[] = "native_eol";
static const char name_notice_ancestry[] = "notice_ancestry";
static const char name_number[] = "number";
static const char name_password[] = "password";
static const char name_path[] = "path";
static const char name_peg_revision[] = "peg_revision";
static const char name_prop_name[] = "prop_name";
static const char name_prop_value[] = "prop_value";
static const char name_recurse[] = "recurse";
static const char name_repos_path[] = "repos_path";
static const char name_revision1[] = "revision1";
static const char name_revision2[] = "revision2";
static const char name_revision[] = "revision";
static const char name_revision_end[] = "revision_end";
static const char name_revision_start[] = "revision_start";
static const char name_size[] = "size";
static const char name_skip_checks[] = "skip_checks";
static const char name_src_revision[] = "src_revision";
static const char name_src_url_or_path[] = "src_url_or_path";
static const char name_strict_node_history[] = "strict_node_history";
static const char name_time[] = "time";
static const char name_tmp_path[] = "tmp_path";
static const char name_to_url[] = "to_url";
static const char name_unlock[] = "unlock";
static const char name_update[] = "update";
static const char name_url[] = "url";
static const char name_url_or_path1[] = "url_or_path1";
static const char name_url_or_path2[] = "url_or_path2";
static const char name_url_or_path[] = "url_or_path";
static const char name_username[] = "username";
static const char name_utf8[] = "UTF-8";


static Py::String *py_name_name = NULL;
static Py::String *py_name_path = NULL;
static Py::String *py_name_repos_path = NULL;
static Py::String *py_name_kind;
static Py::String *py_name_size;
static Py::String *py_name_created_rev;
static Py::String *py_name_time;
static Py::String *py_name_has_props;
static Py::String *py_name_last_author;
static Py::String *py_name_callback_get_login;
static Py::String *py_name_callback_notify;
static Py::String *py_name_callback_cancel;
static Py::String *py_name_callback_get_log_message;
static Py::String *py_name_callback_ssl_server_prompt;
static Py::String *py_name_callback_ssl_server_trust_prompt;
static Py::String *py_name_callback_ssl_client_cert_prompt;
static Py::String *py_name_callback_ssl_client_cert_password_prompt;
static Py::String *py_name_exception_style;

static void init_py_names()
{
    static bool init_done = false;
    if( init_done )
    {
        return;
    }

    py_name_name = new Py::String( name_name );
    py_name_path = new Py::String( name_path );
    py_name_repos_path = new Py::String( name_repos_path );
    py_name_kind = new Py::String( name_kind );
    py_name_size = new Py::String( name_size );
    py_name_created_rev = new Py::String( name_created_rev );
    py_name_time = new Py::String( name_time );
    py_name_has_props = new Py::String( name_header_encoding );
    py_name_last_author = new Py::String( name_last_author );
    py_name_callback_get_login = new Py::String( name_callback_get_login );
    py_name_callback_notify = new Py::String( name_callback_notify );
    py_name_callback_cancel = new Py::String( name_callback_cancel );
    py_name_callback_get_log_message = new Py::String( name_callback_get_log_message );
    py_name_callback_ssl_server_prompt = new Py::String( name_callback_ssl_server_prompt );
    py_name_callback_ssl_server_trust_prompt = new Py::String( name_callback_ssl_server_trust_prompt );
    py_name_callback_ssl_client_cert_prompt = new Py::String( name_callback_ssl_client_cert_prompt );
    py_name_callback_ssl_client_cert_password_prompt = new Py::String( name_callback_ssl_client_cert_password_prompt );
    py_name_exception_style = new Py::String( name_exception_style );

    init_done = true;
}

//--------------------------------------------------------------------------------
std::string name_wrapper_status("PysvnStatus");
std::string name_wrapper_entry("PysvnEntry");
std::string name_wrapper_info("PysvnInfo");
std::string name_wrapper_lock("PysvnLock");
std::string name_wrapper_dirent("PysvnDirent");
std::string name_wrapper_wc_info("PysvnWcInfo");

pysvn_client::pysvn_client
    (
    pysvn_module &_module,
    const std::string &config_dir,
    Py::Dict result_wrappers
    )
: m_module( _module )
, m_result_wrappers( result_wrappers )
, m_context( config_dir )
, m_exception_style( 0 )
, m_wrapper_status( result_wrappers, name_wrapper_status )
, m_wrapper_entry( result_wrappers, name_wrapper_entry )
, m_wrapper_info( result_wrappers, name_wrapper_info )
, m_wrapper_lock( result_wrappers, name_wrapper_lock )
, m_wrapper_dirent( result_wrappers, name_wrapper_dirent )
, m_wrapper_wc_info( result_wrappers, name_wrapper_wc_info )
{
    init_py_names();
}

pysvn_client::~pysvn_client()
{
}

Py::Object pysvn_client::getattr( const char *_name )
{
    std::string name( _name );

    // std::cout << "getattr( " << name << " )" << std::endl << std::flush;
    
    if( name == name___members__ )
    {
        Py::List members;

        members.append( *py_name_callback_get_login );
        members.append( *py_name_callback_notify );
        members.append( *py_name_callback_cancel );
        members.append( *py_name_callback_get_log_message );
        members.append( *py_name_callback_ssl_server_prompt );
        members.append( *py_name_callback_ssl_server_trust_prompt );
        members.append( *py_name_callback_ssl_client_cert_prompt );
        members.append( *py_name_callback_ssl_client_cert_password_prompt );

        members.append( *py_name_exception_style );

        return members;
    }

    if( name == name_callback_get_login )
        return m_context.m_pyfn_GetLogin;
    if( name == name_callback_notify )
        return m_context.m_pyfn_Notify;
#ifdef PYSVN_HAS_CONTEXT_PROGRESS
    if( name == name_callback_progress )
        return m_context.m_pyfn_Progress;
#endif
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
    if( name == name_exception_style )
        return Py::Int( m_exception_style );
    return getattr_default( _name );
}

static void set_callable( Py::Object &callback, const Py::Object &value )
{
    if( value.is( Py::None() ) || value.isCallable() )
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
#ifdef PYSVN_HAS_CONTEXT_PROGRESS
    else if( name == name_callback_progress )
        set_callable( m_context.m_pyfn_Progress, value );
#endif
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
    else if( name == name_exception_style )
    {
        Py::Int style( value );
        if( style == 0 || style == 1 )
        {
            m_exception_style = style;
        }
            else
        {
            throw Py::AttributeError( "exception_style value must be 0 or 1" );
        }
    }
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
#ifdef PYSVN_HAS_CLIENT_ADD2
    { false, name_force },
#endif
#ifdef PYSVN_HAS_CLIENT_ADD3
    { false, name_ignore },
#endif
    { false, NULL }
    };
    FunctionArguments args( "add", args_desc, a_args, a_kws );
    args.check();

    Py::List path_list( toListOfStrings( args.getArg( name_path ) ) );

    bool recurse = args.getBoolean( name_recurse, true );
#ifdef PYSVN_HAS_CLIENT_ADD2
    bool force = args.getBoolean( name_force, false );
#endif
#ifdef PYSVN_HAS_CLIENT_ADD3
    bool ignore = args.getBoolean( name_ignore, true );
#endif

    SvnPool pool( m_context );
    try
    {
        for( Py::List::size_type i=0; i<path_list.length(); i++ )
        {
            Py::String path_str( asUtf8String( path_list[i] ) );
            std::string norm_path( svnNormalisedIfPath( path_str.as_std_string(), pool ) );

            checkThreadPermission();

            PythonAllowThreads permission( m_context );

            SvnPool pool( m_context );

#if defined( PYSVN_HAS_CLIENT_ADD3 )
            svn_error_t * error = svn_client_add3
                (
                norm_path.c_str(),
                recurse,
                force,
                !ignore,
                m_context,
                pool
                );
#elif defined( PYSVN_HAS_CLIENT_ADD2 )
            svn_error_t * error = svn_client_add2
                (
                norm_path.c_str(),
                recurse,
                force,
                m_context,
                pool
                );
#else
            svn_error_t * error = svn_client_add
                (
                norm_path.c_str(),
                recurse,
                m_context,
                pool
                );
#endif
            if( error != NULL )
                throw SvnException( error );
        }
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
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
    // There are cases when the author has been passed as NULL
    // protect against NULL passed for any of the strings
    if( author == NULL )
        author = "";
    if( date == NULL )
        date = "";
    if( line == NULL )
        line = "";

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
#if defined( PYSVN_HAS_CLIENT_ANNOTATE2 )
    { false, name_peg_revision },
#endif
#if defined( PYSVN_HAS_CLIENT_ANNOTATE3 )
    { false, name_ignore_space },
    { false, name_ignore_mime_type },
#endif
    { false, NULL }
    };
    FunctionArguments args( "annotate", args_desc, a_args, a_kws );
    args.check();

    std::string path( args.getUtf8String( name_url_or_path, empty_string ) );
    svn_opt_revision_t revision_start = args.getRevision( name_revision_start, svn_opt_revision_number );
    svn_opt_revision_t revision_end = args.getRevision( name_revision_end, svn_opt_revision_head );
#if defined( PYSVN_HAS_CLIENT_ANNOTATE2 )
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, revision_end );
#endif
#if defined( PYSVN_HAS_CLIENT_ANNOTATE3 )
    svn_diff_file_ignore_space_t ignore_space = svn_diff_file_ignore_space_none;
    if( args.hasArg( name_ignore_space ) )
    {
        Py::ExtensionObject< pysvn_enum_value<svn_diff_file_ignore_space_t> > py_ignore_space( args.getArg( name_ignore_space ) );
        ignore_space = svn_diff_file_ignore_space_t( py_ignore_space.extensionObject()->m_value );
    }

    svn_boolean_t ignore_eol_style = args.getBoolean( name_ignore_eol_style, false );
    svn_boolean_t ignore_mime_type = args.getBoolean( name_ignore_mime_type, false );
#endif
    SvnPool pool( m_context );

#if defined( PYSVN_HAS_CLIENT_ANNOTATE3 )
    svn_diff_file_options_t *diff_options = svn_diff_file_options_create( pool );
    diff_options->ignore_space = ignore_space;
    diff_options->ignore_eol_style = ignore_eol_style;
#endif

    std::list<AnnotatedLineInfo> all_entries;

    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#if defined( PYSVN_HAS_CLIENT_ANNOTATE3 )
        svn_error_t *error = svn_client_blame3
            (
            norm_path.c_str(),
            &peg_revision,
            &revision_start,
            &revision_end,
            diff_options,
            ignore_mime_type,
            annotate_receiver,
            &all_entries,
            m_context,
            pool
            );
#elif defined( PYSVN_HAS_CLIENT_ANNOTATE2 )
        svn_error_t *error = svn_client_blame2
            (
            norm_path.c_str(),
            &peg_revision,
            &revision_start,
            &revision_end,
            annotate_receiver,
            &all_entries,
            m_context,
            pool
            );
#else
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
#endif
        if( error != NULL )
        {
            throw SvnException( error );
        }
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
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
        entry_dict[name_line] = Py::String( entry.m_line );
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
#ifdef PYSVN_HAS_CLIENT_CAT2
    { false, name_peg_revision },
#endif
    { false, NULL }
    };
    FunctionArguments args( "cat", args_desc, a_args, a_kws );
    args.check();

    std::string path( args.getUtf8String( name_url_or_path ) );
    svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );
#ifdef PYSVN_HAS_CLIENT_CAT2
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, revision );
#endif

    SvnPool pool( m_context );

    svn_stringbuf_t * stringbuf = svn_stringbuf_create( empty_string, pool );
    svn_stream_t * stream = svn_stream_from_stringbuf( stringbuf, pool );

    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );
#ifdef PYSVN_HAS_CLIENT_CAT2
        svn_error_t *error = svn_client_cat2
            (
            stream,
            norm_path.c_str(),
            &peg_revision,
            &revision,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_cat
            (
            stream,
            norm_path.c_str(),
            &revision,
            m_context,
            pool
            );
#endif

        if (error != 0)
            throw SvnException (error);
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    // return the bytes as is to the application
    // we can assume nothing about them
    return Py::String( stringbuf->data, (int)stringbuf->len );
}

Py::Object pysvn_client::cmd_checkin( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_path },
    { true,  name_log_message },
    { false, name_recurse },
#if defined( PYSVN_HAS_CLIENT_COMMIT2 )
    { false, name_keep_locks },
#endif
    { false, NULL }
    };
    FunctionArguments args( "checkin", args_desc, a_args, a_kws );
    args.check();

    SvnPool pool( m_context );
    pysvn_commit_info_t *commit_info = NULL;

    apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_path ), pool );

    std::string type_error_message;
    try
    {
        type_error_message = "expecting string for message (arg 2)";

        std::string message( args.getUtf8String( name_log_message ) );

        type_error_message = "expecting boolean for recurse keyword arg";
        bool recurse = args.getBoolean( name_recurse, true );

#ifdef PYSVN_HAS_CLIENT_COMMIT2
        type_error_message = "expecting boolean for keep_locks keyword arg";
        bool keep_locks = args.getBoolean( name_keep_locks, true );
#endif

        try
        {
            checkThreadPermission();

            PythonAllowThreads permission( m_context );

            m_context.setLogMessage( message );

#if defined( PYSVN_HAS_CLIENT_COMMIT3 )
            svn_error_t *error = svn_client_commit3
                (
                &commit_info,   // commit info type changed
                targets,
                recurse,
                keep_locks,
                m_context,
                pool
                );
#elif defined( PYSVN_HAS_CLIENT_COMMIT2 )
            svn_error_t *error = svn_client_commit2
                (
                &commit_info,
                targets,
                recurse,
                keep_locks,
                m_context,
                pool
                );
#else
            svn_error_t *error = svn_client_commit
                (
                &commit_info,
                targets,
                !recurse,       // non recursive
                m_context,
                pool
                );
#endif
            if( error != NULL )
                throw SvnException( error );
        }
        catch( SvnException &e )
        {
            // use callback error over ClientException
            m_context.checkForError( m_module.client_error );

            throw_client_error( e );
        }
    }
    catch( Py::TypeError & )
    {
        throw Py::TypeError( type_error_message );
    }

    return toObject( commit_info );
}

Py::Object pysvn_client::cmd_checkout( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url },
    { true,  name_path },
    { false, name_recurse },
    { false, name_revision },
#ifdef PYSVN_HAS_CLIENT_CHECKOUT2
    { false, name_peg_revision },
    { false, name_ignore_externals },
#endif
    { false, NULL }
    };
    FunctionArguments args( "checkout", args_desc, a_args, a_kws );
    args.check();

    std::string url( args.getUtf8String( name_url ) );
    std::string path( args.getUtf8String( name_path ) );
    bool recurse = args.getBoolean( name_recurse, true );
    svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );
#ifdef PYSVN_HAS_CLIENT_CHECKOUT2
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, revision );
    bool ignore_externals = args.getBoolean( name_ignore_externals, false );
#endif
    SvnPool pool( m_context );

    svn_revnum_t revnum = 0;
    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

        svn_revnum_t revnum = 0;
#ifdef PYSVN_HAS_CLIENT_CHECKOUT2
        svn_error_t *error = svn_client_checkout2
            (
            &revnum,
            url.c_str(),
            norm_path.c_str(),
            &peg_revision,
            &revision,
            recurse,
            ignore_externals,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_checkout
            (
            &revnum,
            url.c_str(),
            norm_path.c_str(),
            &revision,
            recurse,
            m_context,
            pool
            );
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
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
    pysvn_commit_info_t *commit_info = NULL;

    std::string type_error_message;
    try
    {
        type_error_message = "expecting string for src_path (arg 1)";
        Py::String src_path( args.getUtf8String( name_src_url_or_path ) );

        type_error_message = "expecting string for dest_path (arg 2)";
        Py::String dest_path( args.getUtf8String( name_dest_url_or_path ) );

        type_error_message = "expecting revision for keyword src_revision";
        svn_opt_revision_t revision = args.getRevision( name_src_revision, svn_opt_revision_head );

        try
        {
            std::string norm_src_path( svnNormalisedIfPath( src_path, pool ) );
            std::string norm_dest_path( svnNormalisedIfPath( dest_path, pool ) );

            checkThreadPermission();

            PythonAllowThreads permission( m_context );

#if defined( PYSVN_HAS_CLIENT_COPY3 )
            // behavior changed
            svn_error_t *error = svn_client_copy3
                (
                &commit_info,
                norm_src_path.c_str(),
                &revision,
                norm_dest_path.c_str(),
                m_context,
                pool
                );
#elif defined( PYSVN_HAS_CLIENT_COPY2 )
            svn_error_t *error = svn_client_copy2
                (
                &commit_info,       // commit info type changed
                norm_src_path.c_str(),
                &revision,
                norm_dest_path.c_str(),
                m_context,
                pool
                );
#else
            svn_error_t *error = svn_client_copy
                (
                &commit_info,
                norm_src_path.c_str(),
                &revision,
                norm_dest_path.c_str(),
                m_context,
                pool
                );
#endif
            if( error != NULL )
                throw SvnException( error );
        }
        catch( SvnException &e )
        {
            // use callback error over ClientException
            m_context.checkForError( m_module.client_error );

            throw_client_error( e );
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
        close();
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
            std::string msg( "opening file " ); msg += m_filename;
            throw SvnException( svn_error_create( status, NULL, msg.c_str() ) );
        }
    }

    void close()
    {
        // only close if we have an open file
        if( m_apr_file == NULL )
        {
            return;
        }
        apr_file_t *apr_file = m_apr_file;

        // prevent closing the file twice
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

Py::Object pysvn_client::cmd_diff( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_tmp_path },
    { true,  name_url_or_path },
    { false, name_revision1 },
    { false, name_url_or_path2 },
    { false, name_revision2 },
    { false, name_recurse },
    { false, name_ignore_ancestry },
    { false, name_diff_deleted },
#ifdef PYSVN_HAS_CLIENT_DIFF2
    { false, name_ignore_content_type },
#endif
#if defined( PYSVN_HAS_CLIENT_DIFF3 )
    { false, name_header_encoding },
    { false, name_diff_options },
#endif
    { false, NULL }
    };
    FunctionArguments args( "diff", args_desc, a_args, a_kws );
    args.check();

    std::string tmp_path( args.getUtf8String( name_tmp_path ) );
    std::string path1( args.getUtf8String( name_url_or_path ) );
    svn_opt_revision_t revision1 = args.getRevision( name_revision1, svn_opt_revision_base );
    std::string path2( args.getUtf8String( name_url_or_path2, path1 ) );
    svn_opt_revision_t revision2 = args.getRevision( name_revision2, svn_opt_revision_working );
    bool recurse = args.getBoolean( name_recurse, true );
    bool ignore_ancestry = args.getBoolean( name_ignore_ancestry, true );
    bool diff_deleted = args.getBoolean( name_diff_deleted, true );
#ifdef PYSVN_HAS_CLIENT_DIFF2
    bool ignore_content_type = args.getBoolean( name_ignore_content_type, false );
#endif

    SvnPool pool( m_context );

#if defined( PYSVN_HAS_CLIENT_DIFF3 )
    std::string header_encoding( args.getUtf8String( name_header_encoding, empty_string ) );
    const char *header_encoding_ptr = APR_LOCALE_CHARSET;
    if( !header_encoding.empty() )
        header_encoding_ptr = header_encoding.c_str();

    apr_array_header_t *options = NULL;
    if( args.hasArg( name_diff_options ) )
    {
        options = arrayOfStringsFromListOfStrings( args.getArg( name_diff_options ), pool );
    }
    else
    {
        options = apr_array_make( pool, 0, sizeof( const char * ) );
    }
#else
    apr_array_header_t *options = apr_array_make( pool, 0, sizeof( const char * ) );
#endif

    svn_stringbuf_t *stringbuf = NULL;

    try
    {
        std::string norm_tmp_path( svnNormalisedIfPath( tmp_path, pool ) );
        std::string norm_path1( svnNormalisedIfPath( path1, pool ) );
        std::string norm_path2( svnNormalisedIfPath( path2, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );
        pysvn_apr_file output_file( pool );
        pysvn_apr_file error_file( pool );

        output_file.open_unique_file( norm_tmp_path );
        error_file.open_unique_file( norm_tmp_path );

#if defined( PYSVN_HAS_CLIENT_DIFF3 )
        svn_error_t *error = svn_client_diff3
            (
            options,
            norm_path1.c_str(), &revision1,
            norm_path2.c_str(), &revision2,
            recurse,
            ignore_ancestry,
            !diff_deleted,
            ignore_content_type,
            header_encoding_ptr,
            output_file.file(),
            error_file.file(),
            m_context,
            pool
            );
#elif defined( PYSVN_HAS_CLIENT_DIFF2 )
        svn_error_t *error = svn_client_diff2
            (
            options,
            norm_path1.c_str(), &revision1,
            norm_path2.c_str(), &revision2,
            recurse,
            ignore_ancestry,
            !diff_deleted,
            ignore_content_type,
            output_file.file(),
            error_file.file(),
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_diff
            (
            options,
            norm_path1.c_str(), &revision1,
            norm_path2.c_str(), &revision2,
            recurse,
            ignore_ancestry,
            !diff_deleted,
            output_file.file(),
            error_file.file(),
            m_context,
            pool
            );
#endif
        if( error != NULL )
            throw SvnException( error );

        output_file.close();

        output_file.open_tmp_file();
        error = svn_stringbuf_from_aprfile( &stringbuf, output_file.file(), pool );
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    // cannot convert to Unicode as we have no idea of the encoding of the bytes
    return Py::String( stringbuf->data, (int)stringbuf->len );
}

#ifdef PYSVN_HAS_CLIENT_DIFF_PEG
Py::Object pysvn_client::cmd_diff_peg( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_tmp_path },
    { true,  name_url_or_path },
    { false, name_peg_revision },
    { false, name_revision_start },
    { false, name_revision_end },
    { false, name_recurse },
    { false, name_ignore_ancestry },
    { false, name_diff_deleted },
#ifdef PYSVN_HAS_CLIENT_DIFF_PEG2
    { false, name_ignore_content_type },
#endif
#if defined( PYSVN_HAS_CLIENT_DIFF_PEG3 )
    { false, name_header_encoding },
    { false, name_diff_options },
#endif
    { false, NULL }
    };
    FunctionArguments args( "diff", args_desc, a_args, a_kws );
    args.check();

    std::string tmp_path( args.getUtf8String( name_tmp_path ) );
    std::string path( args.getUtf8String( name_url_or_path ) );
    svn_opt_revision_t revision_start = args.getRevision( name_revision1, svn_opt_revision_base );
    svn_opt_revision_t revision_end = args.getRevision( name_revision2, svn_opt_revision_working );
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, revision_end );
    bool recurse = args.getBoolean( name_recurse, true );
    bool ignore_ancestry = args.getBoolean( name_ignore_ancestry, true );
    bool diff_deleted = args.getBoolean( name_diff_deleted, true );
#ifdef PYSVN_HAS_CLIENT_DIFF_PEG2
    bool ignore_content_type = args.getBoolean( name_ignore_content_type, false );
#endif

    SvnPool pool( m_context );

#if defined( PYSVN_HAS_CLIENT_DIFF_PEG3 )
    std::string header_encoding( args.getUtf8String( name_header_encoding, empty_string ) );
    const char *header_encoding_ptr = APR_LOCALE_CHARSET;
    if( !header_encoding.empty() )
        header_encoding_ptr = header_encoding.c_str();

    apr_array_header_t *options = NULL;
    if( args.hasArg( name_diff_options ) )
    {
        options = arrayOfStringsFromListOfStrings( args.getArg( name_diff_options ), pool );
    }
    else
    {
        options = apr_array_make( pool, 0, sizeof( const char * ) );
    }
#else
    apr_array_header_t *options = apr_array_make( pool, 0, sizeof( const char * ) );
#endif

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

#if defined( PYSVN_HAS_CLIENT_DIFF_PEG3 )
        svn_error_t *error = svn_client_diff_peg3
            (
            options,
            norm_path.c_str(),
            &peg_revision,
            &revision_start,
            &revision_end,
            recurse,
            ignore_ancestry,
            !diff_deleted,
            ignore_content_type,
            header_encoding_ptr,
            output_file.file(),
            error_file.file(),
            m_context,
            pool
            );
#elif defined( PYSVN_HAS_CLIENT_DIFF_PEG2 )
        svn_error_t *error = svn_client_diff_peg2
            (
            options,
            norm_path.c_str(),
            &peg_revision,
            &revision_start,
            &revision_end,
            recurse,
            ignore_ancestry,
            !diff_deleted,
            ignore_content_type,
            output_file.file(),
            error_file.file(),
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_diff_peg
            (
            options,
            norm_path.c_str(),
            &peg_revision,
            &revision_start,
            &revision_end,
            recurse,
            ignore_ancestry,
            !diff_deleted,
            output_file.file(),
            error_file.file(),
            m_context,
            pool
            );
#endif
        if( error != NULL )
            throw SvnException( error );

        output_file.close();

        output_file.open_tmp_file();
        error = svn_stringbuf_from_aprfile( &stringbuf, output_file.file(), pool );
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    // cannot convert to Unicode as we have no idea of the encoding of the bytes
    return Py::String( stringbuf->data, (int)stringbuf->len );
}
#endif

Py::Object pysvn_client::cmd_export( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_src_url_or_path },
    { true,  name_dest_path },
    { false, name_force },
    { false, name_revision },
#ifdef PYSVN_HAS_CLIENT_EXPORT2
    { false, name_native_eol },
#endif
#ifdef PYSVN_HAS_CLIENT_EXPORT3
    { false, name_ignore_externals },
    { false, name_recurse },
    { false, name_peg_revision },
#endif
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


#ifdef PYSVN_HAS_CLIENT_EXPORT2
    char *native_eol = NULL;
    if( args.hasArg( name_native_eol ) )
    {
        Py::Object native_eol_obj = args.getArg( name_native_eol );
        if( native_eol_obj != Py::None() )
        {
            Py::String eol_py_str( native_eol_obj );
            std::string eol_str = eol_py_str.as_std_string();
            if( eol_str == "CR" )
                native_eol = "CR";
            else if( eol_str == "CRLF" )
                native_eol = "CRLF";
            else if( eol_str == "LF" )
                native_eol = "LF";
            else
                throw Py::ValueError( "native_eol must be one of None, \"LF\", \"CRLF\" or \"CR\"" );
        }
    }
#endif
#ifdef PYSVN_HAS_CLIENT_EXPORT3
    bool recurse = args.getBoolean( name_recurse, true );
    bool ignore_externals = args.getBoolean( name_ignore_externals, false );
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, revision );
#endif

    svn_revnum_t revnum = 0;

    SvnPool pool( m_context );

    try
    {
        std::string norm_src_path( svnNormalisedIfPath( src_path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#ifdef PYSVN_HAS_CLIENT_EXPORT3
        svn_error_t * error = svn_client_export3
            (
            &revnum,
            norm_src_path.c_str(),
            dest_path.c_str(),
            &peg_revision,
            &revision,
            force,
            ignore_externals,
            recurse,
            native_eol,
            m_context,
            pool
            );
#else
#ifdef PYSVN_HAS_CLIENT_EXPORT2
        svn_error_t * error = svn_client_export2
            (
            &revnum,
            norm_src_path.c_str(),
            dest_path.c_str(),
            &revision,
            force,
            native_eol,
            m_context,
            pool
            );
#else
        svn_error_t * error = svn_client_export
            (
            &revnum,
            norm_src_path.c_str(),
            dest_path.c_str(),
            &revision,
            force,
            m_context,
            pool
            );
#endif
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
}


Py::Object pysvn_client::cmd_import( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_path },
    { true,  name_url },
    { true,  name_log_message },
    { false, name_recurse },
#ifdef PYSVN_HAS_CLIENT_IMPORT2
    { false, name_ignore },
#endif
    { false, NULL }
    };
    FunctionArguments args( "import_", args_desc, a_args, a_kws );
    args.check();

    std::string path( args.getUtf8String( name_path ) );
    std::string url( args.getUtf8String( name_url ) );
    std::string message( args.getUtf8String( name_log_message ) );

    bool recurse = args.getBoolean( name_recurse, true );
#ifdef PYSVN_HAS_CLIENT_IMPORT2
    bool ignore = args.getBoolean( name_ignore, false );
#endif

    SvnPool pool( m_context );
    pysvn_commit_info_t *commit_info = NULL;

    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

        m_context.setLogMessage( message.c_str() );

#if defined( PYSVN_HAS_CLIENT_IMPORT2 )
        svn_error_t *error = svn_client_import2
            (
            &commit_info,       // changed type
            norm_path.c_str(),
            url.c_str(),
            !recurse,           // non_recursive
            !ignore,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_import
            (
            &commit_info,
            norm_path.c_str(),
            url.c_str(),
            !recurse,           // non_recursive
            m_context,
            pool
            );
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return toObject( commit_info );
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

    const svn_wc_entry_t *entry = NULL;

    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

        svn_wc_adm_access_t *adm_access = NULL;

        svn_error_t *error = svn_wc_adm_probe_open( &adm_access, NULL, norm_path.c_str(), false, false, pool );
        if( error != NULL )
            throw SvnException( error );

        error = svn_wc_entry( &entry, norm_path.c_str(), adm_access, false, pool );
        if( error != NULL )
            throw SvnException( error );
        
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
        return Py::None();       // needed to remove warning about return value missing
    }

    if( entry == NULL )
        return Py::None();

    return toObject( *entry, pool, m_wrapper_entry );
}

#ifdef PYSVN_HAS_CLIENT_INFO
class InfoReceiveBaton
{
public:
    InfoReceiveBaton
        (
        PythonAllowThreads *permission,
        const DictWrapper &wrapper_info,
        const DictWrapper &wrapper_lock,
        const DictWrapper &wrapper_wc_info
        )
    : m_permission( permission )
    , m_info_list()
    , m_wrapper_info( wrapper_info )
    , m_wrapper_lock( wrapper_lock )
    , m_wrapper_wc_info( wrapper_wc_info )
    {}
    ~InfoReceiveBaton()
    {}

    PythonAllowThreads  *m_permission;
    Py::List            m_info_list;
    const DictWrapper   &m_wrapper_info;
    const DictWrapper   &m_wrapper_lock;
    const DictWrapper   &m_wrapper_wc_info;
};

extern "C"
{
static svn_error_t *info_receiver_c( void *baton_, const char *path, const svn_info_t *info, apr_pool_t *pool )
{
    InfoReceiveBaton *baton = reinterpret_cast<InfoReceiveBaton *>( baton_ );

    PythonDisallowThreads callback_permission( baton->m_permission );

    if( path != NULL )
    {
        std::string std_path( path );
        if( std_path.empty() )
        {
            std_path = ".";
        }
        Py::String py_path( std_path );

        Py::Tuple py_pair( 2 );
        py_pair[0] = py_path;
        py_pair[1] = toObject(
                    *info,
                    baton->m_wrapper_info,
                    baton->m_wrapper_lock,
                    baton->m_wrapper_wc_info );

        baton->m_info_list.append( py_pair );
    }

    return SVN_NO_ERROR;
}
}

Py::Object pysvn_client::cmd_info2( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url_or_path },
    { false, name_revision },
    { false, name_peg_revision},
    { false, name_recurse },
    { false, NULL }
    };
    FunctionArguments args( "info2", args_desc, a_args, a_kws );
    args.check();

    std::string path( args.getUtf8String( name_url_or_path ) );

    svn_opt_revision_kind kind = svn_opt_revision_unspecified;
    if( is_svn_url( path ) )
        kind = svn_opt_revision_head;

    svn_opt_revision_t revision = args.getRevision( name_revision, kind );
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, revision );

    bool recurse = args.getBoolean( name_recurse, true );

    SvnPool pool( m_context );
    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

        InfoReceiveBaton info_baton( &permission, m_wrapper_info, m_wrapper_lock, m_wrapper_wc_info );

        svn_error_t *error = 
            svn_client_info
                (
                norm_path.c_str(),
                &peg_revision,
                &revision,
                info_receiver_c,
                reinterpret_cast<void *>( &info_baton ),
                recurse,
                m_context,
                pool
                );

        if( error != NULL )
            throw SvnException( error );

        return info_baton.m_info_list;
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
        return Py::None();          // needed to remove warning about return value missing
    }
}
#endif

class LogChangePathInfo
{
public:
    LogChangePathInfo( const char *path, svn_log_changed_path_t *info )
    : m_path( path )
    , m_action( info->action )
    , m_copy_from_path( info->copyfrom_path != NULL ? info->copyfrom_path : "" )
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

    std::string    m_path;
    char        m_action;
    std::string    m_copy_from_path;
    svn_revnum_t    m_copy_from_revision;
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

    if( author == NULL )
        author = "";
    if( date == NULL )
        date = "";
    if( msg == NULL )
        msg = "";

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

#ifdef PYSVN_HAS_CLIENT_LOCK
Py::Object pysvn_client::cmd_lock( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url_or_path },
    { true,  name_comment },
    { false, name_force },
    { false, NULL }
    };
    FunctionArguments args( "lock", args_desc, a_args, a_kws );
    args.check();

    SvnPool pool( m_context );

    apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_url_or_path ), pool );

    std::string type_error_message;
    try
    {
        type_error_message = "expecting string for comment (arg 2)";

        std::string comment( args.getUtf8String( name_comment ) );

        type_error_message = "expecting boolean for force keyword arg";
        bool force = args.getBoolean( name_force, true );

        try
        {
            checkThreadPermission();

            PythonAllowThreads permission( m_context );

            svn_error_t *error = svn_client_lock
                (
                targets,
                comment.c_str(),
                force,        // non recursive
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

            throw_client_error( e );
        }
    }
    catch( Py::TypeError & )
    {
        throw Py::TypeError( type_error_message );
    }

    return Py::None();
}
#endif

Py::Object pysvn_client::cmd_log( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url_or_path },
    { false, name_revision_start },
    { false, name_revision_end },
    { false, name_discover_changed_paths },
    { false, name_strict_node_history },
#if defined( PYSVN_HAS_CLIENT_LOG2 ) || defined( PYSVN_HAS_CLIENT_LOG3 )
    { false, name_limit },
#endif
#ifdef PYSVN_HAS_CLIENT_LOG3
    { false, name_peg_revision },
#endif
    { false, NULL }
    };
    FunctionArguments args( "log", args_desc, a_args, a_kws );
    args.check();

    svn_opt_revision_t revision_start = args.getRevision( name_revision_start, svn_opt_revision_head );
    svn_opt_revision_t revision_end = args.getRevision( name_revision_end, svn_opt_revision_number );
    bool discover_changed_paths = args.getBoolean( name_discover_changed_paths, false );
    bool strict_node_history = args.getBoolean( name_strict_node_history, true );
    int limit = args.getInteger( name_limit, 0 );
#ifdef PYSVN_HAS_CLIENT_LOG3
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, svn_opt_revision_unspecified );
#endif
    SvnPool pool( m_context );
    std::list<LogEntryInfo> all_entries;

    apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_url_or_path ), pool );

    try
    {
        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#if defined( PYSVN_HAS_CLIENT_LOG3 )
        svn_error_t *error = svn_client_log3
            (
            targets,
            &peg_revision,
            &revision_start,
            &revision_end,
            limit,
            discover_changed_paths,
            strict_node_history,
            logReceiver,
            &all_entries,
            m_context,
            pool
            );
#elif defined( PYSVN_HAS_CLIENT_LOG2 )
        svn_error_t *error = svn_client_log2
            (
            targets,
            &revision_start,
            &revision_end,
            limit,
            discover_changed_paths,
            strict_node_history,
            logReceiver,
            &all_entries,
            m_context,
            pool
            );
#else
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
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
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
                changed_entry_dict[name_copyfrom_revision] = Py::None();

            changed_paths_list.append( changed_entry_dict );
        }

        entry_dict[name_changed_paths] = changed_paths_list;

        entries_list.append( entry_dict );
    }

    return entries_list;
}

#if defined( PYSVN_HAS_CLIENT_LIST )
class ListReceiveBaton
{
public:
    ListReceiveBaton( PythonAllowThreads *permission)
        : m_permission( permission )
        , m_list_list()
        {}
    ~ListReceiveBaton()
        {}

    PythonAllowThreads  *m_permission;

    apr_uint32_t        m_dirent_fields;
    bool                m_fetch_locks;
    bool                m_is_url;
    std::string         m_url_or_path;
    DictWrapper         *m_wrapper_lock;

    Py::List            m_list_list;
};

extern "C"
{
svn_error_t *list_receiver_c
    (
    void *baton_,
    const char *path,
    const svn_dirent_t *dirent,
    const svn_lock_t *lock,
    const char *abs_path,
    apr_pool_t *pool
    )
{
    ListReceiveBaton *baton = reinterpret_cast<ListReceiveBaton *>( baton_ );

    PythonDisallowThreads callback_permission( baton->m_permission );

    std::string full_path( baton->m_url_or_path );
    std::string full_repos_path( abs_path );
    if( strlen( path ) != 0 )
    {
        full_path += "/";
        full_path += path;

        full_repos_path += "/";
        full_repos_path += path;
    }

    Py::Tuple py_tuple( 3 );
    py_tuple[0] = Py::String( full_path, name_utf8 );

    Py::Dict entry_dict;
    entry_dict[ *py_name_repos_path ] = Py::String( full_repos_path, name_utf8 );

    if( dirent != NULL )
    {
        if( baton->m_dirent_fields&SVN_DIRENT_KIND )
        {
            entry_dict[ *py_name_kind ] = toEnumValue( dirent->kind );
        }
        if( baton->m_dirent_fields&SVN_DIRENT_SIZE )
        {
            entry_dict[ *py_name_size ] = Py::Long( Py::Float( double( static_cast<signed_int64>( dirent->size ) ) ) );
        }
        if( baton->m_dirent_fields&SVN_DIRENT_CREATED_REV )
        {
            entry_dict[ *py_name_created_rev ] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, dirent->created_rev ) );
        }
        if( baton->m_dirent_fields&SVN_DIRENT_TIME )
        {
            entry_dict[ *py_name_time ] = toObject( dirent->time );
        }
        if( baton->m_dirent_fields&SVN_DIRENT_HAS_PROPS )
        {
            entry_dict[ *py_name_has_props ] = Py::Int( dirent->has_props );
        }
        if( baton->m_dirent_fields&SVN_DIRENT_LAST_AUTHOR )
        {
            entry_dict[ *py_name_last_author ] = utf8_string_or_none( dirent->last_author );
        }
    }
    py_tuple[1] = entry_dict;

    if( lock == NULL )
    {
        py_tuple[2] = Py::None();
    }
    else
    {
        py_tuple[2] = toObject( *lock, *baton->m_wrapper_lock );
    }
    baton->m_list_list.append( py_tuple );

    return SVN_NO_ERROR;
}
}

Py::Object pysvn_client::cmd_list( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url_or_path },
    { false, name_peg_revision },
    { false, name_revision },
    { false, name_recurse },
    { false, name_dirent_fields },
    { false, name_fetch_locks },
    { false, NULL }
    };
    FunctionArguments args( "list", args_desc, a_args, a_kws );
    args.check();

    std::string path( args.getUtf8String( name_url_or_path ) );
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, svn_opt_revision_unspecified );
    bool is_url = is_svn_url( path );
    svn_opt_revision_t revision;
    if( is_url )
         revision = args.getRevision( name_revision, svn_opt_revision_head );
    else
         revision = args.getRevision( name_revision, svn_opt_revision_working );
    bool recurse = args.getBoolean( name_recurse, false );
    apr_uint32_t dirent_fields = args.getInteger( name_dirent_fields, SVN_DIRENT_ALL );
    bool fetch_locks = args.getBoolean( name_fetch_locks, false );

    SvnPool pool( m_context );
    std::string norm_path( svnNormalisedIfPath( path, pool ) );

    try
    {
        checkThreadPermission();

        PythonAllowThreads permission( m_context );

        ListReceiveBaton list_baton( &permission );
        list_baton.m_dirent_fields = dirent_fields;
        list_baton.m_is_url = is_url;
        list_baton.m_fetch_locks = fetch_locks;
        list_baton.m_url_or_path = norm_path;
        list_baton.m_wrapper_lock = &m_wrapper_lock;

        svn_error_t *error = svn_client_list
            (
            norm_path.c_str(),
            &peg_revision,
            &revision,
            recurse,
            dirent_fields,
            fetch_locks,
            list_receiver_c,
            reinterpret_cast<void *>( &list_baton ),
            m_context,
            pool
            );
        if( error != 0 )
            throw SvnException( error );

        return list_baton.m_list_list;
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
        return Py::None();          // needed to remove warning about return value missing
    }
}
#endif

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
#ifdef PYSVN_HAS_CLIENT_LS2
    { false, name_peg_revision },
#endif
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
#ifdef PYSVN_HAS_CLIENT_LS2
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, revision );
#endif

    try
    {
        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#ifdef PYSVN_HAS_CLIENT_LS2
        svn_error_t *error = svn_client_ls2
            (
            &hash,
            norm_path.c_str(),
            &peg_revision,
            &revision,
            recurse,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_ls
            (
            &hash,
            norm_path.c_str(),
            &revision,
            recurse,
            m_context,
            pool
            );
#endif

        if( error != 0 )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
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
        entry_dict[ *py_name_name ] = Py::String( full_name, name_utf8 );
        entry_dict[ *py_name_kind ] = toEnumValue( dirent->kind );
        entry_dict[ *py_name_has_props ] = Py::Int( dirent->has_props );
        entry_dict[ *py_name_size ] = Py::Long( Py::Float( double( static_cast<signed_int64>( dirent->size ) ) ) );
        entry_dict[ *py_name_created_rev ] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, dirent->created_rev ) );
        entry_dict[ *py_name_time ] = toObject( dirent->time );
        entry_dict[ *py_name_last_author ] = utf8_string_or_none( dirent->last_author );

        entries_list.append( m_wrapper_dirent.wrapDict( entry_dict ) );
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
#if defined( PYSVN_HAS_CLIENT_MERGE2 )
    { false, name_merge_options },
#endif
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
    bool dry_run = args.getBoolean( name_dry_run, false );

#if defined( PYSVN_HAS_CLIENT_MERGE2 )
    Py::List merge_options_list;
    if( args.hasArg( name_merge_options ) )
    {
        merge_options_list = args.getArg( name_merge_options );
        for( size_t i=0; i<merge_options_list.length(); i++ )
        {
            Py::String check_is_string( merge_options_list[i] );
        }
    }
#endif

    SvnPool pool( m_context );

#if defined( PYSVN_HAS_CLIENT_MERGE2 )
    apr_array_header_t *merge_options = NULL;
    if( merge_options_list.length() > 0 )
    {
        merge_options = apr_array_make( pool, merge_options_list.length(), sizeof( const char * ) );
        for( size_t i=0; i<merge_options_list.length(); i++ )
        {
            Py::String py_option( merge_options_list[i] );
            std::string option( py_option.as_std_string() );
            
            *((const char **) apr_array_push(merge_options)) = apr_pstrdup( pool, option.c_str() );
        }
    }
#endif

    try
    {
        std::string norm_path1( svnNormalisedIfPath( path1, pool ) );
        std::string norm_path2( svnNormalisedIfPath( path2, pool ) );
        std::string norm_local_path( svnNormalisedIfPath( local_path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#if defined( PYSVN_HAS_CLIENT_MERGE2 )
        svn_error_t *error = svn_client_merge2
            (
            norm_path1.c_str(),
            &revision1,
            norm_path2.c_str(),
            &revision2,
            norm_local_path.c_str(),
            recurse,
            !notice_ancestry,
            force,
            dry_run,
            merge_options,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_merge
            (
            norm_path1.c_str(),
            &revision1,
            norm_path2.c_str(),
            &revision2,
            norm_local_path.c_str(),
            recurse,
            !notice_ancestry,
            force,
            dry_run,
            m_context,
            pool
            );
#endif
        if( error != 0 )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
}

#ifdef PYSVN_HAS_CLIENT_MERGE_PEG
Py::Object pysvn_client::cmd_merge_peg( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url_or_path },
    { true,  name_revision1 },
    { true,  name_revision2 },
    { true,  name_peg_revision },
    { true,  name_local_path },
    { false, name_recurse },
    { false, name_notice_ancestry },
    { false, name_force },
    { false, name_dry_run },
#if defined( PYSVN_HAS_CLIENT_MERGE_PEG2 )
    { false, name_merge_options },
#endif
    { false, NULL }
    };
    FunctionArguments args( "merge", args_desc, a_args, a_kws );
    args.check();

    std::string path( args.getUtf8String( name_url_or_path ) );
    svn_opt_revision_t revision1 = args.getRevision( name_revision1, svn_opt_revision_head );
    svn_opt_revision_t revision2 = args.getRevision( name_revision2, svn_opt_revision_head );
    svn_opt_revision_t peg_revision = args.getRevision( name_revision2, revision2 );
    std::string local_path( args.getUtf8String( name_local_path ) );

    bool force = args.getBoolean( name_force, false );
    bool recurse = args.getBoolean( name_recurse, true );
    bool notice_ancestry = args.getBoolean( name_notice_ancestry, false );
    bool dry_run = args.getBoolean( name_dry_run, false );

#if defined( PYSVN_HAS_CLIENT_MERGE_PEG2 )
    Py::List merge_options_list;
    if( args.hasArg( name_merge_options ) )
    {
        merge_options_list = args.getArg( name_merge_options );
        for( size_t i=0; i<merge_options_list.length(); i++ )
        {
            Py::String check_is_string( merge_options_list[i] );
        }
    }
#endif

    SvnPool pool( m_context );

#if defined( PYSVN_HAS_CLIENT_MERGE_PEG2 )
    apr_array_header_t *merge_options = NULL;
    if( merge_options_list.length() > 0 )
    {
        merge_options = apr_array_make( pool, merge_options_list.length(), sizeof( const char * ) );
        for( size_t i=0; i<merge_options_list.length(); i++ )
        {
            Py::String py_option( merge_options_list[i] );
            std::string option( py_option.as_std_string() );
            
            *((const char **) apr_array_push(merge_options)) = apr_pstrdup( pool, option.c_str() );
        }
    }
#endif

    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );
        std::string norm_local_path( svnNormalisedIfPath( local_path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#if defined( PYSVN_HAS_CLIENT_MERGE_PEG2 )
        svn_error_t *error = svn_client_merge_peg2
            (
            norm_path.c_str(),
            &revision1,
            &revision2,
            &peg_revision,
            norm_local_path.c_str(),
            recurse,
            !notice_ancestry,
            force,
            dry_run,
            merge_options,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_merge_peg
            (
            norm_path.c_str(),
            &revision1,
            &revision2,
            &peg_revision,
            norm_local_path.c_str(),
            recurse,
            !notice_ancestry,
            force,
            dry_run,
            m_context,
            pool
            );
#endif
        if( error != 0 )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
}
#endif

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

    pysvn_commit_info_t *commit_info = NULL;

    try
    {
        checkThreadPermission();

        PythonAllowThreads permission( m_context );

        m_context.setLogMessage( message.c_str() );


#if defined( PYSVN_HAS_CLIENT_MKDIR2 )
        svn_error_t *error = svn_client_mkdir2
            (
            &commit_info,       // changed type
            targets,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_mkdir
            (
            &commit_info,
            targets,
            m_context,
            pool
            );
#endif
        if( error != 0 )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return toObject( commit_info );
}

Py::Object pysvn_client::cmd_move( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_src_url_or_path },
    { true,  name_dest_url_or_path },
    { false, name_force },
    { false, NULL }
    };
    FunctionArguments args( "move", args_desc, a_args, a_kws );
    args.check();

    SvnPool pool( m_context );
    pysvn_commit_info_t *commit_info = NULL;

    std::string type_error_message;
    try
    {
        type_error_message = "expecting string for src_url_or_path (arg 1)";
        Py::String src_path( args.getUtf8String( name_src_url_or_path ) );

        type_error_message = "expecting string for dest_url_or_path (arg 2)";
        Py::String dest_path( args.getUtf8String( name_dest_url_or_path ) );

#ifndef PYSVN_HAS_CLIENT_MOVE2
        svn_opt_revision_t revision;
        revision.kind = svn_opt_revision_head;
#endif

        type_error_message = "expecting boolean for keyword force";
        bool force = args.getBoolean( name_force, false );

        try
        {
            std::string norm_src_path( svnNormalisedIfPath( src_path, pool ) );
            std::string norm_dest_path( svnNormalisedIfPath( dest_path, pool ) );

            checkThreadPermission();

            PythonAllowThreads permission( m_context );

#if defined( PYSVN_HAS_CLIENT_MOVE4 )
            // behavior changed
            svn_error_t *error = svn_client_move4
                (
                &commit_info,
                norm_src_path.c_str(),
                norm_dest_path.c_str(),
                force,
                m_context,
                pool
                );
#elif defined( PYSVN_HAS_CLIENT_MOVE3 )
            svn_error_t *error = svn_client_move3
                (
                &commit_info,               // changed type
                norm_src_path.c_str(),
                norm_dest_path.c_str(),
                force,
                m_context,
                pool
                );
#elif defined( PYSVN_HAS_CLIENT_MOVE2 )
            svn_error_t *error = svn_client_move2
                (
                &commit_info,
                norm_src_path.c_str(),
                norm_dest_path.c_str(),
                force,
                m_context,
                pool
                );
#else
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
#endif

            if( error != NULL )
                throw SvnException( error );
        }
        catch( SvnException &e )
        {
            // use callback error over ClientException
            m_context.checkForError( m_module.client_error );

            throw_client_error( e );
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
#ifdef PYSVN_HAS_CLIENT_PROPSET2
    { false, name_skip_checks },
#endif
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
#ifdef PYSVN_HAS_CLIENT_PROPSET2
    bool skip_checks = args.getBoolean( name_skip_checks, false );
#endif

    SvnPool pool( m_context );
    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#ifdef PYSVN_HAS_CLIENT_PROPSET2
        svn_error_t *error = svn_client_propset2
            (
            propname.c_str(),
            NULL, // value = NULL
            norm_path.c_str(),
            recurse,
            skip_checks,
            m_context.ctx(),
            pool
            );
#else
        svn_error_t *error = svn_client_propset
            (
            propname.c_str(),
            NULL, // value = NULL
            norm_path.c_str(),
            recurse,
            pool
            );
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
}

Py::Object pysvn_client::cmd_propget( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_prop_name },
    { true,  name_url_or_path },
    { false, name_revision },
    { false, name_recurse },
#ifdef PYSVN_HAS_CLIENT_PROPGET2
    { false, name_peg_revision },
#endif
    { false, NULL }
    };
    FunctionArguments args( "propget", args_desc, a_args, a_kws );
    args.check();

    std::string propname( args.getUtf8String( name_prop_name ) );
    std::string path( args.getUtf8String( name_url_or_path ) );

    bool recurse = args.getBoolean( name_recurse, false );
    svn_opt_revision_t revision;
    if( is_svn_url( path ) )
        revision = args.getRevision( name_revision, svn_opt_revision_head );
    else
        revision = args.getRevision( name_revision, svn_opt_revision_working );
#ifdef PYSVN_HAS_CLIENT_PROPGET2
    svn_opt_revision_t peg_revision = args.getRevision( name_peg_revision, revision );
#endif

    SvnPool pool( m_context );
    apr_hash_t *props = NULL;

    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );
#ifdef PYSVN_HAS_CLIENT_PROPGET2
        svn_error_t *error = svn_client_propget2
            (
            &props,
            propname.c_str(),
            norm_path.c_str(),
            &peg_revision,
            &revision,
            recurse,
            m_context,
            pool
            );
#else
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
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return propsToObject( props, pool );
}

Py::Object pysvn_client::cmd_proplist( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url_or_path },
    { false, name_revision },
    { false, name_recurse },
#ifdef PYSVN_HAS_CLIENT_PROPLIST2
    { false, name_peg_revision },
#endif
    { false, NULL }
    };
    FunctionArguments args( "proplist", args_desc, a_args, a_kws );
    args.check();

    Py::List path_list( toListOfStrings( args.getArg( name_url_or_path ) ) );

    bool recurse = args.getBoolean( name_recurse, false );

    bool is_revision_setup = false;
    bool is_url = false;

    svn_opt_revision_t revision_url;
    svn_opt_revision_t revision_file;
    if( args.hasArg( name_revision ) )
    {
        revision_url = args.getRevision( name_revision );
        revision_file = revision_url;
    }
    else
    {
        revision_url.kind = svn_opt_revision_head;
        revision_file.kind = svn_opt_revision_working;
    }
#ifdef PYSVN_HAS_CLIENT_PROPLIST2
    svn_opt_revision_t peg_revision_url;
    svn_opt_revision_t peg_revision_file;
    if( args.hasArg( name_peg_revision ) )
    {
        peg_revision_url = args.getRevision( name_peg_revision );
        peg_revision_file = peg_revision_url;
    }
    else
    {
        peg_revision_url = revision_url;
        peg_revision_file = revision_file;
    }
#endif

    SvnPool pool( m_context );

    Py::List list_of_proplists;

    for( Py::List::size_type i=0; i<path_list.length(); i++ )
    {
        Py::String path_str( asUtf8String( path_list[i] ) );
        std::string path( path_str.as_std_string() );
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        svn_opt_revision_t revision;
        svn_opt_revision_t peg_revision;
        if( !is_revision_setup )
            if( is_svn_url( path ) )
            {
                revision = revision_url;
                peg_revision = peg_revision_url;
                is_url = true;
            }
            else
            {
                revision = revision_file;
                peg_revision = peg_revision_file;
            }
        else
            if( is_svn_url( path ) && !is_url )
            {
                throw Py::AttributeError( "cannot mix URL and PATH in name_path" );
            }

        apr_array_header_t *props = NULL;
        try
        {
            const char *norm_path_c_str= norm_path.c_str();
            checkThreadPermission();

            PythonAllowThreads permission( m_context );
#ifdef PYSVN_HAS_CLIENT_PROPLIST2
            svn_error_t *error = svn_client_proplist2
                (
                &props,
                norm_path_c_str,
                &peg_revision,
                &revision,
                recurse,
                m_context,
                pool
                );
#else
            svn_error_t *error = svn_client_proplist
                (
                &props,
                norm_path_c_str,
                &revision,
                recurse,
                m_context,
                pool
                );
#endif
            if( error != NULL )
                throw SvnException( error );
        }
        catch( SvnException &e )
        {
            // use callback error over ClientException
            m_context.checkForError( m_module.client_error );

            throw_client_error( e );
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
#ifdef PYSVN_HAS_CLIENT_PROPSET2
    { false, name_skip_checks },
#endif
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
#ifdef PYSVN_HAS_CLIENT_PROPSET2
    bool skip_checks = args.getBoolean( name_skip_checks, false );
#endif

    SvnPool pool( m_context );

    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

        const svn_string_t *svn_propval = svn_string_ncreate( propval.c_str(), propval.size(), pool );

#ifdef PYSVN_HAS_CLIENT_PROPSET2
        svn_error_t *error = svn_client_propset2
            (
            propname.c_str(),
            svn_propval,
            norm_path.c_str(),
            recurse,
            skip_checks,
            m_context.ctx(),
            pool
            );
#else
        svn_error_t *error = svn_client_propset
            (
            propname.c_str(),
            svn_propval,
            norm_path.c_str(),
            recurse,
            pool
            );
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
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

    pysvn_commit_info_t *commit_info = NULL;
    try
    {
        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#if defined( PYSVN_HAS_CLIENT_DELETE2 )
        svn_error_t *error = svn_client_delete2
            (
            &commit_info,       // commit_info changed
            targets,
            force,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_delete
            (
            &commit_info,
            targets,
            force,
            m_context,
            pool
            );
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
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
            // use callback error over ClientException
            m_context.checkForError( m_module.client_error );

            throw_client_error( e );
        }
    }
    catch( Py::TypeError & )
    {
        throw Py::TypeError( type_error_message );
    }

    return Py::None();
}

Py::Object pysvn_client::cmd_revpropdel( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_prop_name },
    { true,  name_url },
    { false, name_revision },
    { false, name_force },
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
            NULL,            // value = NULL
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    Py::Tuple result(2);
    result[0] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
    // prop_name that is not in this rev returns a NULL value
    if( propval == NULL )
    {
        result[1] = Py::None();
    }
    else
    {
        result[1] = Py::String( propval->data, propval->len, name_utf8 );
    }

    return result;
}

Py::Object pysvn_client::cmd_revproplist( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url },
    { false, name_revision },
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
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
    { false, name_force },
    { false, NULL }
    };
    FunctionArguments args( "revpropset", args_desc, a_args, a_kws );
    args.check();

    std::string propname( args.getUtf8String( name_prop_name ) );
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

        const svn_string_t *svn_propval = svn_string_ncreate( propval.c_str(), propval.size(), pool );
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
}


struct StatusEntriesBaton
{
    apr_pool_t* pool;
    apr_hash_t* hash;
};

#ifdef PYSVN_HAS_CLIENT_STATUS2
static void StatusEntriesFunc
    (
    void *baton,
    const char *path,
    svn_wc_status2_t *status
    )
{
    svn_wc_status2_t *stat;
    StatusEntriesBaton* seb = (StatusEntriesBaton*)baton;

    path = apr_pstrdup( seb->pool, path );
    stat = svn_wc_dup_status2( status, seb->pool );
    apr_hash_set( seb->hash, path, APR_HASH_KEY_STRING, stat );
}
#else
static void StatusEntriesFunc
    (
    void *baton,
    const char *path,
    svn_wc_status_t *status
    )
{
    svn_wc_status_t *stat;
    StatusEntriesBaton* seb = (StatusEntriesBaton*)baton;

    path = apr_pstrdup( seb->pool, path );
    stat = svn_wc_dup_status( status, seb->pool );
    apr_hash_set( seb->hash, path, APR_HASH_KEY_STRING, stat );
}
#endif


//qqq
extern Py::Object toObject(
    const char *path, pysvn_wc_status_t &svn_status,
    const DictWrapper &wrapper_status,
    const DictWrapper &wrapper_lock
    );

Py::Object pysvn_client::cmd_status( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_path },
    { false, name_recurse },
    { false, name_get_all },
    { false, name_update },
    { false, name_ignore },
#ifdef PYSVN_HAS_CLIENT_STATUS2
    { false, name_ignore_externals },
#endif
    { false, NULL }
    };
    FunctionArguments args( "status", args_desc, a_args, a_kws );
    args.check();

    Py::String path( args.getUtf8String( name_path ) );
    bool recurse = args.getBoolean( name_recurse, true );
    bool get_all = args.getBoolean( name_get_all, true );
    bool update = args.getBoolean( name_update, false );
    bool ignore = args.getBoolean( name_ignore, false );
#ifdef PYSVN_HAS_CLIENT_STATUS2
    bool ignore_externals = args.getBoolean( name_ignore_externals, false );
#endif

    SvnPool pool( m_context );
    apr_hash_t *status_hash = NULL;

    Py::List entries_list;
    try
    {
        std::string norm_path( svnNormalisedIfPath( path, pool ) );

        checkThreadPermission();

        PythonAllowThreads permission( m_context );

        svn_revnum_t revnum;
        svn_opt_revision_t rev = { svn_opt_revision_head, {0} };

        StatusEntriesBaton baton;

        status_hash = apr_hash_make( pool );
        baton.hash = status_hash;
        baton.pool = pool;

#ifdef PYSVN_HAS_CLIENT_STATUS2
        svn_error_t *error = svn_client_status2
            (
            &revnum,            // revnum
            norm_path.c_str(),  // path
            &rev,
            StatusEntriesFunc,  // status func
            &baton,             // status baton
            recurse,
            get_all,
            update,
            !ignore,
            ignore_externals,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_status
            (
            &revnum,            // revnum
            norm_path.c_str(),  // path
            &rev,
            StatusEntriesFunc,  // status func
            &baton,             // status baton
            recurse,
            get_all,
            update,
            !ignore,
            m_context,
            pool
            );
#endif

        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    apr_array_header_t *statusarray = svn_sort__hash( status_hash, svn_sort_compare_items_as_paths, pool );

    // Loop over array, printing each name/status-structure
    for (int i = statusarray->nelts-1; i >= 0; i--)
    {
        const svn_sort__item_t *item = &APR_ARRAY_IDX( statusarray, i, const svn_sort__item_t );
        pysvn_wc_status_t *status = (pysvn_wc_status_t *)item->value;

//        entries_list.append( Py::asObject(
//            new pysvn_status( (const char *)item->key, status, m_context,
//                m_wrapper_lock ) ) );
        entries_list.append( toObject(
                Py::String( osNormalisedPath( (const char *)item->key, pool ), "UTF-8" ),
                *status,
                pool,
                m_wrapper_status,
                m_wrapper_entry,
                m_wrapper_lock ) );
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
}

#ifdef PYSVN_HAS_CLIENT_LOCK
Py::Object pysvn_client::cmd_unlock( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_url_or_path },
    { false, name_force },
    { false, NULL }
    };
    FunctionArguments args( "unlock", args_desc, a_args, a_kws );
    args.check();

    SvnPool pool( m_context );

    apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_url_or_path ), pool );

    std::string type_error_message;
    try
    {
        type_error_message = "expecting boolean for force keyword arg";
        bool force = args.getBoolean( name_force, true );

        try
        {
            checkThreadPermission();

            PythonAllowThreads permission( m_context );

            svn_error_t *error = svn_client_unlock
                (
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
            // use callback error over ClientException
            m_context.checkForError( m_module.client_error );

            throw_client_error( e );
        }
    }
    catch( Py::TypeError & )
    {
        throw Py::TypeError( type_error_message );
    }

    return Py::None();
}
#endif

#ifdef PYSVN_HAS_CLIENT_UPDATE2
Py::Object pysvn_client::cmd_update( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_path },
    { false, name_recurse },
    { false, name_revision },
#ifdef PYSVN_HAS_CLIENT_UPDATE2
    { false, name_ignore_externals },
#endif
    { false, NULL }
    };
    FunctionArguments args( "update", args_desc, a_args, a_kws );
    args.check();

    SvnPool pool( m_context );

    apr_array_header_t *targets = targetsFromStringOrList( args.getArg( name_path ), pool );

    svn_opt_revision_t revision = args.getRevision( name_revision, svn_opt_revision_head );
    bool recurse = args.getBoolean( name_recurse, true );
#ifdef PYSVN_HAS_CLIENT_UPDATE2
    bool ignore_externals = args.getBoolean( name_recurse, false );
#endif
    apr_array_header_t *result_revs = NULL;

    try
    {
        checkThreadPermission();

        PythonAllowThreads permission( m_context );

#ifdef PYSVN_HAS_CLIENT_UPDATE2
        svn_error_t *error = svn_client_update2
            (
            &result_revs,
            targets,
            &revision,
            recurse,
            ignore_externals,
            m_context,
            pool
            );
#else
        svn_error_t *error = svn_client_update
            (
            &result_revs,
            targets,
            &revision,
            recurse,
            m_context,
            pool
            );
#endif
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return revnumListToObject( result_revs, pool );
}

#else
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, revnum ) );
}
#endif

Py::Object pysvn_client::helper_boolean_auth_set( FunctionArguments &a_args, const char *a_arg_name, const char *a_param_name )
{
    a_args.check();

    bool enable( a_args.getBoolean( a_arg_name ) );
    try
    {
        void *param = 0;
        if( !enable )
            param = (void *)"1";

        svn_auth_set_parameter
            (
            m_context.ctx()->auth_baton,
            a_param_name,
            param
            );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
}

Py::Object pysvn_client::helper_boolean_auth_get( FunctionArguments &a_args, const char *a_param_name )
{
    a_args.check();

    char *param = NULL;
    try
    {
        param = (char *)svn_auth_get_parameter
            (
            m_context.ctx()->auth_baton,
            a_param_name
            );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    bool not_set = param != NULL && param[0] == '1';
    if( not_set )
        return Py::Int( 0 );
    return Py::Int( 1 );
}

Py::Object pysvn_client::helper_string_auth_set
    (
    FunctionArguments &a_args,
    const char *a_arg_name,
    const char *a_param_name,
    std::string &ctx_str
    )
{
    a_args.check();

    const char *param = NULL;
    Py::Object param_obj( a_args.getArg( a_arg_name ) );
    if( !param_obj.is( Py::None() ) )
    {
        Py::String param_str( param_obj );
        ctx_str = param_str.as_std_string();
        param = ctx_str.c_str();
    }

    try
    {
        svn_auth_set_parameter
            (
            m_context.ctx()->auth_baton,
            a_param_name,
            param
            );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
}

Py::Object pysvn_client::helper_string_auth_get( FunctionArguments &a_args, const char *a_param_name )
{
    a_args.check();

    char *param = NULL;
    try
    {
        param = (char *)svn_auth_get_parameter
            (
            m_context.ctx()->auth_baton,
            a_param_name
            );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    if( param != NULL )
        return Py::String( param );

    return Py::None();
}

#if defined( PYSVN_HAS_WC_ADM_DIR )
Py::Object pysvn_client::get_adm_dir( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { false, NULL }
    };
    FunctionArguments args( "get_adm_dir", args_desc, a_args, a_kws );
    args.check();

    const char *name = NULL;

    try
    {
        name = svn_wc_get_adm_dir
            (
            m_context.getContextPool()
            );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::String( name );
}
#endif

Py::Object pysvn_client::get_auth_cache( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { false, NULL }
    };
    FunctionArguments args( "get_auth_cache", args_desc, a_args, a_kws );

    return helper_boolean_auth_get( args, SVN_AUTH_PARAM_NO_AUTH_CACHE );
}

Py::Object pysvn_client::get_interactive( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { false, NULL }
    };
    FunctionArguments args( "get_interactive", args_desc, a_args, a_kws );

    return helper_boolean_auth_get( args, SVN_AUTH_PARAM_NON_INTERACTIVE );
}

Py::Object pysvn_client::get_store_passwords( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { false, NULL }
    };
    FunctionArguments args( "get_store_passwords", args_desc, a_args, a_kws );

    return helper_boolean_auth_get( args, SVN_AUTH_PARAM_DONT_STORE_PASSWORDS );
}

Py::Object pysvn_client::get_default_username( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { false, NULL }
    };
    FunctionArguments args( "get_default_username", args_desc, a_args, a_kws );

    return helper_string_auth_get( args, SVN_AUTH_PARAM_DEFAULT_USERNAME );
}

Py::Object pysvn_client::get_default_password( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { false, NULL }
    };
    FunctionArguments args( "get_default_password", args_desc, a_args, a_kws );

    return helper_string_auth_get( args, SVN_AUTH_PARAM_DEFAULT_PASSWORD );
}

#if defined( PYSVN_HAS_WC_ADM_DIR )
Py::Object pysvn_client::set_adm_dir( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_name },
    { false, NULL }
    };
    FunctionArguments args( "set_adm_dir", args_desc, a_args, a_kws );

    args.check();

    std::string name( args.getString( name_name ) );

    try
    {
        svn_wc_set_adm_dir
            (
            name.c_str(),
            m_context.getContextPool()
            );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
}
#endif

Py::Object pysvn_client::set_auth_cache( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_enable },
    { false, NULL }
    };
    FunctionArguments args( "set_auth_cache", args_desc, a_args, a_kws );

    return helper_boolean_auth_set( args, name_enable, SVN_AUTH_PARAM_NO_AUTH_CACHE );
}

Py::Object pysvn_client::set_interactive( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_enable },
    { false, NULL }
    };
    FunctionArguments args( "set_interactive", args_desc, a_args, a_kws );

    return helper_boolean_auth_set( args, name_enable, SVN_AUTH_PARAM_NON_INTERACTIVE );
}

Py::Object pysvn_client::set_store_passwords( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_enable },
    { false, NULL }
    };
    FunctionArguments args( "set_store_passwords", args_desc, a_args, a_kws );

    return helper_boolean_auth_set( args, name_enable, SVN_AUTH_PARAM_DONT_STORE_PASSWORDS );
}

Py::Object pysvn_client::set_default_username( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_username },
    { false, NULL }
    };
    FunctionArguments args( "set_default_username", args_desc, a_args, a_kws );

    return helper_string_auth_set( args, name_username, SVN_AUTH_PARAM_DEFAULT_USERNAME, m_context.m_default_username );
}

Py::Object pysvn_client::set_default_password( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_password },
    { false, NULL }
    };
    FunctionArguments args( "set_default_password", args_desc, a_args, a_kws );

    return helper_string_auth_set( args, name_password, SVN_AUTH_PARAM_DEFAULT_PASSWORD, m_context.m_default_password );
}

Py::Object pysvn_client::get_auto_props( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { false, NULL }
    };
    FunctionArguments args( "get_auto_props", args_desc, a_args, a_kws );
    args.check();

    svn_boolean_t enable = false;
    try
    {
        svn_config_t *cfg = (svn_config_t *)apr_hash_get
            (
            m_context.ctx()->config,
            SVN_CONFIG_CATEGORY_CONFIG,
            APR_HASH_KEY_STRING
            );
        svn_error_t *error = svn_config_get_bool
            (
            cfg,
            &enable,
            SVN_CONFIG_SECTION_MISCELLANY,
            SVN_CONFIG_OPTION_ENABLE_AUTO_PROPS,
            enable
            );
        if( error != NULL )
            throw SvnException( error );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::Int( enable );
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
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::None();
}

#if defined( PYSVN_HAS_WC_ADM_DIR )
Py::Object pysvn_client::is_adm_dir( const Py::Tuple &a_args, const Py::Dict &a_kws )
{
    static argument_description args_desc[] =
    {
    { true,  name_name },
    { false, NULL }
    };
    FunctionArguments args( "is_adm_dir", args_desc, a_args, a_kws );

    args.check();

    std::string name( args.getString( name_name ) );

    svn_boolean_t name_is_adm_dir = 0;
    try
    {
        name_is_adm_dir = svn_wc_is_adm_dir
            (
            name.c_str(),
            m_context.getContextPool()
            );
    }
    catch( SvnException &e )
    {
        // use callback error over ClientException
        m_context.checkForError( m_module.client_error );

        throw_client_error( e );
    }

    return Py::Int( name_is_adm_dir );
}
#endif

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

void pysvn_client::throw_client_error( SvnException &e )
{
    throw Py::Exception(
        m_module.client_error,
        e.pythonExceptionArg( m_exception_style ) );
}

void pysvn_client::init_type()
{
    behaviors().name("Client");
    behaviors().doc( pysvn_client_doc );
    behaviors().supportGetattr();
    behaviors().supportSetattr();

    add_keyword_method("add", &pysvn_client::cmd_add, pysvn_client_add_doc );
    add_keyword_method("annotate", &pysvn_client::cmd_annotate, pysvn_client_annotate_doc );
    add_keyword_method("cat", &pysvn_client::cmd_cat, pysvn_client_cat_doc );
    add_keyword_method("checkin", &pysvn_client::cmd_checkin, pysvn_client_checkin_doc );
    add_keyword_method("checkout", &pysvn_client::cmd_checkout, pysvn_client_checkout_doc );
    add_keyword_method("cleanup", &pysvn_client::cmd_cleanup, pysvn_client_cleanup_doc );
    add_keyword_method("copy", &pysvn_client::cmd_copy, pysvn_client_copy_doc );
    add_keyword_method("diff", &pysvn_client::cmd_diff, pysvn_client_diff_doc );
    add_keyword_method("export", &pysvn_client::cmd_export, pysvn_client_export_doc );
#if defined( PYSVN_HAS_WC_ADM_DIR )
    add_keyword_method("get_adm_dir", &pysvn_client::get_adm_dir, pysvn_client_get_adm_dir_doc );
#endif
    add_keyword_method("get_auth_cache", &pysvn_client::get_auth_cache, pysvn_client_get_auth_cache_doc );
    add_keyword_method("get_auto_props", &pysvn_client::get_auto_props, pysvn_client_get_auto_props_doc );
    add_keyword_method("get_default_password", &pysvn_client::get_default_password, pysvn_client_get_default_password_doc );
    add_keyword_method("get_default_username", &pysvn_client::get_default_username, pysvn_client_get_default_username_doc );
    add_keyword_method("get_interactive", &pysvn_client::get_interactive, pysvn_client_get_interactive_doc );
    add_keyword_method("get_store_passwords", &pysvn_client::get_store_passwords, pysvn_client_get_store_passwords_doc );
    add_keyword_method("import_", &pysvn_client::cmd_import, pysvn_client_import__doc );
    add_keyword_method("info", &pysvn_client::cmd_info, pysvn_client_info_doc );

#ifdef PYSVN_HAS_CLIENT_INFO
    add_keyword_method("info2", &pysvn_client::cmd_info2, pysvn_client_info2_doc );
#endif
#if defined( PYSVN_HAS_WC_ADM_DIR )
    add_keyword_method("is_adm_dir", &pysvn_client::is_adm_dir, pysvn_client_is_adm_dir_doc );
#endif
    add_keyword_method("is_url", &pysvn_client::is_url, pysvn_client_is_url_doc );
#ifdef PYSVN_HAS_CLIENT_LOCK
    add_keyword_method("lock", &pysvn_client::cmd_lock, pysvn_client_lock_doc );
#endif
    add_keyword_method("log", &pysvn_client::cmd_log, pysvn_client_log_doc );
#if defined( PYSVN_HAS_CLIENT_LIST )
    add_keyword_method("list", &pysvn_client::cmd_list, pysvn_client_list_doc );
#endif
    add_keyword_method("ls", &pysvn_client::cmd_ls, pysvn_client_ls_doc );
    add_keyword_method("merge", &pysvn_client::cmd_merge, pysvn_client_merge_doc );
    add_keyword_method("mkdir", &pysvn_client::cmd_mkdir, pysvn_client_mkdir_doc );
    add_keyword_method("move", &pysvn_client::cmd_move, pysvn_client_move_doc );
    add_keyword_method("propdel", &pysvn_client::cmd_propdel, pysvn_client_propdel_doc );
    add_keyword_method("propget", &pysvn_client::cmd_propget, pysvn_client_propget_doc );
    add_keyword_method("proplist", &pysvn_client::cmd_proplist, pysvn_client_proplist_doc );
    add_keyword_method("propset", &pysvn_client::cmd_propset, pysvn_client_propset_doc );
    add_keyword_method("relocate", &pysvn_client::cmd_relocate, pysvn_client_relocate_doc );
    add_keyword_method("remove", &pysvn_client::cmd_remove, pysvn_client_remove_doc );
    add_keyword_method("resolved", &pysvn_client::cmd_resolved, pysvn_client_resolved_doc );
    add_keyword_method("revert", &pysvn_client::cmd_revert, pysvn_client_revert_doc );
    add_keyword_method("revpropdel", &pysvn_client::cmd_revpropdel, pysvn_client_revpropdel_doc );
    add_keyword_method("revpropget", &pysvn_client::cmd_revpropget, pysvn_client_revpropget_doc );
    add_keyword_method("revproplist", &pysvn_client::cmd_revproplist, pysvn_client_revproplist_doc );
    add_keyword_method("revpropset", &pysvn_client::cmd_revpropset, pysvn_client_revpropset_doc );
#if defined( PYSVN_HAS_WC_ADM_DIR )
    add_keyword_method("set_adm_dir", &pysvn_client::set_adm_dir, pysvn_client_set_adm_dir_doc );
#endif
    add_keyword_method("set_auth_cache", &pysvn_client::set_auth_cache, pysvn_client_set_auth_cache_doc );
    add_keyword_method("set_auto_props", &pysvn_client::set_auto_props, pysvn_client_set_auto_props_doc );
    add_keyword_method("set_default_password", &pysvn_client::set_default_password, pysvn_client_set_default_password_doc );
    add_keyword_method("set_default_username", &pysvn_client::set_default_username, pysvn_client_set_default_username_doc );
    add_keyword_method("set_interactive", &pysvn_client::set_interactive, pysvn_client_set_interactive_doc );
    add_keyword_method("set_store_passwords", &pysvn_client::set_store_passwords, pysvn_client_set_store_passwords_doc );
    add_keyword_method("status", &pysvn_client::cmd_status, pysvn_client_status_doc );
    add_keyword_method("switch", &pysvn_client::cmd_switch, pysvn_client_switch_doc );
#ifdef PYSVN_HAS_CLIENT_LOCK
    add_keyword_method("unlock", &pysvn_client::cmd_unlock, pysvn_client_unlock_doc );
#endif
    add_keyword_method("update", &pysvn_client::cmd_update, pysvn_client_update_doc );
}

//--------------------------------------------------------------------------------
template <> void pysvn_enum< svn_opt_revision_kind >::init_type(void)
{
    behaviors().name("opt_revision_kind");
    behaviors().doc("opt_revision_kind enumeration");
    behaviors().supportGetattr();
}

template <> void pysvn_enum_value< svn_opt_revision_kind >::init_type(void)
{
    behaviors().name("opt_revision_kind");
    behaviors().doc("opt_revision_kind value");
    behaviors().supportCompare();
    behaviors().supportRepr();
    behaviors().supportStr();
    behaviors().supportHash();
}

template <> void pysvn_enum< svn_wc_notify_action_t >::init_type(void)
{
    behaviors().name("wc_notify_action");
    behaviors().doc("wc_notify_action enumeration");
    behaviors().supportGetattr();
}

template <> void pysvn_enum_value< svn_wc_notify_action_t >::init_type(void)
{
    behaviors().name("wc_notify_action");
    behaviors().doc("wc_notify_action value");
    behaviors().supportCompare();
    behaviors().supportRepr();
    behaviors().supportStr();
    behaviors().supportHash();
}

template <> void pysvn_enum< svn_wc_status_kind >::init_type(void)
{
    behaviors().name("wc_status_kind");
    behaviors().doc("wc_status_kind enumeration");
    behaviors().supportGetattr();
}

template <> void pysvn_enum_value< svn_wc_status_kind >::init_type(void)
{
    behaviors().name("wc_status_kind");
    behaviors().doc("wc_status_kind value");
    behaviors().supportCompare();
    behaviors().supportRepr();
    behaviors().supportStr();
    behaviors().supportHash();
}

template <> void pysvn_enum< svn_wc_schedule_t >::init_type(void)
{
    behaviors().name("wc_schedule");
    behaviors().doc("wc_schedule enumeration");
    behaviors().supportGetattr();
}

template <> void pysvn_enum_value< svn_wc_schedule_t >::init_type(void)
{
    behaviors().name("wc_schedule");
    behaviors().doc("wc_schedule value");
    behaviors().supportCompare();
    behaviors().supportRepr();
    behaviors().supportStr();
    behaviors().supportHash();
}

template <> void pysvn_enum< svn_wc_merge_outcome_t >::init_type(void)
{
    behaviors().name("wc_merge_outcome");
    behaviors().doc("wc_merge_outcome enumeration");
    behaviors().supportGetattr();
}

template <> void pysvn_enum_value< svn_wc_merge_outcome_t >::init_type(void)
{
    behaviors().name("wc_merge_outcome");
    behaviors().doc("wc_merge_outcome value");
    behaviors().supportCompare();
    behaviors().supportRepr();
    behaviors().supportStr();
    behaviors().supportHash();
}

template <> void pysvn_enum< svn_wc_notify_state_t >::init_type(void)
{
    behaviors().name("wc_notify_state");
    behaviors().doc("wc_notify_state enumeration");
    behaviors().supportGetattr();
}

template <> void pysvn_enum_value< svn_wc_notify_state_t >::init_type(void)
{
    behaviors().name("wc_notify_state");
    behaviors().doc("wc_notify_state value");
    behaviors().supportCompare();
    behaviors().supportRepr();
    behaviors().supportStr();
    behaviors().supportHash();
}

template <> void pysvn_enum< svn_node_kind_t >::init_type(void)
{
    behaviors().name("node_kind");
    behaviors().doc("node_kind enumeration");
    behaviors().supportGetattr();
}

template <> void pysvn_enum_value< svn_node_kind_t >::init_type(void)
{
    behaviors().name("node_kind");
    behaviors().doc("node_kind value");
    behaviors().supportCompare();
    behaviors().supportRepr();
    behaviors().supportStr();
    behaviors().supportHash();
}

#if defined( PYSVN_HAS_DIFF_FILE_IGNORE_SPACE )
template <> void pysvn_enum< svn_diff_file_ignore_space_t >::init_type(void)
{
    behaviors().name("diff_file_ignore_space");
    behaviors().doc("diff_file_ignore_space enumeration");
    behaviors().supportGetattr();
}

template <> void pysvn_enum_value< svn_diff_file_ignore_space_t >::init_type(void)
{
    behaviors().name("diff_file_ignore_space");
    behaviors().doc("diff_file_ignore_space value");
    behaviors().supportCompare();
    behaviors().supportRepr();
    behaviors().supportStr();
    behaviors().supportHash();
}
#endif
