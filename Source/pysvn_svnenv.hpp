#ifndef __PYSVN_SVNENV__
#define __PYSVN_SVNENV__

#include <svn_client.h>
#include <svn_fs.h>
#include <svn_repos.h>
#include <string>

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 1) || SVN_VER_MAJOR > 1
#define PYSVN_HAS_CLIENT_ADD2
#define PYSVN_HAS_CLIENT_EXPORT2
#endif

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 2) || SVN_VER_MAJOR > 1
#define PYSVN_HAS_CLIENT_INFO
#define PYSVN_HAS_CLIENT_LOCK
#define PYSVN_HAS_CLIENT_CHECKOUT2
#define PYSVN_HAS_CONTEXT_NOTIFY2
#endif

#include "CXX/Objects.hxx"

#ifndef PYCXX_MAKEVERSION
#error PyCXX version 5.3.3 is required
#endif

#if PYCXX_VERSION < PYCXX_MAKEVERSION( 5, 3, 3 )
#error PyCXX version 5.3.3 is required
#endif


class SvnPool;
class SvnContext;
class SvnTransaction;

class SvnException
{
public:
    SvnException( svn_error_t *error );
    SvnException( const SvnException &other );

    virtual ~SvnException();

    // access methods
    Py::String &message();
    Py::Object &pythonExceptionArg( int style );
    apr_status_t code();

private:
    int                 m_code;
    Py::String          m_message;
    Py::Object          m_exception_arg;

private:
    SvnException();
    SvnException &operator=( const SvnException & );
};


class SvnPool
{
public:
    SvnPool( SvnContext &ctx );
    SvnPool( SvnTransaction &txn );
    ~SvnPool();

    operator apr_pool_t *() const;

private:
    apr_pool_t *m_pool;
};

class SvnContext
{
public:
    SvnContext( const std::string &config_dir="" );
    virtual ~SvnContext();

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
#ifdef PYSVN_HAS_CONTEXT_NOTIFY2
    virtual void contextNotify2
        (
        const svn_wc_notify_t *notify,
        apr_pool_t *pool
        ) = 0;
#else
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
#endif

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

#ifdef PYSVN_HAS_CONTEXT_NOTIFY2
    static void SvnContext::handlerNotify2
        (
        void * baton,
	const svn_wc_notify_t *notify,
	apr_pool_t *pool        
        );
#else
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
#endif
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

class SvnTransaction
{
public:
    SvnTransaction();
    ~SvnTransaction();

    svn_error_t *init( const std::string &repos_path, const std::string &transaction );

    operator svn_fs_txn_t *();
    svn_fs_txn_t *transaction();
    operator svn_fs_t *();
    operator svn_repos_t *();

private:
    apr_pool_t          *m_pool;
    svn_repos_t         *m_repos;
    svn_fs_t            *m_fs;
    svn_fs_txn_t        *m_txn;
    char                *m_txn_name;
};

#endif // __PYSVN_SVNENV__
