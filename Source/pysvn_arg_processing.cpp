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


static const char g_utf_8[] = "utf-8";

FunctionArguments::FunctionArguments
    (
    const char *function_name,
    const argument_description *arg_desc,
    const Py::Tuple &args,
    const Py::Dict &kws
    )
: m_function_name( function_name )
, m_arg_desc( arg_desc )
, m_args( args )
, m_kws( kws )
, m_checked_args()
, m_min_args( 0 )
, m_max_args( 0 )
{
    // find out the min and max number of args
    for( const argument_description *p=arg_desc; p->m_arg_name; ++p )
    {
        m_max_args++;
        if( p->m_required )
        {
            m_min_args++;
        }
    }
}

FunctionArguments::~FunctionArguments()
{
}

static char *int_to_string_inner( int n, char *buffer )
{
    char digit = (n%10) + '0';
    int remainder = n/10;
    if( remainder > 0 )
        buffer = int_to_string_inner( remainder, buffer );
    *buffer++ = digit;
    return buffer;
}

static const char *int_to_string( int n )
{
    static char number_string[20];
    char *buffer = int_to_string_inner( n, &number_string[0] );
    return number_string;
}



void FunctionArguments::check()
{
    if( m_args.size() > m_max_args )
    {
        std::string msg = m_function_name;
        msg += "() takes exactly ";
        msg += int_to_string( m_max_args );
        msg += " arguments (";
        msg += int_to_string( m_args.size() );
        msg += " given)";
        throw Py::TypeError( msg );
    }

    int i;
    // place all the positional args in the checked args dict
    for( i=0; i<m_args.size(); i++ )
    {
        m_checked_args[ m_arg_desc[i].m_arg_name ] = m_args[i];
    }

    // look for args by name in the kws dict
    for( i=0; i<m_max_args; i++ )
    {
        const argument_description &arg_desc = m_arg_desc[i];

        // check for duplicate
        if( m_kws.hasKey( arg_desc.m_arg_name ) )
        {
            if( m_checked_args.hasKey( arg_desc.m_arg_name ) )
            {
                std::string msg = m_function_name;
                msg += "() multiple values for keyword argument '";
                msg += arg_desc.m_arg_name;
                msg += "'";
                throw Py::TypeError( msg );
            }

            m_checked_args[ arg_desc.m_arg_name ] = m_kws[ arg_desc.m_arg_name ];
        }
    }

    // check for names we dont known about
    Py::List names( m_kws.keys() );
    for( i=0; i< names.length(); i++ )
    {
        bool found = false;
        Py::String py_name( names[i] );
        std::string name( py_name );

        for( i=0; i<m_max_args; i++ )
        {
            if( name == m_arg_desc[i].m_arg_name )
                {
                found = true;
                break;
                }
        }

        if( !found )
        {
            std::string msg = m_function_name;
            msg += "() got an unexpected keyword argument '";
            msg += name;
            msg += "'";
            throw Py::TypeError( msg );
        }
    }
}


bool FunctionArguments::hasArg( const char *arg_name )
{
    return m_checked_args.hasKey( arg_name );
}

Py::Object FunctionArguments::getArg( const char *arg_name )
{
    return m_checked_args[ arg_name ];
}

bool FunctionArguments::getBoolean( const char *name )
{
    Py::Int boolean_val( getArg( name ) );
    return boolean_val != 0;
}

bool FunctionArguments::getBoolean( const char *name, bool default_value )
{
    if( hasArg( name ) )
    {
        return getBoolean( name );
    }
    else
    {
        return default_value;
    }
}

std::string FunctionArguments::getUtf8String( const char *name )
{
    Py::String any( getArg( name ) );
    Py::String utf8( any.encode( g_utf_8 ) );
    return utf8.as_std_string();
}

std::string FunctionArguments::getUtf8String( const char *name, const std::string &default_value )
{
    if( hasArg( name ) )
    {
        return getUtf8String( name );
    }
    else
    {
        return default_value;
    }
}

std::string FunctionArguments::getString( const char *name )
{
    Py::String any( getArg( name ) );
    return any.as_std_string();
}

std::string FunctionArguments::getString( const char *name, const std::string &default_value )
{
    if( hasArg( name ) )
    {
        return getString( name );
    }
    else
    {
        return default_value;
    }
}

svn_opt_revision_t FunctionArguments::getRevision( const char *name )
{
    Py::Object obj( getArg( name ) );
    if( pysvn_revision::check( obj ) )
    {
        pysvn_revision *rev = static_cast<pysvn_revision *>( obj.ptr() );
        // copy out to caller
        return rev->getSvnRevision();
    }
    else
    {
        std::string msg = m_function_name;
        msg += "() expecting revision object for keyword ";
        msg += name;
        throw Py::AttributeError( msg );
    }
}

svn_opt_revision_t FunctionArguments::getRevision
    (
    const char *name,
    svn_opt_revision_kind default_value
    )
{

    if( hasArg( name ) )
    {
        return getRevision( name );
    }
    else
    {
        svn_opt_revision_t revision;
        revision.kind = default_value;
        if( revision.kind == svn_opt_revision_number )
            revision.value.number = 1;
        return revision;
    }
}
