#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "svn_config.h"
#include "svn_pools.h"
#include "svn_client.h"

static int elapse_time();

int main( int argc, char **argv )
{
    if( argc < 3 )
    {
        std::cout << "Usage: " << argv[0] << " <path-to-wc-file> <num-calls>" << std::endl;
        return 1;
    }
    apr_initialize();
    apr_pool_initialize();

    apr_pool_t *m_pool;

    apr_pool_create( &m_pool, NULL );

    svn_config_ensure( "", m_pool );

    svn_client_ctx_t    m_context;
#if 0
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
#endif

    // get the config based on the config dir passed in
    svn_config_get_config( &m_context.config, "", m_pool );

#if 0
    // tell the auth functions where the config dir is
    svn_auth_set_parameter( auth_baton, SVN_AUTH_PARAM_CONFIG_DIR, config_dir );

    m_context.auth_baton = auth_baton;
    m_context.log_msg_func = handlerLogMsg;
    m_context.log_msg_baton = this;
    m_context.notify_func = handlerNotify;
    m_context.notify_baton = this;
    m_context.cancel_func = handlerCancel;
    m_context.cancel_baton = this;
#endif

    apr_pool_t *pool = svn_pool_create( NULL );

    svn_opt_revision_t revision;
    revision.kind = svn_opt_revision_working;


    char *path = argv[1];

    svn_boolean_t recurse = FALSE;

    int t0 = elapse_time();

    int max_calls = atoi( argv[2] );
    int i;
    for( i=0; i<max_calls; i++ )
    {
        apr_array_header_t *props = NULL;

        svn_error_t *error = svn_client_proplist
            (
            &props,
            path,
            &revision,
            recurse,
            &m_context,
            pool
            );
    }

    int t1 = elapse_time();

    int total = t1 - t0;

    std::cout << "Time for " << max_calls << " calls " << total << "ms" << std::endl;
    std::cout << "Time for " << 1 << " call " << total/max_calls << "ms" << std::endl;

    return 0;
}


#ifdef WIN32
#include <windows.h>

static int elapse_time()
{
    return GetTickCount();
}

#else
#include <sys/time.h>

static bool need_init_elapse_time = true;
static struct timeval start_time;

static int elapse_time()
{
    if( need_init_elapse_time )
    {
        gettimeofday( &start_time, NULL );
    }
    timeval now;
    gettimeofday( &now, NULL );

    return (now.tv_sec - start_time.tv_sec)*1000 + ( (now.tv_usec - start_time.tv_usec)/1000);
}
#endif
