#ifndef __PYSVN_SVNENV__
#define __PYSVN_SVNENV__

#include <svn_client.h>
#include <string>

#include "CXX/Objects.hxx"

class SvnPool;
class SvnContext;

class SvnException
{
public:
    SvnException( svn_error_t *error );
    SvnException( const SvnException &other );

    virtual ~SvnException();

    // access methods
    const std::string &message() const;
    Py::Object pythonExceptionArg();
    apr_status_t code() const;

private:
    apr_status_t	m_code;
    std::string         m_message;
    Py::Object          m_exception_arg;

private:
    SvnException();
    SvnException &operator=( const SvnException & );
};


class SvnPool
{
public:
    SvnPool( SvnContext &ctx );
    ~SvnPool();

    operator apr_pool_t *() const;

private:
    apr_pool_t *m_pool;
};

class SvnContext
{
public:
    SvnContext( const std::string &config_dir="" );
    ~SvnContext();

    operator svn_client_ctx_t *();
    svn_client_ctx_t *ctx();

    //
    // this method will be called to retrieve
    // authentication information
    //
    // WORKAROUND FOR apr_xlate PROBLEM: 
    // STRINGS ALREADY HAVE TO BE UTF8!!!
    //
    // @retval true continue
    //
    virtual bool contextGetLogin
        (
        const std::string & realm,
        std::string & username, 
        std::string & password,
        bool &may_save
        ) = 0;

    // 
    // this method will be called to notify about
    // the progress of an ongoing action
    //
    virtual void contextNotify
        (
        const char *path,
        svn_wc_notify_action_t action,
        svn_node_kind_t kind,
        const char *mime_type,
        svn_wc_notify_state_t content_state,
        svn_wc_notify_state_t prop_state,
        svn_revnum_t revision
        ) = 0;

    //
    // this method will be called periodically to allow
    // the app to cancel long running operations
    //
    // @return cancel action?
    // @retval true cancel
    //
    virtual bool contextCancel
        (
        ) = 0;

    //
    // this method will be called to retrieve
    // a log message
    //
    virtual bool contextGetLogMessage
        (
        std::string & msg
        ) = 0;

    //
    // this method is called if there is ssl server
    // information, that has to be confirmed by the user
    //
    // @param data 
    // @return @a SslServerTrustAnswer
    //
    virtual bool contextSslServerTrustPrompt
        (
        const svn_auth_ssl_server_cert_info_t &info, 
	const std::string &relam,
        apr_uint32_t & acceptedFailures,
        bool &accept_permanent
        ) = 0;

    //
    // this method is called to retrieve client side
    // information
    //
    virtual bool contextSslClientCertPrompt
        (
        std::string &certFile
        ) = 0;

    //
    // this method is called to retrieve the password
    // for the certificate
    //
    // @param password
    //
    virtual bool contextSslClientCertPwPrompt
        (
        std::string & password,
        const std::string &realm,
        bool &may_save
        ) = 0;

private:
    static svn_error_t *SvnContext::handlerLogMsg
        (
        const char **log_msg,
        const char **tmp_file,
        apr_array_header_t *commit_items,
        void *baton,
        apr_pool_t *pool
        );

    static void SvnContext::handlerNotify
        (
        void * baton,
        const char *path,
        svn_wc_notify_action_t action,
        svn_node_kind_t kind,
        const char *mime_type,
        svn_wc_notify_state_t content_state,
        svn_wc_notify_state_t prop_state,
        svn_revnum_t revision
        );

    static svn_error_t *SvnContext::handlerCancel
        (
        void * baton
        );

    static svn_error_t *handlerSimplePrompt
        (
        svn_auth_cred_simple_t **cred,
        void *baton,
        const char *realm,
        const char *username, 
        svn_boolean_t _may_save,
        apr_pool_t *pool
        );

    static svn_error_t *handlerSslServerTrustPrompt 
        (
        svn_auth_cred_ssl_server_trust_t **cred, 
        void *baton,
        const char *realm,
        apr_uint32_t failures,
        const svn_auth_ssl_server_cert_info_t *info,
        svn_boolean_t may_save,
        apr_pool_t *pool
        );

    static svn_error_t *handlerSslClientCertPrompt 
        (
        svn_auth_cred_ssl_client_cert_t **cred, 
        void *baton, 
        apr_pool_t *pool
        );

    static svn_error_t *handlerSslClientCertPwPrompt
        (
        svn_auth_cred_ssl_client_cert_pw_t **cred, 
        void *baton, 
        const char *realm,
        svn_boolean_t maySave,
        apr_pool_t *pool
        );


private:
    apr_pool_t          *m_pool;
    svn_client_ctx_t    m_context;
};

#endif // __PYSVN_SVNENV__
