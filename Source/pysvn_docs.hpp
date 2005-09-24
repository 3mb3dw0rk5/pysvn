//
// ====================================================================
// Copyright (c) 2003-2005 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
//
//   pysvn_docs.hpp
//

//--------------------------------------------------------------------------------
//
//   Doc strings
//
//--------------------------------------------------------------------------------
extern const char *copyright;
extern const char *module_doc;
extern const char *class_client_doc;
extern const char *class_revision_doc;
extern const char *class_transaction_doc;

#define GET_AUTH_CACHE_DOC \
"enable = get_auth_cache()\n\
return true if credential caching is enabled\n\
otherwise return false\n"

#define SET_AUTH_CACHE_DOC \
"set_auth_cache( enable )\n\
Allow authorisation credential caching if enable is true\n\
otherwise disable caching\n"

#define GET_AUTO_PROPS_DOC \
"enable = get_auto_props()\n\
Returns true When svn will automatically set properties\n\
when adding files otherwise return false\n"

#define SET_AUTO_PROPS_DOC \
"set_auto_props( enable )\n\
When enabled svn will automatically set properties\n\
when adding files otherwise when disabled will not\n"

#define IS_URL_DOC \
"is_url( path_or_url )\n\
return true if path_or_url is a SVN URL\n"

#define PYSVN_ADD_DOC \
"add( path,\n\
     recurse=True )\n\
\n\
Schedules the file or directory specfied in path for addition to the repository.  Set recurse to True to recursively add a directory's children.\n\
Files are added at the next check in.\n"
#define PYSVN_ANNOTATE_DOC \
"annotation_list = annotate( url_or_path,\n\
     revision_start=pysvn.Revision( opt_revision_kind.number, 0 ),\n\
     revision_end=pysvn.Revision( opt_revision_kind.head ) )\n\
\n\
annotate command.\n"
#define PYSVN_CAT_DOC \
"cat( url_or_path,\n\
     revision=pysvn.Revision( opt_revision_kind.head ) )\n\
\n\
Return the contents of path for the specified revision."
#define PYSVN_CHECKOUT_DOC \
"checkout( url,\n\
          path,\n\
          recurse=True,\n\
          revision=pysvn.Revision( opt_revision_kind.head ) )\n\
\n\
Checkout the repository at url into the location specified by path.  Set recurse to True to recursively check out a directory's children.\n\
Specify a revision to check out a particular version of the source tree."
#define PYSVN_CLEANUP_DOC \
"cleanup( path )\n\
\n\
Clean up any locks in the working copy at path.  Usually such locks are the result of a failed or interrupted operation.\n"
#define PYSVN_CHECKIN_DOC \
"checkin( path_list,\n\
        log_message,\n\
        recurse=True )\n\
\n\
Check in the files in the path_list to the repository with the specifed log_message.  Set recurse to True to recursively check in a directory's children. with \
the same log message.\n"
#define PYSVN_COPY_DOC \
"copy( src_url_or_path,\n\
      dest_url_or_path,\n\
      src_revision=pysvn.Revision( opt_revision_kind.head ) )\n\
\n\
Duplicate something in working copy or repos, remembering history.\n\
\n\
src_path and dest_path can each be either a working copy (WC) path or URL:\n\
  WC  -> WC:   copy and schedule for addition (with history)\n\
  WC  -> URL:  immediately check in a copy of WC to URL\n\
  URL -> WC:   check out URL into WC, schedule for addition\n\
  URL -> URL:  complete server-side copy;  used to branch & tag"
#ifdef PYSVN_HAS_CLIENT_DIFF2
#define PYSVN_DIFF_DOC \
"diff( tmp_path,\n\
      url_or_path,\n\
      revision1=pysvn.Revision( opt_revision_kind.base ),\n\
      url_or_path2,\n\
      revision2=pysvn.Revision( opt_revision_kind.working ),\n\
      recurse=True,\n\
      ignore_ancestry=False,\n\
      diff_deleted=True,\n\
      ignore_content_type )\n\
\n\
Display the differences between two paths."
#else
#define PYSVN_DIFF_DOC \
"diff( tmp_path,\n\
      url_or_path,\n\
      revision1=pysvn.Revision( opt_revision_kind.base ),\n\
      url_or_path2,\n\
      revision2=pysvn.Revision( opt_revision_kind.working ),\n\
      recurse=True,\n\
      ignore_ancestry=False,\n\
      diff_deleted=True )\n\
\n\
Display the differences between two paths."
#endif

#ifdef PYSVN_HAS_CLIENT_EXPORT2
#define PYSVN_EXPORT_DOC \
"export( src_url_or_path,\n\
        dest_path,\n\
        force=False,\n\
        revision=pysvn.Revision( opt_revision_kind.head ),\n\
        native_eol=False )\n\
\n\
Create an unversioned copy of a tree.\n\
\n\
1. Exports a clean directory tree from the repository specified by\n\
   URL src_path, at revision if it is given, otherwise at HEAD, into\n\
   dest_path.\n\
\n\
2. Exports a clean directory tree from the working copy specified by\n\
   src_path into dest_path.  All local changes will be preserved, but files\n\
   not under revision control will not be copied."
#else
#define PYSVN_EXPORT_DOC \
"export( src_url_or_path,\n\
        dest_path,\n\
        force=False,\n\
        revision=pysvn.Revision( opt_revision_kind.head ) )\n\
\n\
Create an unversioned copy of a tree.\n\
\n\
1. Exports a clean directory tree from the repository specified by\n\
   URL src_path, at revision if it is given, otherwise at HEAD, into\n\
   dest_path.\n\
\n\
2. Exports a clean directory tree from the working copy specified by\n\
   src_path into dest_path.  All local changes will be preserved, but files\n\
   not under revision control will not be copied."
#endif

#define PYSVN_INFO_DOC \
"entry = info( path )\n\
return info on path"

#ifdef PYSVN_HAS_CLIENT_INFO
#define PYSVN_INFO2_DOC \
"entry_list = \n\
info2( url_or_path,\n\
       revision=pysvn.Revision( opt_revision_kind.unspecified )\n\
       peg_revision=pysvn.Revision( opt_revision_kind.unspecified )\n\
       recurse=False )\n\
\n\
return info on url_or_path"
#else
#define PYSVN_INFO2_DOC ""
#endif

#define PYSVN_IMPORT_DOC \
"import_( path,\n\
        url,\n\
        log_message,\n\
        recurse=True )\n\
\n\
Check in an unversioned file or tree into the repository.\n\
\n\
Recursively check in a copy of PATH to URL.\n\
Parent directories are created as necessary in the repository."


#ifdef PYSVN_HAS_CLIENT_LOCK
#define PYSVN_LOCK_DOC \
"lock( url_or_path,\n\
      message,\n\
      force=False)\n\
Lock the url_or_path give the message as the reason. Set force True to break\n\
replace an existing lock with this one.\n\
"
#else
#define PYSVN_LOCK_DOC ""
#endif

#define PYSVN_LOG_DOC \
"log_list = log( url_or_path,\n\
                revision_start=pysvn.Revision( opt_revision_kind.head ),\n\
                revision_end=pysvn.Revision( opt_revision_kind.number, 0 ),\n\
                discover_changed_paths=False,\n\
                strict_node_history=True )\n\
Return the log messages for the specifed path between revisions start and end.\n\
If strict_node_history is True do not cross copies while traversing history.\n\
If discover_changed_paths is True return the set of changed paths for each revision reported.\n\
\n\
Show the log messages for a set of revisions of path."

#define PYSVN_LS_DOC \
"entries_list = ls( url_or_path,\n\
            revision=pysvn.Revision( opt_revision_kind.head ),\n\
            recurse=True )\n\
\n\
Return entries_list for the given path at the provided revision."
#define PYSVN_MERGE_DOC \
"merge( url_or_path1,\n\
       revision1,\n\
       url_or_path2,\n\
       revision2,\n\
       local_path,\n\
       force=False,\n\
       recurse=True,\n\
       notice_ancestry=False,\n\
       dry_run=False )\n\
\n\
Apply the differences between two sources to a working copy path."

#define PYSVN_MKDIR_DOC \
"mkdir( url_or_path,\n\
       log_message )\n\
mkdir( [url_or_path,...],\n\
       log_message )\n\
\n\
Create a new directory under revision control.\n\
\n\
Create version controlled directories.\n\
\n\
If run on a working copy PATHs, each directory is scheduled for\n\
addition upon the next check in.\n\
\n\
If run on URLs, the directories are created in the repository via an\n\
immediate check in.\n\
\n\
In both cases, all the intermediate directories must already exist."
#define PYSVN_MOVE_DOC \
"move( src_url_or_path,\n\
      dest_url_or_path,\n\
      src_revision=pysvn.Revision( opt_revision_kind.head ),\n\
      force=False )\n\
\n\
Move/rename something in working copy or repository.\n\
\n\
NOTE:  this command is equivalent to a 'copy' and 'delete'.\n\
\n\
src_path and dest_path can both be working copy (WC) paths or URLs:\n\
  WC  -> WC:   move and schedule for addition (with history)\n\
  URL -> URL:  complete server-side rename."

#define PYSVN_PROPDEL_DOC \
"rev = propdel( prop_name,\n\
         url_or_path,\n\
         revision=pysvn.Revision( opt_revision_kind.head ),\n\
         recurse=False )\n\
\n\
delete the property propname from path\n\
"
#define PYSVN_PROPGET_DOC \
"prop_list = propget( prop_name,\n\
                      url_or_path,\n\
                      src_revision=pysvn.Revision( opt_revision_kind.head ),\n\
                      recurse=False )\n\
Returns a list of tuples (path, prop_dict)\n\
"
#define PYSVN_PROPLIST_DOC \
"prop_list = proplist( url_or_path,\n\
                      revision=pysvn.Revision( opt_revision_kind.head ),\n\
                      recurse=False )\n\
Returns a list of tuples (path, prop_dict)\n\
"
#define PYSVN_PROPSET_DOC \
"rev = propset( prop_name,\n\
         prop_value,\n\
         url_or_path,\n\
         revision=pysvn.Revision( opt_revision_kind.head ),\n\
         recurse=False )\n\
\n\
set the property prop_name to prop_value in url_or_path\n\
"
#define PYSVN_RELOCATE_DOC \
"relocate( from_url,\n\
           to_url,\n\
           path,\n\
           recurse=True)\n\
\n\
relocate the working copy from from_url to to_url of path\n\
"

#define PYSVN_REMOVE_DOC \
"remove( url_or_path,\n\
        force=False )\n\
remove( [url_or_path, ...],\n\
        force=False )\n\
\n\
If run on a working copy PATHs, each item is scheduled for deletion\n\
upon the next check in.  Files, and directories that have not been\n\
checked in, are immediately removed from the working copy.  The\n\
command will not remove PATHs that are, or contain, unversioned\n\
or modified items; set force=True to override this\n\
behaviour.\n\
\n\
If run on URLs, the items are deleted from the repository via an\n\
immediate check in."

#define PYSVN_RESOLVED_DOC \
"resolved( path,\n\
          recurse=True )\n\
Mark the conflicted file at path as resolved."
#define PYSVN_REVERT_DOC \
"revert( path,\n\
        recurse=False )\n\
Discard any changes in the working copy at path. Set recurse to True to recursively revert a directory's children.\n"

#define PYSVN_REVPROPDEL_DOC \
"rev = revpropdel( prop_name,\n\
                  url,\n\
                  revision=pysvn.Revision( opt_revision_kind.head ),\n\
                  force=False )\n\
\n\
delete the property prop_name from url\n\
"

#define PYSVN_REVPROPGET_DOC \
"prop_list = revpropget( prop_name,\n\
                        url,\n\
                        revision=pysvn.Revision( opt_revision_kind.head ) )\n\
Returns a list of tuples (path, prop_dict)\n\
"
#define PYSVN_REVPROPLIST_DOC \
"prop_list = revproplist( url,\n\
                         revision=pysvn.Revision( opt_revision_kind.head ) )\n\
Returns a list of tuples (path, prop_dict)\n\
"
#define PYSVN_REVPROPSET_DOC \
"rev = revpropset( prop_name,\n\
         prop_value,\n\
         url,\n\
         revision=pysvn.Revision( opt_revision_kind.head ),\n\
         force=False )\n\
\n\
set the property propname to propval in path\n\
"


#ifdef PYSVN_HAS_CLIENT_STATUS2
#define PYSVN_STATUS_DOC \
"path_status_list = status( path,\n\
           recurse=True,\n\
           get_all=True,\n\
           update=False,\n\
           ignore=False,\n\
           ignore_externals=False )\n\
returns status for all files in the directory path in path_status_list"
#else
#define PYSVN_STATUS_DOC \
"path_status_list = status( path,\n\
           recurse=True,\n\
           get_all=True,\n\
           update=False,\n\
           ignore=False )\n\
returns status for all files in the directory path in path_status_list"
#endif

#define PYSVN_SWITCH_DOC \
"switch( path,\n\
        url,\n\
        recurse=True,\n\
        revision=pysvn.Revision( opt_revision_kind.head ) )\n\
\n\
Update the working copy to a different URL.\n\
\n\
1. Update the working copy to mirror a new URL.  This behaviour is a\n\
   superset of \"svn update\".\n\
   Note:  this is the way to move a working copy to a new branch.\n\
\n\
2. Reconnect the working copy when the repository URL has changed."

#ifdef PYSVN_HAS_CLIENT_LOCK
#define PYSVN_UNLOCK_DOC \
"unlock( url_or_path,\n\
        force=False)\n\
Unlock the url_or_path. Set force True to break\n\
unlock an existing lock.\n\
"
#else
#define PYSVN_UNLOCK_DOC ""
#endif


#define PYSVN_UPDATE_DOC \
"revision = \\\n\
update( path,\n\
        recurse=True,\n\
        revision=pysvn.Revision( opt_revision_kind.head ) )\n\
\n\
Update the file in the working copy at path to the specified revision.  Set recurse to True to recursively update a directory's children.\n\
This command is typically used to get the latest changes from the repository.  Note that updating to an older revision does not change the \n\
current revision.  To make the current version identical to an older revision, use a merge followed by a check in.\n\
update returns a pysvn.Revision containing the number of the revision the working copy was updated to."



//
// Transaction docs
//
#define TXN_CAT_DOC \
"x = cat( path )\n\
\n\
TBD\n\
"

#define TXN_CHANGED_DOC \
"x = changed()\n\
\n\
TBD\n\
"

#define TXN_DIFF_DOC \
"x = diff( path )\n\
\n\
TBD\n\
"

#define TXN_PROPDEL_DOC \
"propdel( prop_name )\n\
\n\
delete the property prop_name from the transaction\n\
"
#define TXN_PROPGET_DOC \
"prop_value = propget( prop_name )\n\
\n\
Returns the prop_value string for prop_name in the transaction\n\
"
#define TXN_PROPLIST_DOC \
"prop_dict = proplist()\n\
\n\
Returns a dict with all the properties in the transaction\n\
"
#define TXN_PROPSET_DOC \
"propset( prop_name,\n\
          prop_value )\n\
\n\
set the property prop_name to prop_value in the transaction\n\
"

#define TXN_REVPROPDEL_DOC \
"revpropdel( prop_name )\n\
\n\
delete the revproperty prop_name from the transaction\n\
"
#define TXN_REVPROPGET_DOC \
"prop_value = revpropget( prop_name )\n\
\n\
Returns the prop_value string for prop_name in the transaction\n\
"
#define TXN_REVPROPLIST_DOC \
"revprop_dict = revproplist()\n\
\n\
Returns a dict with all the revproperties in the transaction\n\
"
#define TXN_REVPROPSET_DOC \
"revpropset( prop_name,\n\
          prop_value )\n\
\n\
set the revproperty prop_name to prop_value in the transaction\n\
"
