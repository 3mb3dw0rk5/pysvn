//
// ====================================================================
// Copyright (c) 2003-2006 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
//
//  pysvn_entry.cpp
//

#ifdef _MSC_VER
// disable warning C4786: symbol greater than 255 character,
// nessesary to ignore as <map> causes lots of warning
#pragma warning(disable: 4786)
#endif

#include "pysvn.hpp"

//--------------------------------------------------------------------------------

pysvn_entry::pysvn_entry( const svn_wc_entry_t *svn_entry, SvnContext &context)
: m_pool( context )
, m_svn_entry( svn_wc_entry_dup( svn_entry, m_pool ) )
{
}

pysvn_entry::~pysvn_entry()
{
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
#ifdef PYSVN_HAS_CLIENT_STATUS2
        members.append( Py::String( "lock_token" ) );
        members.append( Py::String( "lock_owner" ) );
        members.append( Py::String( "lock_comment" ) );
        members.append( Py::String( "lock_creation_date" ) );
#endif
        return members;
    }

    else if( name == "checksum" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->checksum );
    }
    else if( name == "commit_author" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->cmt_author );
    }
    else if( name == "commit_revision" )
    {
        if( m_svn_entry != NULL )
            return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, m_svn_entry->cmt_rev ) );
    }
    else if( name == "commit_time" )
    {
        if( m_svn_entry != NULL )
            return toObject( m_svn_entry->cmt_date );
    }
    else if( name == "conflict_new" )
    {
        if( m_svn_entry != NULL )
            return path_string_or_none( m_svn_entry->conflict_new, m_pool );
    }
    else if( name == "conflict_old" )
    {
        if( m_svn_entry != NULL )
            return path_string_or_none( m_svn_entry->conflict_old, m_pool );
    }
    else if( name == "conflict_work" )
    {
        if( m_svn_entry != NULL )
            return path_string_or_none( m_svn_entry->conflict_wrk, m_pool );
    }
    else if( name == "copy_from_revision" )
    {
        if( m_svn_entry != NULL )
            return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, m_svn_entry->copyfrom_rev ) );
    }
    else if( name == "copy_from_url" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->copyfrom_url );
    }
    else if( name == "is_absent" )
    {
        if( m_svn_entry != NULL )
            return Py::Int( m_svn_entry->absent );
    }
    else if( name == "is_copied" )
    {
        if( m_svn_entry != NULL )
            return Py::Int( m_svn_entry->copied );
    }
    else if( name == "is_deleted" )
    {
        if( m_svn_entry != NULL )
            return Py::Int( m_svn_entry->deleted );
    }
    else if( name == "is_valid" )
    {
        return Py::Int( m_svn_entry != NULL );
    }
    else if( name == "kind" )
    {
        if( m_svn_entry != NULL )
            return toEnumValue( m_svn_entry->kind );
    }
    else if( name == "name" )
    {
        if( m_svn_entry != NULL )
            return path_string_or_none( m_svn_entry->name, m_pool );
    }
    else if( name == "properties_time" )
    {
        if( m_svn_entry != NULL )
            return toObject( m_svn_entry->prop_time );
    }
    else if( name == "property_reject_file" )
    {
        if( m_svn_entry != NULL )
            return path_string_or_none( m_svn_entry->prejfile, m_pool );
    }
    else if( name == "repos" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->repos );
    }
    else if( name == "revision" )
    {
        if( m_svn_entry != NULL )
            return Py::asObject( new pysvn_revision( svn_opt_revision_number, 0, m_svn_entry->revision ) );
    }
    else if( name == "schedule" )
    {
        if( m_svn_entry != NULL )
            return toEnumValue( m_svn_entry->schedule );
    }
    else if( name == "text_time" )
    {
        if( m_svn_entry != NULL )
            return toObject( m_svn_entry->text_time );
    }
    else if( name == "url" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->url );
    }
    else if( name == "uuid" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->uuid );
    }
#ifdef PYSVN_HAS_CLIENT_STATUS2
    else if( name == "lock_token" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->lock_token );
    }
    else if( name == "lock_owner" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->lock_owner );
    }
    else if( name == "lock_comment" )
    {
        if( m_svn_entry != NULL )
            return utf8_string_or_none( m_svn_entry->lock_comment );
    }
    else if( name == "lock_creation_date" )
    {
        if( m_svn_entry != NULL )
            return toObject( m_svn_entry->lock_creation_date );
    }
#endif

    else
        return getattr_methods( _name );

    // the name is valid but the object is not valid
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
