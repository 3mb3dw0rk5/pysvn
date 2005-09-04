//
// ====================================================================
// Copyright (c) 2003-2005 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//

#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "pysvn.hpp"
#include "svn_config.h"
#include "svn_time.h"

static const char name_utf8[] = "utf-8";
static const std::string str_checksum( "checksum" );
static const std::string str_comment( "comment" );
static const std::string str_conflict_new( "conflict_new" );
static const std::string str_conflict_old( "conflict_old" );
static const std::string str_conflict_wrk( "conflict_wrk" );
static const std::string str_copyfrom_rev( "copyfrom_rev" );
static const std::string str_copyfrom_url( "copyfrom_url" );
static const std::string str_creation_date( "creation_date" );
static const std::string str_expiration_date( "expiration_date" );
static const std::string str_is_dav_comment( "is_dav_comment" );
static const std::string str_kind( "kind" );
static const std::string str_last_changed_author( "last_changed_author" );
static const std::string str_last_changed_date( "last_changed_date" );
static const std::string str_last_changed_rev( "last_changed_rev" );
static const std::string str_lock( "lock" );
static const std::string str_owner( "owner" );
static const std::string str_path( "path" );
static const std::string str_prejfile( "prejfile" );
static const std::string str_prop_time( "prop_time" );
static const std::string str_repos_root_URL( "repos_root_URL" );
static const std::string str_repos_UUID( "repos_UUID" );
static const std::string str_rev( "rev" );
static const std::string str_schedule( "schedule" );
static const std::string str_text_time( "text_time" );
static const std::string str_token( "token" );
static const std::string str_URL( "URL" );
static const std::string str_wc_info( "wc_info" );

Py::Object utf8_string_or_none( const char *str )
{
    if( str == NULL )
        return Py::None();
    else
        return Py::String( str, name_utf8 );
}

Py::Object path_string_or_none( const char *str, SvnPool &pool )
{
    if( str == NULL )
        return Py::None();
    else
        return Py::String( osNormalisedPath( str, pool ), name_utf8 );
}

Py::Object utf8_string_or_none( const std::string &str )
{
    if( !str.empty() )
        return Py::String( str, name_utf8 );
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

#ifdef PYSVN_HAS_CLIENT_INFO
Py::Object toObject( const svn_info_t *info )
{
    Py::Dict py_info;

    // Where the item lives in the repository.
    py_info[str_URL] = utf8_string_or_none( info->URL );

    // The revision of the object.  If path_or_url is a working-copy
    // path, then this is its current working revnum.  If path_or_url
    // is a URL, then this is the repos revision that path_or_url lives in.
    py_info[str_rev] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, info->rev ) );

    // The node's kind.
    py_info[str_kind] = toEnumValue( info->kind );

    // The root URL of the repository.
    py_info[str_repos_root_URL] = utf8_string_or_none( info->repos_root_URL );

    // The repository's UUID.
    py_info[str_repos_UUID] = utf8_string_or_none( info->repos_UUID );

    // The last revision in which this object changed.
    py_info[str_last_changed_rev] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, info->last_changed_rev ) );

    // The date of the last_changed_rev.
    py_info[str_last_changed_date] = toObject( info->last_changed_date );

    // The author of the last_changed_rev.
    py_info[str_last_changed_author] = utf8_string_or_none( info->last_changed_author );

    // An exclusive lock, if present.  Could be either local or remote.
    if( info->lock == NULL )
    {
        py_info[str_lock] = Py::None();
    }
    else
    {
        Py::Dict py_lock;
        py_lock[str_path] = utf8_string_or_none( info->lock->path );
        py_lock[str_token] = utf8_string_or_none( info->lock->token );
        py_lock[str_owner] = utf8_string_or_none( info->lock->owner );
        py_lock[str_comment] = utf8_string_or_none( info->lock->comment );
        py_lock[str_is_dav_comment] = Py::Int( info->lock->is_dav_comment != 0 );
        if( info->lock->creation_date == 0 )
            py_lock[str_creation_date] = Py::None();
        else
            py_lock[str_creation_date] = toObject( info->lock->creation_date );
        if( info->lock->expiration_date == 0 )
            py_lock[str_expiration_date] = Py::None();
        else
            py_lock[str_expiration_date] = toObject( info->lock->expiration_date );

        py_info[str_lock] = py_lock;
    }

    // Whether or not to ignore the next 10 wc-specific fields.
    if( info->has_wc_info == 0 )
    {
        py_info[str_wc_info] = Py::None();
    }
    else
    {
        Py::Dict py_wc_info;
        py_wc_info[str_schedule] = toEnumValue( info->schedule );
        py_wc_info[str_copyfrom_url] = utf8_string_or_none( info->copyfrom_url );
        py_wc_info[str_copyfrom_rev] = Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, info->copyfrom_rev ) );
        py_wc_info[str_text_time] = toObject( info->text_time );
        py_wc_info[str_prop_time] = toObject( info->prop_time );
        py_wc_info[str_checksum] = utf8_string_or_none( info->checksum );
        py_wc_info[str_conflict_old] = utf8_string_or_none( info->conflict_old );
        py_wc_info[str_conflict_new] = utf8_string_or_none( info->conflict_new );
        py_wc_info[str_conflict_wrk] = utf8_string_or_none( info->conflict_wrk );
        py_wc_info[str_prejfile] = utf8_string_or_none( info->prejfile );
        py_info[str_wc_info] = py_wc_info;
    }

    return py_info;
}
#endif

Py::Object propsToObject( apr_hash_t *props, SvnPool &pool )
{
    Py::Dict py_prop_dict;

    for( apr_hash_index_t *hi = apr_hash_first( pool, props ); hi; hi = apr_hash_next( hi ) )
    {
        const void *key = NULL;
        void *val = NULL;

        apr_hash_this (hi, &key, NULL, &val);
        const svn_string_t *propval = (const svn_string_t *)val;
        
        py_prop_dict[ Py::String( (const char *)key ) ] = Py::String( propval->data, (int)propval->len );
    }

    return py_prop_dict;
}

void proplistToObject( Py::List &py_path_propmap_list, apr_array_header_t *props, SvnPool &pool )
{
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
}

Py::String asUtf8String( Py::Object obj )
{
    Py::String any( obj );
    Py::String utf8( any.encode( name_utf8 ) );
    return utf8;
}


apr_array_header_t *targetsFromStringOrList( Py::Object arg, SvnPool &pool )
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

            for( Py::List::size_type i=0; i<path_list.length(); i++ )
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

Py::List toListOfStrings( Py::Object obj )
{
    Py::List list;
    if( obj.isList() )
        list = obj;
    else
        list.append( obj );

    // check all members of the list are strings
    for( Py::List::size_type i=0; i<list.length(); i++ )
    {
        Py::String path_str( list[i] );
    }

    return list;
}
