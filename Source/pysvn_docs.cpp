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
//  pysvn_docs.cpp
//
#include "pysvn_docs.hpp"

const char *copyright =
" Copyright (c) 2003-2005 Barry A Scott.  All rights reserved.\n"
"\n"
" This software is licensed as described in the file LICENSE.txt,\n"
" which you should have received as part of this distribution.\n"
"\n"
" =============================================================\n"
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
GET_AUTH_CACHE_DOC "\n\n"
SET_AUTH_CACHE_DOC "\n\n"
GET_AUTO_PROPS_DOC "\n\n"
SET_AUTO_PROPS_DOC "\n\n"
PYSVN_ADD_DOC "\n\n"
PYSVN_ANNOTATE_DOC "\n\n"
PYSVN_CAT_DOC "\n\n"
PYSVN_CHECKIN_DOC "\n\n"
PYSVN_CHECKOUT_DOC "\n\n"
PYSVN_CLEANUP_DOC "\n\n"
PYSVN_COPY_DOC "\n\n"
PYSVN_DIFF_DOC "\n\n"
PYSVN_EXPORT_DOC "\n\n"
PYSVN_IMPORT_DOC "\n\n"
PYSVN_INFO_DOC "\n\n"
PYSVN_INFO2_DOC "\n\n"
PYSVN_LOCK_DOC  "\n\n"
PYSVN_LOG_DOC "\n\n"
PYSVN_LS_DOC "\n\n"
PYSVN_MERGE_DOC "\n\n"
PYSVN_MKDIR_DOC "\n\n"
PYSVN_MOVE_DOC "\n\n"
PYSVN_PROPDEL_DOC "\n\n"
PYSVN_PROPGET_DOC "\n\n"
PYSVN_PROPLIST_DOC "\n\n"
PYSVN_PROPSET_DOC "\n\n"
PYSVN_RELOCATE_DOC "\n\n"
PYSVN_REMOVE_DOC "\n\n"
PYSVN_RESOLVED_DOC "\n\n"
PYSVN_REVERT_DOC "\n\n"
PYSVN_REVPROPDEL_DOC "\n\n"
PYSVN_REVPROPGET_DOC "\n\n"
PYSVN_REVPROPLIST_DOC "\n\n"
PYSVN_REVPROPSET_DOC "\n\n"
PYSVN_STATUS_DOC "\n\n"
PYSVN_SWITCH_DOC "\n\n"
PYSVN_UNLOCK_DOC  "\n\n"
PYSVN_UPDATE_DOC
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

const char *class_transaction_doc =
"Transaction - Subversion transaction interface\n"
"\n"
"Interface summary:\n"
"\n"
"transaction = pysvn.Transaction()\n"
"\n"
"Transaction has the following methods:\n"
"\n"
TXN_CHANGED_DOC "\n\n"
TXN_PROPDEL_DOC "\n\n"
TXN_PROPGET_DOC "\n\n"
TXN_PROPLIST_DOC "\n\n"
TXN_PROPSET_DOC "\n\n"
TXN_REVPROPDEL_DOC "\n\n"
TXN_REVPROPGET_DOC "\n\n"
TXN_REVPROPLIST_DOC "\n\n"
TXN_REVPROPSET_DOC
;
