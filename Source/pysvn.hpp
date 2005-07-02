//
// ====================================================================
// Copyright (c) 2003-2005 Barry A Scott.  All rights reserved.
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

#include "pysvn_svnenv.hpp"

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
    Py::Object new_client( const Py::Tuple &a_args, const Py::Dict &a_kws );
    Py::Object new_transaction( const Py::Tuple &a_args, const Py::Dict &a_kws );
    Py::Object new_revision( const Py::Tuple &a_args, const Py::Dict &a_kws );

public:// variables
    Py::ExtensionExceptionType client_error;
};


//--------------------------------------------------------------------------------
class PythonAllowThreads;

class pysvn_context : public SvnContext
{
public:
    pysvn_context( const std::string &config_dir );
    virtual ~pysvn_context();

    // return true if the callbacks are being used on some thread
    bool hasPermission();
    // give the callbacks permission to run
    void setPermission( PythonAllowThreads &_permission );
    // revoke permission
    void clearPermission();

    void checkForError( Py::ExtensionExceptionType &exception_for_error );

public:    // data

    //
    // Python class that has implements the callback methods
    //
    Py::Object m_pyfn_GetLogin;
    Py::Object m_pyfn_Notify;
    Py::Object m_pyfn_Cancel;
    Py::Object m_pyfn_GetLogMessage;
    Py::Object m_pyfn_SslServerPrompt;
    Py::Object m_pyfn_SslServerTrustPrompt;
    Py::Object m_pyfn_SslClientCertPrompt;
    Py::Object m_pyfn_SslClientCertPwPrompt;

    void setLogMessage( const std::string &message );

private:// methods

    //
    // this method will be called to retrieve
    // authentication information
    //
    bool contextGetLogin 
        (
        const std::string & realm,
        std::string & username, 
        std::string & password,
        bool &may_save
        );

    // 
    // this method will be called to notify about
    // the progress of an ongoing action
    //
#ifdef PYSVN_HAS_CONTEXT_NOTIFY2
    void contextNotify2
        (
        const svn_wc_notify_t *notify,
        apr_pool_t *pool
        );
#else
    void contextNotify
        (
        const char *path,
        svn_wc_notify_action_t action,
        svn_node_kind_t kind,
        const char *mime_type,
        svn_wc_notify_state_t content_state,
        svn_wc_notify_state_t prop_state,
        svn_revnum_t revision
        );
#endif

    //
    // this method will be called periodically to allow
    // the app to cancel long running operations
    //
    // @return cancel action?
    // @retval true cancel
    //
    bool contextCancel();

    //
    // this method will be called to retrieve
    // a log message
    //
    bool contextGetLogMessage( std::string & msg );

    //
    // this method is called if there is ssl server
    // information, that has to be confirmed by the user
    //
    // @param data 
    // @return @a SslServerTrustAnswer
    //
    bool contextSslServerTrustPrompt
        (
        const svn_auth_ssl_server_cert_info_t &info, 
        const std::string &realm,
        apr_uint32_t &a_accepted_failures,
        bool &accept_permanent
        );
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
    bool contextSslClientCertPwPrompt
        (
        std::string & password,
        const std::string &realm,
        bool &may_save
        );

private:// vaiables

    PythonAllowThreads    *m_permission;
    std::string        m_error_message;
    std::string        m_log_message;
};

class pysvn_client : public Py::PythonExtension<pysvn_client>
{
public:
    pysvn_client( pysvn_module &module, const std::string &config_dir );
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
    Py::Object cmd_cleanup( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_checkin( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_copy( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_diff( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_export( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_info( const Py::Tuple& args, const Py::Dict &kws );
#ifdef PYSVN_HAS_CLIENT_INFO
    Py::Object cmd_info2( const Py::Tuple& args, const Py::Dict &kws );
#endif
    Py::Object cmd_import( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_log( const Py::Tuple& args, const Py::Dict &kws );
#ifdef PYSVN_HAS_CLIENT_LOCK
    Py::Object cmd_lock( const Py::Tuple& args, const Py::Dict &kws );
#endif
    Py::Object cmd_ls( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_merge( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_mkdir( const Py::Tuple& args, const Py::Dict &kws );
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
#ifdef PYSVN_HAS_CLIENT_UNLOCK
    Py::Object cmd_unlock( const Py::Tuple& args, const Py::Dict &kws );
#endif
    Py::Object cmd_update( const Py::Tuple& args, const Py::Dict &kws );

    // SVN commands
    Py::Object is_url( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object get_auth_cache( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object set_auth_cache( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object get_auto_props( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object set_auto_props( const Py::Tuple& args, const Py::Dict &kws );

    // check that we are not in use on another thread
    void checkThreadPermission();
private:
    // helper function that wraps up the logic to throw a client error exception
    void throw_client_error( SvnException & );
private:
    pysvn_module    &m_module;
    pysvn_context    m_context;
    int        m_exception_style;
};

class pysvn_transaction : public Py::PythonExtension<pysvn_transaction>
{
public:
    pysvn_transaction( pysvn_module &module,
        const std::string &repos_path,
        const std::string &transaction_name );
    virtual ~pysvn_transaction();

    // override functions from PythonExtension
    virtual Py::Object getattr( const char *name );
    virtual int setattr( const char *name, const Py::Object &value );
    static void init_type( void );

    // SVN Transaction commands
    Py::Object cmd_cat( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_changed( const Py::Tuple& args, const Py::Dict &kws );

#if 0
    Py::Object cmd_diff( const Py::Tuple& args, const Py::Dict &kws );
#endif

    Py::Object cmd_propdel( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_propget( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_proplist( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_propset( const Py::Tuple& args, const Py::Dict &kws );

    Py::Object cmd_revpropdel( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_revpropget( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_revproplist( const Py::Tuple& args, const Py::Dict &kws );
    Py::Object cmd_revpropset( const Py::Tuple& args, const Py::Dict &kws );

    // check that we are not in use on another thread
    void checkThreadPermission();
private:
    // helper function that wraps up the logic to throw a client error exception
    void throw_client_error( SvnException & );
private:
    pysvn_module            &m_module;
    SvnTransaction          m_transaction;
    int                     m_exception_style;
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
    pysvn_revision();    // not defined
    svn_opt_revision_t m_svn_revision;
};

class pysvn_status : public Py::PythonExtension<pysvn_status>
{
public:
    pysvn_status( const char *path, svn_wc_status_t *svn_status, SvnContext &context );

    virtual ~pysvn_status();

    virtual Py::Object getattr( const char *name );

    static void init_type(void);
private:
    SvnContext              &m_context;
    SvnPool                 m_pool;
    std::string             m_path;
    const svn_wc_status_t   *m_svn_status;
};

class pysvn_entry : public Py::PythonExtension<pysvn_entry>
{
public:
    pysvn_entry( const svn_wc_entry_t *svn_entry, SvnContext &context );

    virtual ~pysvn_entry();

    virtual Py::Object getattr( const char *name );

    static void init_type(void);
private:
    SvnPool m_pool;
    const svn_wc_entry_t *m_svn_entry;
};

// help functions to check the arguments pass and throw AttributeError
struct argument_description
{
    bool m_required;            // true if this argument must be present
    const char *m_arg_name;     // the name of this arg to lookup in the keywords dictionary
};

class FunctionArguments
{
public:
    FunctionArguments
        (
        const char *function_name,
        const argument_description *arg_info,
        const Py::Tuple &args,
        const Py::Dict &kws
        );
    ~FunctionArguments();
    void check();

    bool hasArg( const char *arg_name );
    Py::Object getArg( const char *arg_name );

    bool getBoolean( const char *name );
    bool getBoolean( const char *name, bool default_value );
    std::string getUtf8String( const char *name );
    std::string getUtf8String( const char *name, const std::string &default_value );
    std::string getString( const char *name, const std::string &default_value );
    std::string getString( const char *name );
    svn_opt_revision_t getRevision( const char *name );
    svn_opt_revision_t getRevision( const char *name, svn_opt_revision_kind default_value );

private:
    const std::string           m_function_name;
    const argument_description  *m_arg_desc;
    const Py::Tuple             &m_args;
    const Py::Dict              &m_kws;

    Py::Dict                    m_checked_args;
    int                         m_min_args;
    int                         m_max_args;
};

extern Py::Object toObject( apr_time_t time );

//------------------------------------------------------------
template<TEMPLATE_TYPENAME T>
class EnumString
{
public:
    EnumString();
    ~EnumString() {}

    const std::string &toTypeName( T )
    {
        return m_type_name;
    }

    const std::string &toString( T value )
    {
        static std::string not_found( "-unknown-" );
        EXPLICIT_TYPENAME std::map<T,std::string>::iterator it = m_enum_to_string.find( value );
        if( it != m_enum_to_string.end() )
            return (*it).second;

        return not_found;
    }

    bool toEnum( const std::string &string, T &value )
    {
        EXPLICIT_TYPENAME std::map<std::string,T>::iterator it = m_string_to_enum.find( string );
        if( it != m_string_to_enum.end() )
        {
            value = (*it).second;
            return true;
        }

        return false;
    }

    EXPLICIT_TYPENAME std::map<std::string,T>::iterator begin()
    {
        return m_string_to_enum.begin();
    }

    EXPLICIT_TYPENAME std::map<std::string,T>::iterator end()
    {
        return m_string_to_enum.end();
    }

private:
    void add( T value, std::string string )
    {
        m_string_to_enum[string] = value;
        m_enum_to_string[value] = string;
    }
 
    std::string             m_type_name;
    std::map<std::string,T> m_string_to_enum;
    std::map<T,std::string> m_enum_to_string;
};

template<TEMPLATE_TYPENAME T>
const std::string &toTypeName( T value )
{
    static EnumString< T > enum_map;

    return enum_map.toTypeName( value );
}

template<TEMPLATE_TYPENAME T>
const std::string &toString( T value )
{
    static EnumString< T > enum_map;

    return enum_map.toString( value );
}

template<TEMPLATE_TYPENAME T>
bool toEnum( const std::string &string, T &value )
{
    static EnumString< T > enum_map;

    return enum_map.toEnum( string, value );
}

template<TEMPLATE_TYPENAME T>
Py::List memberList( T value )
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
class pysvn_enum_value : public Py::PythonExtension< EXPLICIT_CLASS pysvn_enum_value<T> >
{
public:
    pysvn_enum_value( T _value)
        : Py::PythonExtension<pysvn_enum_value>()
        , m_value( _value )
    { }

    virtual ~pysvn_enum_value()
    { }

    virtual int compare( const Py::Object &other )
    {
        if( pysvn_enum_value::check( other ) )
        {
            pysvn_enum_value<T> *other_value = static_cast<pysvn_enum_value *>( other.ptr() );
            if( m_value == other_value->m_value )
                return 0;

            if( m_value > other_value->m_value )
                return 1;
            else
                return -1;
        }
        else
        {
            std::string msg( "expecting " );
            msg += toTypeName( m_value );
            msg += " object for compare ";
            throw Py::AttributeError( msg );
        }
    }

    virtual Py::Object repr()
    {
        std::string s("<");
        s += toTypeName( m_value );
        s += ".";
        s += toString( m_value );
        s += ">";

        return Py::String( s );
    }

    virtual Py::Object str()
    {
        return Py::String( toString( m_value ) );
    }

    // need a hash so that the enums can go into a map
    virtual long hash()
    {
        static Py::String type_name( toTypeName( m_value ) );

        // use the m_value plus the hash of the type name
        return long( m_value ) + type_name.hashValue();
    }

    static void init_type(void);

public:
    T m_value;
};

//------------------------------------------------------------
template<TEMPLATE_TYPENAME T>
class pysvn_enum : public Py::PythonExtension< EXPLICIT_CLASS pysvn_enum<T> >
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

        return this->getattr_methods( _name );    
    }

    static void init_type(void);
};

template<TEMPLATE_TYPENAME T> Py::Object toEnumValue( const T &value )
{
    return Py::asObject( new pysvn_enum_value<T>( value ) );
}

// true if path exists and is a directory
extern bool is_path_dir( const std::string &path );

// true if the path_or_url is a svn url
extern bool is_svn_url( const std::string &path_or_url );

// convert a path to what SVN likes only if its not a URL
extern std::string svnNormalisedIfPath( const std::string &unnormalised, SvnPool &pool );

// convert a path to what the native OS likes
extern std::string osNormalisedPath( const std::string &unnormalised, SvnPool &pool );

// need a type to convert from apr_time_t to signed_int64 to double
#if defined(_MSC_VER)
typedef signed __int64 signed_int64;
#else
typedef signed long long signed_int64;
#endif


//--------------------------------------------------------------------------------
//
//    PythonAllowThreads provides a exception safe
//    wrapper for the C idiom:
//
//    Py_BEGIN_ALLOW_THREADS
//    ...Do some blocking I/O operation...
//    Py_END_ALLOW_THREADS
//
//    IN C++ use PythonAllowThreads in main code:
//{
//    PythonAllowThreads main_permission;
//    ...Do some blocking I/O operation that may throw
//} // allow d'tor grabs the lock
//
//    In C++ use PythonDisallowThreads in callback code:
//{
//    PythonDisallowThreads permission( main_permission );
//    ... Python operations that may throw
//} // allow d'tor to release the lock
//
//--------------------------------------------------------------------------------
class PythonAllowThreads
{
public:
    // calls allowOtherThreads()
    PythonAllowThreads( pysvn_context &_context );
    // calls allowThisThread() if necessary
    ~PythonAllowThreads();

    void allowOtherThreads();
    void allowThisThread();
private:
    pysvn_context            &m_callbacks;
    PyThreadState            *m_save;
};

class PythonDisallowThreads
{
public:
    // calls allowThisThread()
    PythonDisallowThreads( PythonAllowThreads *_permission );
    // calls allowOtherThreads() if necessary
    ~PythonDisallowThreads();
private:
    PythonAllowThreads *m_permission;
};

//--------------------------------------------------------------------------------
extern Py::Object utf8_string_or_none( const char *str );
extern Py::Object path_string_or_none( const char *str, SvnPool &pool );
extern Py::Object utf8_string_or_none( const std::string &str );
extern apr_time_t convertStringToTime( const std::string &text, apr_time_t now, SvnPool &pool );
extern Py::Object toObject( apr_time_t t );
extern Py::Object toObject( svn_client_commit_info_t *commit_info );
extern Py::Object propsToObject( apr_hash_t *props, SvnPool &pool );
extern void proplistToObject( Py::List &py_path_propmap_list, apr_array_header_t *props, SvnPool &pool );
extern Py::String asUtf8String( Py::Object obj );
extern apr_array_header_t *targetsFromStringOrList( Py::Object arg, SvnPool &pool );
extern Py::List toListOfStrings( Py::Object obj );
//--------------------------------------------------------------------------------
