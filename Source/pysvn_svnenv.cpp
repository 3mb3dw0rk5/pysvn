#include "pysvn_svnenv.hpp"
#include "svn_config.h"
#include "svn_pools.h"

//--------------------------------------------------------------------------------
//
//	SvnException
//
//--------------------------------------------------------------------------------
SvnException::SvnException( svn_error_t *error )
: m_code( error->apr_err )
, m_message( error->message )
{ }

SvnException::SvnException( const SvnException &other )
: m_code( other.m_code )
, m_message( other.m_message )
{ }

SvnException::~SvnException()
{ }

const std::string &SvnException::message() const
{
    return m_message;
}

apr_status_t SvnException::code() const
{
    return m_code;
}

//--------------------------------------------------------------------------------
//
//	SvnContext
//
//--------------------------------------------------------------------------------
SvnContext::SvnContext( const std::string &config_dir_str )
: m_pool( NULL )
{
    apr_pool_create( &m_pool, NULL );

    const char *config_dir = NULL;
    if( !config_dir_str.empty() )
	config_dir = config_dir_str.c_str();

    svn_config_ensure( config_dir, m_pool );

    apr_array_header_t *providers = apr_array_make( m_pool, 8, sizeof( svn_auth_provider_object_t * ) );

    // simple providers
    svn_auth_provider_object_t *provider = NULL;
    svn_client_get_simple_provider( &provider, m_pool );
    *(svn_auth_provider_object_t **)apr_array_push( providers ) = provider;

    svn_client_get_username_provider( &provider, m_pool );
    *(svn_auth_provider_object_t **)apr_array_push( providers ) = provider;

    svn_client_get_simple_prompt_provider( &provider, handlerSimplePrompt, this, 1000000, m_pool );
    *(svn_auth_provider_object_t **)apr_array_push( providers ) = provider;

    // ssl providers

    // order is important - file first then prompt providers
    svn_client_get_ssl_server_trust_file_provider( &provider, m_pool );
    *(svn_auth_provider_object_t **)apr_array_push( providers ) = provider;

    svn_client_get_ssl_client_cert_file_provider( &provider, m_pool );
    *(svn_auth_provider_object_t **)apr_array_push( providers ) = provider;

    svn_client_get_ssl_client_cert_pw_file_provider( &provider, m_pool );
    *(svn_auth_provider_object_t **)apr_array_push( providers ) = provider;

    svn_client_get_ssl_server_trust_prompt_provider( &provider, handlerSslServerTrustPrompt, this, m_pool );
    *(svn_auth_provider_object_t **)apr_array_push (providers) = provider;

    svn_client_get_ssl_client_cert_pw_prompt_provider( &provider, handlerSslClientCertPwPrompt, this, 3, m_pool );
    *(svn_auth_provider_object_t **)apr_array_push (providers) = provider;

    svn_auth_baton_t *auth_baton = NULL;
    svn_auth_open( &auth_baton, providers, m_pool );

    // get the config based on the config dir passed in
    svn_config_get_config( &m_context.config, config_dir, m_pool );

    // tell the auth functions where the config dir is
    svn_auth_set_parameter( auth_baton, SVN_AUTH_PARAM_CONFIG_DIR, config_dir );

    m_context.auth_baton = auth_baton;
    m_context.log_msg_func = handlerLogMsg;
    m_context.log_msg_baton = this;
    m_context.notify_func = handlerNotify;
    m_context.notify_baton = this;
    m_context.cancel_func = handlerCancel;
    m_context.cancel_baton = this;
}

SvnContext::~SvnContext()
{
}

SvnContext::operator svn_client_ctx_t *()
{
    return &m_context;
}

svn_client_ctx_t *SvnContext::ctx()
{
    return &m_context;
}


svn_error_t *SvnContext::handlerLogMsg
    (
    const char **log_msg,
    const char **tmp_file,
    apr_array_header_t *commit_items,
    void *baton,
    apr_pool_t *pool
    )
{
    SvnContext *context = reinterpret_cast<SvnContext *>( baton );

    std::string msg;

    if (!context->contextGetLogMessage( msg ) )
        return svn_error_create( SVN_ERR_CANCELLED, NULL, "" );

    *log_msg = svn_string_ncreate( msg.data(), msg.length(), pool )->data;
    *tmp_file = NULL;

    return SVN_NO_ERROR;
}

void SvnContext::handlerNotify
    (
    void * baton,
    const char *path,
    svn_wc_notify_action_t action,
    svn_node_kind_t kind,
    const char *mime_type,
    svn_wc_notify_state_t content_state,
    svn_wc_notify_state_t prop_state,
    svn_revnum_t revision
    )
{
    SvnContext *context = reinterpret_cast<SvnContext *>( baton );

    context->contextNotify( path, action, kind, mime_type, content_state, prop_state, revision );
}

svn_error_t *SvnContext::handlerCancel
    (
    void * baton
    )
{
    SvnContext *context = reinterpret_cast<SvnContext *>( baton );

    if( context->contextCancel() )
        return svn_error_create( SVN_ERR_CANCELLED, NULL, "cancelled by user" );
    else
        return SVN_NO_ERROR;
}

svn_error_t *SvnContext::handlerSimplePrompt
    (
    svn_auth_cred_simple_t **cred,
    void *baton,
    const char *a_realm,
    const char *a_username, 
    svn_boolean_t a_may_save,
    apr_pool_t *pool
    )
{
    SvnContext *context = reinterpret_cast<SvnContext *>( baton );

    bool may_save = a_may_save != 0;

    std::string realm( a_realm );
    std::string username( a_username );
    std::string password;

    if( !context->contextGetLogin( realm, username, password, may_save ) )
        return svn_error_create( SVN_ERR_CANCELLED, NULL, "" );

    svn_auth_cred_simple_t *lcred = (svn_auth_cred_simple_t *)apr_palloc( pool, sizeof( svn_auth_cred_simple_t ) );
    lcred->username = svn_string_ncreate( username.data(), username.length(), pool )->data;
    lcred->password = svn_string_ncreate( password.data(), password.length(), pool )->data;

    // tell svn if the credentials need to be saved
    lcred->may_save = may_save;
    *cred = lcred;

    return SVN_NO_ERROR;
}

svn_error_t *SvnContext::handlerSslServerTrustPrompt 
    (
    svn_auth_cred_ssl_server_trust_t **cred, 
    void *baton,
    const char *realm,
    apr_uint32_t failures,
    const svn_auth_ssl_server_cert_info_t *info,
    svn_boolean_t may_save,
    apr_pool_t *pool
    )
{
    SvnContext *context = reinterpret_cast<SvnContext *>( baton );

    apr_uint32_t accepted_failures = failures;
    bool accept_permanently = true;

    if( !context->contextSslServerTrustPrompt( *info, realm, accepted_failures, accept_permanently ) )
    {
        *cred = NULL;

        return SVN_NO_ERROR;
    }

    svn_auth_cred_ssl_server_trust_t *new_cred = (svn_auth_cred_ssl_server_trust_t *)
            apr_palloc( pool, sizeof (svn_auth_cred_ssl_server_trust_t) );

    if( accept_permanently )
    {
        new_cred->may_save = 1;
        new_cred->accepted_failures = accepted_failures;
    }

    *cred = new_cred;

    return SVN_NO_ERROR;
}

svn_error_t *SvnContext::handlerSslClientCertPrompt 
    (
    svn_auth_cred_ssl_client_cert_t **cred, 
    void *baton, 
    apr_pool_t *pool
    )
{
    SvnContext *context = reinterpret_cast<SvnContext *>( baton );

    std::string cert_file;
    if( !context->contextSslClientCertPrompt( cert_file ) )
        return svn_error_create (SVN_ERR_CANCELLED, NULL, "");

    svn_auth_cred_ssl_client_cert_t *new_cred = (svn_auth_cred_ssl_client_cert_t*)
        apr_palloc (pool, sizeof (svn_auth_cred_ssl_client_cert_t));

    new_cred->cert_file = svn_string_ncreate( cert_file.data(), cert_file.length(), pool )->data;

    *cred = new_cred;

    return SVN_NO_ERROR;
}

svn_error_t *SvnContext::handlerSslClientCertPwPrompt
    (
    svn_auth_cred_ssl_client_cert_pw_t **cred, 
    void *baton, 
    const char *realm,
    svn_boolean_t maySave,
    apr_pool_t *pool
    )
{
    SvnContext *context = reinterpret_cast<SvnContext *>( baton );

    std::string password;
    bool may_save = maySave != 0;
    if( !context->contextSslClientCertPwPrompt( password, realm, may_save ) )
        return svn_error_create( SVN_ERR_CANCELLED, NULL, "" );

    svn_auth_cred_ssl_client_cert_pw_t *new_cred = (svn_auth_cred_ssl_client_cert_pw_t *)
        apr_palloc (pool, sizeof (svn_auth_cred_ssl_client_cert_pw_t));

    new_cred->password = svn_string_ncreate( password.data(), password.length(), pool )->data;

    new_cred->may_save = may_save;
    *cred = new_cred;

    return SVN_NO_ERROR;
}


//--------------------------------------------------------------------------------
//
//	Pool
//
//--------------------------------------------------------------------------------
SvnPool::SvnPool( SvnContext &ctx )
: m_pool( NULL )
{
    m_pool = svn_pool_create( NULL );
}

SvnPool::~SvnPool()
{
    if( m_pool != NULL )
    {
	svn_pool_destroy( m_pool );
    }
}

SvnPool::operator apr_pool_t *() const 
{
    return m_pool;
}


