//
// ====================================================================
// Copyright (c) 2003 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
//
//	pysvn_revision.cpp
//

#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "pysvn.hpp"
#include "pysvn_docs.hpp"

//--------------------------------------------------------------------------------

pysvn_revision::pysvn_revision( svn_opt_revision_kind kind,
		double date, int revnum )
	{
	svn_revision.kind = kind;
	svn_revision.value.date = apr_time_t( date * 1000000 );
	svn_revision.value.number = revnum;
	}

pysvn_revision::~pysvn_revision()
	{ }

const svn_opt_revision_t &pysvn_revision::getSvnRevision() const
	{
	return svn_revision;
	}

Py::Object pysvn_revision::getattr( const char *_name )
	{
	std::string name( _name );

	if( name == "__members__" )
		{
		Py::List members;

		members.append( Py::String( "kind" ) );
		members.append( Py::String( "date" ) );
		members.append( Py::String( "number" ) );

		return members;
		}

	else if( name == "kind" )
		{
		return Py::asObject(
			new pysvn_enum_value<svn_opt_revision_kind>( svn_revision.kind ) );
		}
	else if( name == "date" )
		{
		if( svn_revision.kind == svn_opt_revision_date )
			return Py::Float( double( svn_revision.value.date )/1000000 );
		else
			return Py::Nothing();
		}
	else if( name == "number" )
		{
		if( svn_revision.kind == svn_opt_revision_number )
			return Py::Int( svn_revision.value.number );
		else
			return Py::Nothing();
		}
	else
		return getattr_methods( _name );
	}

int pysvn_revision::setattr( const char *_name, const Py::Object &value )
	{
	std::string name( _name );
	if( name == "kind" )
		{
		Py::ExtensionObject< pysvn_enum_value<svn_opt_revision_kind> > kind( value );

		svn_revision.kind = svn_opt_revision_kind( kind.extensionObject()->value );
		}
	else if( name == "date" )
		{
		Py::Float py_date( value );
		apr_time_t date( double( py_date ) * 1000000 );
		svn_revision.value.date = date;
		}
	else if( name == "number" )
		{
		Py::Int revnum( value );
		svn_revision.value.number = revnum;
		}
	else
		throw Py::AttributeError( "Unknown revision attribute" );

	return 0;
	}


void pysvn_revision::init_type()
	{
	behaviors().name("revision");
	behaviors().doc( class_revision_doc );
	behaviors().supportGetattr();
	//behaviors().supportSetattr();
	}
