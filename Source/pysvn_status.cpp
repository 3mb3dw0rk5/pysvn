//
// ====================================================================
// Copyright (c) 2003-2004 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
//
//	pysvn_status.cpp
//

#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "pysvn.hpp"

//--------------------------------------------------------------------------------

pysvn_status::pysvn_status( const svn::Status &_svn_status )
	: m_svn_status( _svn_status )
	{ }

pysvn_status::~pysvn_status()
	{ }

Py::Object pysvn_status::getattr( const char *_name )
	{
	std::string name( _name );

	if( name == "__name__" )
		{
		return Py::String( "status" );
		}
	else if( name == "__members__" )
		{
		Py::List members;

		members.append( Py::String( "path" ) );
		members.append( Py::String( "entry" ) );
		members.append( Py::String( "is_versioned" ) );
		members.append( Py::String( "is_locked" ) );
		members.append( Py::String( "is_copied" ) );
		members.append( Py::String( "is_switched" ) );
		members.append( Py::String( "prop_status" ) );
		members.append( Py::String( "text_status" ) );
		members.append( Py::String( "repos_prop_status" ) );
		members.append( Py::String( "repos_text_status" ) );

		return members;
		}

	else if( name == "path" )
		{
		return Py::String( osNormalisedPath( m_svn_status.path() ) );
		}
	else if( name == "entry" )
		{
		return Py::asObject( new pysvn_entry( m_svn_status.entry() ) );
		}
	else if( name == "is_versioned" )
		{
		return Py::Int( m_svn_status.isVersioned() );
		}
	else if( name == "is_locked" )
		{
		return Py::Int( m_svn_status.isLocked() );
		}
	else if( name == "is_copied" )
		{
		return Py::Int( m_svn_status.isCopied() );
		}
	else if( name == "is_switched" )
		{
		return Py::Int( m_svn_status.isSwitched() );
		}
	else if( name == "prop_status" )
		{
		return toEnumValue( m_svn_status.propStatus() );
		}
	else if( name == "text_status" )
		{
		return toEnumValue( m_svn_status.textStatus() );
		}
	else if( name == "repos_prop_status" )
		{
		return toEnumValue( m_svn_status.reposPropStatus() );
		}
	else if( name == "repos_text_status" )
		{
		return toEnumValue( m_svn_status.reposTextStatus() );
		}
	else
		return getattr_methods( _name );
	}

static const char class_doc[] =
"status - subversion status\n"
"\n"
"The status object has the following member variables:\n"
"\n"
"    path              - path this status refers to\n"
"    entry             - a subversion entry object\n"
"    is_versioned      - boolean\n"
"    is_locked         - boolean\n"
"    is_copied         - boolean\n"
"    is_switched       - boolean\n"
"    prop_status       - enumeration of svn_wc_status_kind\n"
"    text_status       - enumeration of svn_wc_status_kind\n"
"    repos_prop_status - enumeration of svn_wc_status_kind\n"
"    repos_text_status - enumeration of svn_wc_status_kind\n";


void pysvn_status::init_type()
	{
	behaviors().name("status");
	behaviors().doc( class_doc );
	behaviors().supportGetattr();
	}
