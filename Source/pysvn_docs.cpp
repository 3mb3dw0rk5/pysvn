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
//	pysvn_docs.cpp
//
#include "pysvn_docs.hpp"

const char *copyright =
" Copyright (c) 2003-2004 Barry A Scott.  All rights reserved.\n"
"\n"
" This software is licensed as described in the file LICENSE.txt,\n"
" which you should have received as part of this distribution.\n"
"\n"
" =============================================================\n"
"\n"
" This product includes software developed by the\n"
" RapidSvn Group (http://rapidsvn.tigris.org/).\n"
"\n"
" This product includes software developed by\n"
" CollabNet (http://www.Collab.Net/).\n";


const char *module_doc =
"PySVN module interface to Subversion.\n"
"\n"
"This modules copyright is available in pysvn.copyright\n"
"and the modules version is available in pysvn.version as\n"
"a tuple, (major, minor, patch, build). The subversion\n"
"version is available as a tuple (major, minor, micro, tag)\n"
"\n"
"The pysvn module has two classes:\n"
"\n"
"    Client - the subversion client interface\n"
"    Revision - subversion revision objects\n"
"\n"
"The following enumerations are provided:\n"
"\n"
"    opt_revision_kind\n"
"    wc_notify_action\n"
"    wc_status_kind\n"
"    wc_merge_outcome\n"
"    wc_notify_state\n"
"    node_kind\n"
"\n"
"Use python builtin dir() to list all available values in an enumeration:\n"
"\n"
"    print dir( pysvn.wc_notify_action )\n";

const char *class_client_doc =
"Client - Subversion client interface\n"
"\n"
"Interface summary:\n"
"\n"
"client = pysvn.Client()\n"
"\n"
"Client has the following methods:\n"

IS_URL_DOC "\n\n"
SET_AUTH_CACHE_DOC "\n\n"
SVN_ADD_DOC "\n\n"
SVN_ANNOTATE_DOC "\n\n"
SVN_CAT_DOC "\n\n"
SVN_CHECKIN_DOC "\n\n"
SVN_CHECKOUT_DOC "\n\n"
SVN_CLEANUP_DOC "\n\n"
SVN_COPY_DOC "\n\n"
SVN_DIFF_DOC "\n\n"
SVN_EXPORT_DOC "\n\n"
SVN_IMPORT_DOC "\n\n"
SVN_INFO_DOC "\n\n"
SVN_LOG_DOC "\n\n"
SVN_LS_DOC "\n\n"
SVN_MERGE_DOC "\n\n"
SVN_MKDIR_DOC "\n\n"
SVN_MOVE_DOC "\n\n"
SVN_PROPDEL_DOC "\n\n"
SVN_PROPGET_DOC "\n\n"
SVN_PROPLIST_DOC "\n\n"
SVN_PROPSET_DOC "\n\n"
SVN_RELOCATE_DOC "\n\n"
SVN_REMOVE_DOC "\n\n"
SVN_RESOLVED_DOC "\n\n"
SVN_REVERT_DOC "\n\n"
SVN_REVPROPDEL_DOC "\n\n"
SVN_REVPROPGET_DOC "\n\n"
SVN_REVPROPLIST_DOC "\n\n"
SVN_REVPROPSET_DOC "\n\n"
SVN_SINGLE_STATUS_DOC "\n\n"
SVN_STATUS_DOC "\n\n"
SVN_SWITCH_DOC "\n\n"
SVN_UPDATE_DOC
;

const char *class_revision_doc =
"revision - subversion revision\n"
"\n"
"The revision object has three member variables:\n"
"\n"
"    kind   - the kind of revision, its value is one of the\n"
"             opt_revision_kind enumerations.\n"
"    date   - date and time when kind is opt_revision_kind.date\n"
"    number - revision number when kind is opt_revision_kind.number\n"
"\n"
"Interface summary:\n"
"\n"
"    import pysvn\n"
"    import time\n"
"\n"
"    revhead = pysvn.revision( pysvn.opt_revision_kind.head )\n"
"    revdate = pysvn.revision( pysvn.opt_revision_kind.date, time.time() )\n"
"    revnum = pysvn.revision( pysvn.opt_revision_kind.number, 4721 )\n"
;
