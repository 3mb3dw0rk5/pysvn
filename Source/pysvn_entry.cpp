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
//	pysvn_entry.cpp
//

#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "pysvn.hpp"

//--------------------------------------------------------------------------------

pysvn_entry::pysvn_entry( const svn::Entry &_svn_entry )
	: svn_entry( _svn_entry )
	{ }

pysvn_entry::~pysvn_entry()
	{ }

static Py::Object string_or_none( const char *str )
	{
	if( str == NULL )
		return Py::Nothing();
	else
		return Py::String( str );
	}

static Py::Object path_string_or_none( const char *str )
	{
	if( str == NULL )
		return Py::Nothing();
	else
		return Py::String( osNormalisedPath( str ) );
	}

Py::Object pysvn_entry::getattr( const char *_name )
	{
	std::string name( _name );

	if( name == "__name__" )
		{
		return Py::String( "entry" );
		}
	else if( name == "__members__" )
		{
		Py::List members;

		members.append( Py::String( "checksum" ) );
		members.append( Py::String( "commit_author" ) );
		members.append( Py::String( "commit_revision" ) );
		members.append( Py::String( "commit_time" ) );
		members.append( Py::String( "conflict_new" ) );
		members.append( Py::String( "conflict_old" ) );
		members.append( Py::String( "conflict_work" ) );
		members.append( Py::String( "copy_from_revision" ) );
		members.append( Py::String( "copy_from_url" ) );
		members.append( Py::String( "is_absent" ) );
		members.append( Py::String( "is_copied" ) );
		members.append( Py::String( "is_deleted" ) );
		members.append( Py::String( "is_valid" ) );
		members.append( Py::String( "kind" ) );
		members.append( Py::String( "name" ) );
		members.append( Py::String( "properties_time" ) );
		members.append( Py::String( "property_reject_file" ) );
		members.append( Py::String( "repos" ) );
		members.append( Py::String( "revision" ) );
		members.append( Py::String( "schedule" ) );
		members.append( Py::String( "text_time" ) );
		members.append( Py::String( "url" ) );
		members.append( Py::String( "uuid" ) );

		return members;
		}

	else if( name == "checksum" )
		{
		if( svn_entry.isValid() )
			return string_or_none( svn_entry.checksum() );
		}
	else if( name == "commit_author" )
		{
		if( svn_entry.isValid() )
			return string_or_none( svn_entry.cmtAuthor() );
		}
	else if( name == "commit_revision" )
		{
		if( svn_entry.isValid() )
			return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, svn_entry.cmtRev() ) );
		}
	else if( name == "commit_time" )
		{
		if( svn_entry.isValid() )
			return toObject( svn_entry.cmtDate() );
		}
	else if( name == "conflict_new" )
		{
		if( svn_entry.isValid() )
			return path_string_or_none( svn_entry.conflictNew() );
		}
	else if( name == "conflict_old" )
		{
		if( svn_entry.isValid() )
			return path_string_or_none( svn_entry.conflictOld() );
		}
	else if( name == "conflict_work" )
		{
		if( svn_entry.isValid() )
			return path_string_or_none( svn_entry.conflictWrk() );
		}
	else if( name == "copy_from_revision" )
		{
		if( svn_entry.isValid() )
			return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, svn_entry.copyfromRev() ) );
		}
	else if( name == "copy_from_url" )
		{
		if( svn_entry.isValid() )
			return string_or_none( svn_entry.copyfromUrl() );
		}
	else if( name == "is_absent" )
		{
		if( svn_entry.isValid() )
			return Py::Int( svn_entry.isAbsent() );
		}
	else if( name == "is_copied" )
		{
		if( svn_entry.isValid() )
			return Py::Int( svn_entry.isCopied() );
		}
	else if( name == "is_deleted" )
		{
		if( svn_entry.isValid() )
			return Py::Int( svn_entry.isDeleted() );
		}
	else if( name == "is_valid" )
		{
		return Py::Int( svn_entry.isValid() );
		}
	else if( name == "kind" )
		{
		if( svn_entry.isValid() )
			return toEnumValue( svn_entry.kind() );
		}
	else if( name == "name" )
		{
		if( svn_entry.isValid() )
			return path_string_or_none( svn_entry.name() );
		}
	else if( name == "properties_time" )
		{
		if( svn_entry.isValid() )
			return toObject( svn_entry.propTime() );
		}
	else if( name == "property_reject_file" )
		{
		if( svn_entry.isValid() )
			return path_string_or_none( svn_entry.prejfile() );
		}
	else if( name == "repos" )
		{
		if( svn_entry.isValid() )
			return string_or_none( svn_entry.repos() );
		}
	else if( name == "revision" )
		{
		if( svn_entry.isValid() )
			return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, svn_entry.revision() ) );
		}
	else if( name == "schedule" )
		{
		if( svn_entry.isValid() )
			return toEnumValue( svn_entry.schedule() );
		}
	else if( name == "text_time" )
		{
		if( svn_entry.isValid() )
			return toObject( svn_entry.textTime() );
		}
	else if( name == "url" )
		{
		if( svn_entry.isValid() )
			return string_or_none( svn_entry.url() );
		}
	else if( name == "uuid" )
		{
		if( svn_entry.isValid() )
			return string_or_none( svn_entry.uuid() );
		}
	else
		return getattr_methods( _name );

	// the name is valid bit the object is not valid
	return Py::Nothing();
	}


static const char class_doc[] =
"entry - subversion entry object\n"
"\n"
"When is_valid is false all other variables will return a value of None.\n"
"\n"
"The entry object has the following member variables:\n"
"\n"
"    checksum             - string\n"
"    commit_author        - string\n"
"    commit_revision      - int revision number\n"
"    commit_time          - time\n"
"    conflict_new         - string\n"
"    conflict_old         - string\n"
"    conflict_work        - string\n"
"    copy_from_revision   - int revision number\n"
"    copy_from_url        - string\n"
"    is_copied            - boolean\n"
"    is_deleted           - boolean\n"
"    is_valid             - boolean\n"
"    kind                 - string\n"
"    name                 - svn_node_kind_t enumeration\n"
"    properties_time      - time\n"
"    property_reject_file - string\n"
"    repos                - string\n"
"    revision             - revision number\n"
"    schedule             - svn_wc_schedule_t enumeration\n"
"    text_time            - time\n"
"    url                  - string\n"
"    uuid                 - string\n";


void pysvn_entry::init_type()
	{
	behaviors().name("entry");
	behaviors().doc( class_doc );
	behaviors().supportGetattr();
	}
