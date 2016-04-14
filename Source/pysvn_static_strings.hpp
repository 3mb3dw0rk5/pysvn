//
// ====================================================================
// Copyright (c) 2003-2013 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
//
//  pysvn_static_strings.hpp
//
#ifndef PYSVN_STATIC_PY_STRINGS_HPP
#define PYSVN_STATIC_PY_STRINGS_HPP 1


#ifndef PYSVN_STATIC_STRING
#define PYSVN_STATIC_STRING( name, value ) const char name[] = value;
#define PYSVN_STATIC_PY_STRING_P( name ) extern Py::String *name;
#endif

PYSVN_STATIC_STRING( empty_string, "" )
PYSVN_STATIC_STRING( name___members__, "__members__" )
PYSVN_STATIC_STRING( name_action, "action" )
PYSVN_STATIC_STRING( name_add_parents, "add_parents" )
PYSVN_STATIC_STRING( name_adds_as_modification, "adds_as_modification" )
PYSVN_STATIC_STRING( name_allow_mixed_revisions, "allow_mixed_revisions" )
PYSVN_STATIC_STRING( name_allow_unver_obstructions, "allow_unver_obstructions" )
PYSVN_STATIC_STRING( name_annotation, "annotation" )
PYSVN_STATIC_STRING( name_author, "author" )
PYSVN_STATIC_STRING( name_autoprops, "autoprops" )
PYSVN_STATIC_STRING( name_base_revision_for_url, "base_revision_for_url" )
PYSVN_STATIC_STRING( name_callback_cancel, "callback_cancel" )
PYSVN_STATIC_STRING( name_callback_conflict_resolver, "callback_conflict_resolver" )
PYSVN_STATIC_STRING( name_callback_get_log_message, "callback_get_log_message" )
PYSVN_STATIC_STRING( name_callback_get_login, "callback_get_login" )
PYSVN_STATIC_STRING( name_callback_notify, "callback_notify" )
PYSVN_STATIC_STRING( name_callback_progress, "callback_progress" )
PYSVN_STATIC_STRING( name_callback_ssl_client_cert_password_prompt, "callback_ssl_client_cert_password_prompt" )
PYSVN_STATIC_STRING( name_callback_ssl_client_cert_prompt, "callback_ssl_client_cert_prompt" )
PYSVN_STATIC_STRING( name_callback_ssl_server_prompt, "callback_ssl_server_prompt" )
PYSVN_STATIC_STRING( name_callback_ssl_server_trust_prompt, "callback_ssl_server_trust_prompt" )
PYSVN_STATIC_STRING( name_changed_paths, "changed_paths" )
PYSVN_STATIC_STRING( name_changelist, "changelist" )
PYSVN_STATIC_STRING( name_changelists, "changelists" )
PYSVN_STATIC_STRING( name_comment, "comment" )
PYSVN_STATIC_STRING( name_commit_as_operations, "commit_as_operations" )
PYSVN_STATIC_STRING( name_commit_info_style, "commit_info_style" )
PYSVN_STATIC_STRING( name_config_dir, "config_dir" )
PYSVN_STATIC_STRING( name_conflict_choice, "conflict_choice" )
PYSVN_STATIC_STRING( name_copy_as_child, "copy_as_child" )
PYSVN_STATIC_STRING( name_copy_info, "copy_info" )
PYSVN_STATIC_STRING( name_copyfrom_path, "copyfrom_path" )
PYSVN_STATIC_STRING( name_copyfrom_revision, "copyfrom_revision" )
PYSVN_STATIC_STRING( name_created_rev, "created_rev" )
PYSVN_STATIC_STRING( name_date, "date" )
PYSVN_STATIC_STRING( name_depth, "depth" )
PYSVN_STATIC_STRING( name_depth_as_sticky, "depth_as_sticky" )
PYSVN_STATIC_STRING( name_depth_is_sticky, "depth_is_sticky" )
PYSVN_STATIC_STRING( name_dest_path, "dest_path" )
PYSVN_STATIC_STRING( name_dest_url_or_path, "dest_url_or_path" )
PYSVN_STATIC_STRING( name_diff_added, "diff_added" )
PYSVN_STATIC_STRING( name_diff_deleted, "diff_deleted" )
PYSVN_STATIC_STRING( name_diff_options, "diff_options" )
PYSVN_STATIC_STRING( name_dirent_fields, "dirent_fields" )
PYSVN_STATIC_STRING( name_discover_changed_paths, "discover_changed_paths" )
PYSVN_STATIC_STRING( name_dry_run, "dry_run" )
PYSVN_STATIC_STRING( name_enable, "enable" )
PYSVN_STATIC_STRING( name_end_revision, "end_revision" )
PYSVN_STATIC_STRING( name_exception_style, "exception_style" )
PYSVN_STATIC_STRING( name_fetch_locks, "fetch_locks" )
PYSVN_STATIC_STRING( name_force, "force" )
PYSVN_STATIC_STRING( name_from_url, "from_url" )
PYSVN_STATIC_STRING( name_get_all, "get_all" )
PYSVN_STATIC_STRING( name_has_props, "has_props" )
PYSVN_STATIC_STRING( name_header_encoding, "header_encoding" )
PYSVN_STATIC_STRING( name_ignore, "ignore" )
PYSVN_STATIC_STRING( name_ignore_ancestry, "ignore_ancestry" )
PYSVN_STATIC_STRING( name_ignore_content_type, "ignore_content_type" )
PYSVN_STATIC_STRING( name_ignore_eol_style, "ignore_eol_style" )
PYSVN_STATIC_STRING( name_ignore_externals, "ignore_externals" )
PYSVN_STATIC_STRING( name_ignore_keywords, "ignore_keywords" )
PYSVN_STATIC_STRING( name_ignore_mime_type, "ignore_mime_type" )
PYSVN_STATIC_STRING( name_ignore_properties, "ignore_properties" )
PYSVN_STATIC_STRING( name_ignore_space, "ignore_space" )
PYSVN_STATIC_STRING( name_ignore_unknown_node_types, "ignore_unknown_node_types" )
PYSVN_STATIC_STRING( name_ignore_whitespace, "ignore_whitespace" )
PYSVN_STATIC_STRING( name_include_dir_externals, "include_dir_externals" )
PYSVN_STATIC_STRING( name_include_file_externals, "include_file_externals" )
PYSVN_STATIC_STRING( name_include_merged_revisions, "include_merged_revisions" )
PYSVN_STATIC_STRING( name_is_revision, "is_revision" )
PYSVN_STATIC_STRING( name_keep_changelist, "keep_changelist" )
PYSVN_STATIC_STRING( name_keep_local, "keep_local" )
PYSVN_STATIC_STRING( name_keep_locks, "keep_locks" )
PYSVN_STATIC_STRING( name_kind, "kind" )
PYSVN_STATIC_STRING( name_last_author, "last_author" )
PYSVN_STATIC_STRING( name_limit, "limit" )
PYSVN_STATIC_STRING( name_line, "line" )
PYSVN_STATIC_STRING( name_local_change, "local_change" )
PYSVN_STATIC_STRING( name_local_path, "local_path" )
PYSVN_STATIC_STRING( name_lock, "lock" )
PYSVN_STATIC_STRING( name_log_message, "log_message" )
PYSVN_STATIC_STRING( name_make_parents, "make_parents" )
PYSVN_STATIC_STRING( name_merge_options, "merge_options" )
PYSVN_STATIC_STRING( name_merged_author, "merged_author" )
PYSVN_STATIC_STRING( name_merged_path, "merged_path" )
PYSVN_STATIC_STRING( name_merged_revision, "merged_revision" )
PYSVN_STATIC_STRING( name_message, "message" )
PYSVN_STATIC_STRING( name_metadata_only, "metadata_only" )
PYSVN_STATIC_STRING( name_move_as_child, "move_as_child" )
PYSVN_STATIC_STRING( name_name, "name" )
PYSVN_STATIC_STRING( name_native_eol, "native_eol" )
PYSVN_STATIC_STRING( name_node_kind, "node_kind" )
PYSVN_STATIC_STRING( name_notice_ancestry, "notice_ancestry" )
PYSVN_STATIC_STRING( name_number, "number" )
PYSVN_STATIC_STRING( name_password, "password" )
PYSVN_STATIC_STRING( name_patch_abspath, "patch_abspath" )
PYSVN_STATIC_STRING( name_path, "path" )
PYSVN_STATIC_STRING( name_peg_revision, "peg_revision" )
PYSVN_STATIC_STRING( name_prop_changed, "prop_changed" )
PYSVN_STATIC_STRING( name_prop_name, "prop_name" )
PYSVN_STATIC_STRING( name_prop_value, "prop_value" )
PYSVN_STATIC_STRING( name_properties_only, "properties_only" )
PYSVN_STATIC_STRING( name_ranges_to_merge, "ranges_to_merge" )
PYSVN_STATIC_STRING( name_record_only, "record_only" )
PYSVN_STATIC_STRING( name_recurse, "recurse" )
PYSVN_STATIC_STRING( name_relative_to_dir, "relative_to_dir" )
PYSVN_STATIC_STRING( name_remove_tempfiles, "remove_tempfiles" )
PYSVN_STATIC_STRING( name_repos_path, "repos_path" )
PYSVN_STATIC_STRING( name_result_wrappers, "result_wrappers" )
PYSVN_STATIC_STRING( name_reverse, "reverse" )
PYSVN_STATIC_STRING( name_revision, "revision" )
PYSVN_STATIC_STRING( name_revision1, "revision1" )
PYSVN_STATIC_STRING( name_revision2, "revision2" )
PYSVN_STATIC_STRING( name_revision_end, "revision_end" )
PYSVN_STATIC_STRING( name_revision_start, "revision_start" )
PYSVN_STATIC_STRING( name_revprops, "revprops" )
PYSVN_STATIC_STRING( name_show_copies_as_adds, "show_copies_as_adds" )
PYSVN_STATIC_STRING( name_size, "size" )
PYSVN_STATIC_STRING( name_skip_checks, "skip_checks" )
PYSVN_STATIC_STRING( name_sources, "sources" )
PYSVN_STATIC_STRING( name_src_revision, "src_revision" )
PYSVN_STATIC_STRING( name_src_url_or_path, "src_url_or_path" )
PYSVN_STATIC_STRING( name_start_revision, "start_revision" )
PYSVN_STATIC_STRING( name_strict_node_history, "strict_node_history" )
PYSVN_STATIC_STRING( name_strip_count, "strip_count" )
PYSVN_STATIC_STRING( name_summarize_kind, "summarize_kind" )
PYSVN_STATIC_STRING( name_target_wcpath, "target_wcpath" )
PYSVN_STATIC_STRING( name_time, "time" )
PYSVN_STATIC_STRING( name_tmp_path, "tmp_path" )
PYSVN_STATIC_STRING( name_to_url, "to_url" )
PYSVN_STATIC_STRING( name_transaction_name, "transaction_name" )
PYSVN_STATIC_STRING( name_unlock, "unlock" )
PYSVN_STATIC_STRING( name_update, "update" )
PYSVN_STATIC_STRING( name_url, "url" )
PYSVN_STATIC_STRING( name_url_or_path, "url_or_path" )
PYSVN_STATIC_STRING( name_url_or_path1, "url_or_path1" )
PYSVN_STATIC_STRING( name_url_or_path2, "url_or_path2" )
PYSVN_STATIC_STRING( name_use_git_diff_format, "use_git_diff_format" )
PYSVN_STATIC_STRING( name_username, "username" )
PYSVN_STATIC_STRING( name_utf8, "utf-8" )
PYSVN_STATIC_STRING( name_wc_dir_abspath, "wc_dir_abspath" )

PYSVN_STATIC_PY_STRING_P( py_name_callback_cancel )
PYSVN_STATIC_PY_STRING_P( py_name_callback_conflict_resolver )
PYSVN_STATIC_PY_STRING_P( py_name_callback_get_log_message )
PYSVN_STATIC_PY_STRING_P( py_name_callback_get_login )
PYSVN_STATIC_PY_STRING_P( py_name_callback_notify )
PYSVN_STATIC_PY_STRING_P( py_name_callback_ssl_client_cert_password_prompt )
PYSVN_STATIC_PY_STRING_P( py_name_callback_ssl_client_cert_prompt )
PYSVN_STATIC_PY_STRING_P( py_name_callback_ssl_server_prompt )
PYSVN_STATIC_PY_STRING_P( py_name_callback_ssl_server_trust_prompt )
PYSVN_STATIC_PY_STRING_P( py_name_commit_info_style )
PYSVN_STATIC_PY_STRING_P( py_name_created_rev )
PYSVN_STATIC_PY_STRING_P( py_name_exception_style )
PYSVN_STATIC_PY_STRING_P( py_name_has_props )
PYSVN_STATIC_PY_STRING_P( py_name_kind )
PYSVN_STATIC_PY_STRING_P( py_name_last_author )
PYSVN_STATIC_PY_STRING_P( py_name_name )
PYSVN_STATIC_PY_STRING_P( py_name_node_kind )
PYSVN_STATIC_PY_STRING_P( py_name_path )
PYSVN_STATIC_PY_STRING_P( py_name_prop_changed )
PYSVN_STATIC_PY_STRING_P( py_name_repos_path )
PYSVN_STATIC_PY_STRING_P( py_name_size )
PYSVN_STATIC_PY_STRING_P( py_name_summarize_kind )
PYSVN_STATIC_PY_STRING_P( py_name_time )

#undef PYSVN_STATIC_STRING
#undef PYSVN_STATIC_PY_STRING_P

#endif
