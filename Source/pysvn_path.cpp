//
// ====================================================================
// Copyright (c) 2003 Barry A Scott.  All rights reserved.
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
#include <algorithm>
#include "svn_path.h"
#include <sys/types.h>
#include <sys/stat.h>

// SVN lives in a world of 

// is
bool is_svn_url( const std::string &path_or_url )
	{
	return svn_path_is_url( path_or_url.c_str() ) != 0;
	}

// convert a path to what SVN likes
std::string svnNormalisedIfPath( const std::string &unnormalised )
	{
	svn::Pool pool;
	if( is_svn_url( unnormalised ) )
		return unnormalised;

	const char *normalised_path = svn_path_internal_style( unnormalised.c_str(), pool );
	return std::string( normalised_path );
	}

// convert a path to what the native OS likes
std::string osNormalisedPath( const std::string &unnormalised )
	{
	svn::Pool pool;
	const char *local_path = svn_path_local_style( unnormalised.c_str(), pool );

	return std::string( local_path );
	}

// true if path exists and is a directory
bool is_path_dir( const std::string &path )
	{
	// is it the svn current dir string?
	if( path == "" )
		return true;

	struct stat stat_buf;

	int rc = stat( path.c_str(), &stat_buf );
	if( rc == 0 && (stat_buf.st_mode & S_IFDIR) != 0 )
		return true;

	return false;
	}
