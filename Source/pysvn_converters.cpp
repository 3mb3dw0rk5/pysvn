//
// ====================================================================
// Copyright (c) 2003-2004 Barry A Scott.  All rights reserved.
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

Py::Object utf8_string_or_none( const char *str )
	{
	if( str == NULL )
		return Py::Nothing();
	else
		return Py::String( str, name_utf8 );
	}

Py::Object path_string_or_none( const char *str, SvnPool &pool )
	{
	if( str == NULL )
		return Py::Nothing();
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

Py::List toListOfStrings( Py::Object obj )
	{
	Py::List list;
	if( obj.isList() )
		list = obj;
	else
		list.append( obj );

	// check all members of the list are strings
	for( int i=0; i<list.length(); i++ )
		{
		Py::String path_str( list[i] );
		}

	return list;
	}
