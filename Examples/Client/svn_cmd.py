'''
 ====================================================================
 Copyright (c) 2003 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================
'''
import pysvn
import time
import sys
import os
import parse_datetime

class CommandError( Exception ):
	def __init__( self, reason ):
		Exception.__init__( self )
		self._reason = reason

	def reason( self ):
		return self._reason

	def __str__( self ):
		return self._reason

def main( args ):
	progname = os.path.basename( args[0] )

	svn_cmd = SvnCommand( progname )
	return svn_cmd.dispatch( args[1:] )

def fmtDateTime( t ):
	return time.strftime( '%d-%b-%Y %X', time.localtime( t ) )

wc_status_kind_map = {
pysvn.wc_status_kind.added: 'A',
pysvn.wc_status_kind.conflicted: 'C',
pysvn.wc_status_kind.deleted: 'D',
pysvn.wc_status_kind.external: 'X',
pysvn.wc_status_kind.ignored: 'I',
pysvn.wc_status_kind.incomplete: '!',
pysvn.wc_status_kind.missing: '!',
pysvn.wc_status_kind.merged: 'G',
pysvn.wc_status_kind.modified: 'M',
pysvn.wc_status_kind.none: ' ',
pysvn.wc_status_kind.normal: ' ',
pysvn.wc_status_kind.obstructed: '~',
pysvn.wc_status_kind.replaced: 'R',
pysvn.wc_status_kind.unversioned: '?',
}

wc_notify_action_map = {
pysvn.wc_notify_action.add: 'A',
pysvn.wc_notify_action.commit_added: 'A',
pysvn.wc_notify_action.commit_deleted: 'D',
pysvn.wc_notify_action.commit_modified: 'M',
pysvn.wc_notify_action.commit_postfix_txdelta: '?',
pysvn.wc_notify_action.commit_replaced: 'R',
pysvn.wc_notify_action.copy: 'c',
pysvn.wc_notify_action.delete: 'D',
pysvn.wc_notify_action.failed_revert: 'F',
pysvn.wc_notify_action.resolved: 'R',
pysvn.wc_notify_action.restore: 'R',
pysvn.wc_notify_action.revert: 'R',
pysvn.wc_notify_action.skip: '?',
pysvn.wc_notify_action.status_completed: '?',
pysvn.wc_notify_action.status_external: 'X',
pysvn.wc_notify_action.update_add: 'A',
pysvn.wc_notify_action.update_completed: '?',
pysvn.wc_notify_action.update_delete: 'D',
pysvn.wc_notify_action.update_external: 'X',
pysvn.wc_notify_action.update_update: 'U',
}


class SvnCommand:
	def __init__( self, progname ):
		self.progname = progname
		self.client = None

	def initClient( self, config_dir ):
		self.client = c = pysvn.Client( config_dir )
		self.client.callback_get_login = self.callback_getLogin
		self.client.callback_get_log_message = self.callback_getLogMessage
		self.client.callback_notify = self.callback_notify
		self.client.callback_cancel = self.callback_cancel
		self.client.callback_ssl_client_cert_password_prompt = self.callback_ssl_client_cert_password_prompt
		self.client.callback_ssl_client_cert_prompt = self.callback_ssl_client_cert_prompt
		self.client.callback_ssl_server_prompt = self.callback_ssl_server_prompt
		self.client.callback_ssl_server_trust_prompt = self.callback_ssl_server_trust_prompt


	def callback_ssl_client_cert_password_prompt( self ):
		print 'callback_ssl_client_cert_password_prompt'

	def callback_ssl_client_cert_prompt( self ):
		print 'callback_ssl_client_cert_prompt'

	def callback_ssl_server_prompt( self ):
		print 'callback_ssl_server_prompt'

	def callback_ssl_server_trust_prompt( self, trust_data ):
		for key,value in trust_data.items():
			print '%s: %s' % (key, value)
		print
		answer = ''
		while answer.lower() not in ['p','t','r']:
			sys.stdout.write( '(P)ermanent accept, (T)emporary accept or (R)eject: ' )
			answer = sys.stdin.readline().strip()
		if answer.lower() == 'p':
			return 2, trust_data['failures']
		if answer.lower() == 't':
			return 1, trust_data['failures']
		return 0, 0


	def callback_cancel( self ):
		return False

	def callback_notify( self, arg_dict ):
		if arg_dict['path'] != '':
			print '%s %s' % (wc_notify_action_map[ arg_dict['action'] ], arg_dict['path'])

	def callback_getLogin( self, realm, username, may_save ):
		print 'May save:',may_save
		print 'Realm:',realm
		if username:
			print 'Username:',username
		else:
			sys.stdout.write( 'Username: ' )
			username = sys.stdin.readline().strip()
			if len(username) == 0:
				return 0, '', '', False

		sys.stdout.write( 'Password: ' )
		password = sys.stdin.readline().strip()

		save_password = 'x'
		while save_password.lower() not in ['y','ye','yes','n', 'no','']:
			sys.stdout.write( 'Save password? [y/n] ' )
			save_password = sys.stdin.readline().strip()
		
		return 1, username, password, save_password in ['y','ye','yes']

	def getLogMessage( self ):
		sys.stdout.write( 'Log message\n' )
		sys.stdout.write( '--- -------\n' )
		message = sys.stdin.read()
		return message

	def callback_getLogMessage( self ):
		return 1, self.getLogMessage()

	def dispatch( self, argv ):

		try:
			args = SvnArguments( argv )
			cmd_name = 'cmd_%s' % args.getCommandName( 'help' )

			self.initClient( args.getOptionalValue( '--config-dir', '' ) )
			self.client.set_auth_cache( args.getBooleanOption( '--no-auth-cache', False ) )

			getattr( self, cmd_name, self.cmd_help )( args )
		except pysvn.ClientError, e:
			print e
			return 1
		except CommandError, e:
			print e.reason()
			return 1

		return 0

	def cmd_is_url( self, args ):
		path = args.getPositionalArgs( 1 )[0]
		is_url = self.client.is_url( path )
		if is_url:
			print 'url',path
		else:
			print 'path',path

	def cmd_add( self, args ):
		recurse = args.getBooleanOption( '--non-recursive', False )
		
		self.client.add( args.getPositionalArgs( 1 ), recurse=recurse )

	def cmd_annotate( self, args ):
		start_revision, end_revision = args.getOptionalRevisionPair( '--revision', '0', 'head' )
		positional_args = args.getPositionalArgs( 1, 1 )
		all_lines = self.client.annotate( positional_args[0],
					revision_start=start_revision,
					revision_end=end_revision )

		for line in all_lines:
			print '%d| r%d | %s | %s | %s' % \
				(line['number'],
				line['revision'].number,
				line['author'],
				line['date'],
				line['line'])
	cmd_ann = cmd_annotate

	def cmd_cat( self, args ):
		revision = args.getOptionalRevision( '--revision', 'base' )
		text = self.client.cat( args.getPositionalArgs( 1, 1 )[0], revision=revision )
		print text

	def cmd_checkout( self, args ):
		recurse = args.getBooleanOption( '--non-recursive', False )
		positional_args = args.getPositionalArgs( 1, 2 )
		if len(positional_args) == 1:
			positional_args.append( os.path.basename( positional_args[0] ) )

		self.client.checkout( positional_args[0], positional_args[1], recurse=recurse )
	cmd_co = cmd_checkout

	def cmd_cleanup( self, args ):
		positional_args = args.getPositionalArgs( 0, 1 )
		if len(positional_args) == 0:
			positional_args.append( '.' )

		self.client.cleanup( positional_args[0] )

	def cmd_checkin( self, args ):
		msg = args.getOptionalValue( '--message', '' )
		recurse = args.getBooleanOption( '--non-recursive', False )
		positional_args = args.getPositionalArgs( 0 )
		if len(positional_args) == 0:
			positional_args.append( '.' )
		if msg == '':
			msg = self.getLogMessage()
			
		rev = self.client.checkin( positional_args, msg, recurse=recurse )
		if rev.number > 0:
			print 'Revision',rev.number
		else:
			print 'Commit failed'

	cmd_commit = cmd_checkin
	cmd_ci = cmd_checkin

	def cmd_copy( self, args ):
		positional_args = args.getPositionalArgs( 2, 2 )
		self.client.copy( positional_args[0], positional_args[1] )
	cmd_cp = cmd_copy

	def cmd_diff( self, args ):
		recurse = args.getBooleanOption( '--non-recursive', False )
		positional_args = args.getPositionalArgs( 0, 1 )
		if len(positional_args) == 0:
			positional_args.append( '.' )

		if os.environ.has_key('TEMP'):
			tmpdir = os.environ['TEMP']
		elif os.environ.has_key('TMPDIR'):
			tmpdir = os.environ['TMPDIR']
		else:
			tmpdir = '/usr/tmp/svn_tmp'

		diff_text = self.client.diff( tmpdir, positional_args[0], recurse=recurse )
		print diff_text

	def cmd_export( self, args ):
		revision = args.getOptionalRevision( '--revision', 'base' )
		positional_args = args.getPositionalArgs( 2, 2 )
		self.client.export( positional_args[0], positional_args[1], revision=revision )

	def cmd_info( self, args ):
		positional_args = args.getPositionalArgs( 0, 1 )
		if len(positional_args) == 0:
			positional_args.append( '.' )
		path = positional_args[0]

		entry = self.client.info( path )

		print 'Path:',path
		if entry.name and entry.name != 'svn:this_dir':
			print 'Name:',entry.name
		if entry.url:
			print 'Url:',entry.url
		if entry.repos:
			print 'Repository:',entry.repos
		if entry.uuid:
			print 'Repository UUID:',entry.uuid
		if entry.revision.kind == pysvn.opt_revision_kind.number:
			print 'Revision:',entry.revision.number
		if entry.kind == pysvn.node_kind.file:
			print 'Node kind: file'
		elif entry.kind == pysvn.node_kind.dir:
			print 'Node kind: directory'
		elif entry.kind == pysvn.node_kind.none:
			print 'Node kind: none'
		else:
			print 'Node kind: unknown'

		if entry.schedule == pysvn.wc_schedule.normal:
			print "Schedule: normal"
		elif entry.schedule == pysvn.wc_schedule.add:
			print "Schedule: add"
		elif entry.schedule == pysvn.wc_schedule.delete:
			print "Schedule: delete"
		elif entry.schedule == pysvn.wc_schedule.replace:
			print "Schedule: replace"
		if entry.is_copied:
			if entry.copyfrom_url:
				print 'Copied From URL:', entry.copyfrom_url
			if entry.copyfrom_rev.number:
				print 'Copied From Rev:', entry.copyfrom_rev.number
		if entry.commit_author:
			print 'Last Changed Author:', entry.commit_author
		if entry.commit_revision.number:
			print 'Last Changed Rev:', entry.commit_revision.number
		if entry.commit_time:
			print 'Last Changed Date:', fmtDateTime( entry.commit_time )
		if entry.text_time:
			print 'Text Last Updated:', fmtDateTime( entry.text_time )
		if entry.properties_time:
			print 'Properties Last Updated:', fmtDateTime( entry.properties_time )
		if entry.checksum:
			print 'Checksum:', entry.checksum

	def cmd_import( self, args ):
		msg = self.getOptionalValue( '--message', '' )
		recurse = args.getBooleanOption( '--non-recursive', False )
		positional_args = args.getPositionalArgs( 2, 2 )
		self.client.import_( positional_args[0], positional_args[1], recurse=recurse )

	def cmd_log( self, args ):
		start_revision, end_revision = args.getOptionalRevisionPair( '--revision', 'head', '0' )
		positional_args = args.getPositionalArgs( 1, 1 )
		all_logs = self.client.log( positional_args[0],
					revision_start=start_revision, revision_end=end_revision )

		for log in all_logs:
			print '-'*60
			print 'rev %d: %s | %s | %d lines' % \
				(log['revision'].number, log['author'], log['date'],
				len(log['message'].split('\n')))
			print log['message']
		print '-'*60

	def cmd_ls( self, args ):
		recurse = args.getBooleanOption( '--recursive', True )
		revision = args.getOptionalRevision( '--revision', 'head' )
		verbose = args.getBooleanOption( '--verbose', True )
		positional_args = args.getPositionalArgs( 0 )
		if len(positional_args) == 0:
			positional_args.append( '.' )

		for arg in positional_args:
			all_files = self.client.ls( arg, revision=revision, recurse=recurse )
			if verbose:
				for file in all_files:
					file['time_str'] = fmtDateTime( file['time'] )
					print '%(created_rev)7d %(last_author)-10s %(size)6d %(time_str)s %(name)s' % file
			else:
				for file in all_files:
					print '%(name)s' % file

	def cmd_merge( self, args ):
		recurse = args.getBooleanOption( '--recursive', True )
		dry_run = args.getBooleanOption( '--dry-run', False )
		notice_ancestry = args.getBooleanOption( '--notice-ancestry', False )

		# need to figure out which variaty of the merge command this is
		if args.haveOption( '--revision' ):
			# its merge -r N:M SOURCE [WCPATH]
			revision1, revision2 = args.getMandatoryRevisionPair( '--revision' )
			positional_args = args.getPositionalArgs( 1, 2 )
			if len(positional_args) == 1:
				positional_args.append( '.' )
			path1 = positional_args[0]
			path2 = positional_args[0]
			wcpath = positional_args[1]
		else:
			# its merge sourceURL1[@N] sourceURL2[@M] [WCPATH]
			positional_args = args.getPositionalArgs( 2, 3 )
			if len(positional_args) == 2:
				positional_args.append( '.' )

			path1, rev1 = self.parsePathWithRevision( positional_args[0] )
			path2, rev2 = self.parsePathWithRevision( positional_args[1] )
			wcpath = positional_args[2]

			
		self.client.merge( path1, revision1, path2, revision2, wcpath,
				recurse=recurse, dry_run=dry_run, notice_ancestry=notice_ancestry )

	def cmd_mkdir( self, args ):
		msg = args.getOptionalValue( '--message', '' )
		if msg == '':
			msg = self.getLogMessage()

		self.client.mkdir( args.getPositionalArgs( 1, 1 )[0], msg )

	def cmd_move( self, args ):
		positional_args = args.getPositionalArgs( 2, 2 )
		self.client.move( positional_args[0], positional_args[1] )
	cmd_mv = cmd_move

	def props_by_path( self, a, b ):
		return cmp( a[0], b[0] )


	def cmd_proplist( self, args ):
		recurse = args.getBooleanOption( '--recursive', True )
		revision = args.getOptionalRevision( '--revision', 'working' )
		verbose = args.getBooleanOption( '--verbose', True )
		positional_args = args.getPositionalArgs( 0, 1 )
		if len(positional_args) == 0:
			positional_args.append( '.' )

		if self.client.is_url( positional_args[0] ):
			revision = args.getOptionalRevision( '--revision', 'head' )

		all_props = self.client.proplist( positional_args[0], revision=revision, recurse=recurse )
		all_props.sort( self.props_by_path )

		for path, props in all_props:
			print "Properties on '%s':" % path
			prop_names = props.keys()
			prop_names.sort()
			for name in prop_names:
				if verbose:
					print '  %s: %s' % (name, props[name])
				else:
					print '  %s' % name
			
	cmd_pl = cmd_proplist

	def cmd_propget( self, args ):
		recurse = args.getBooleanOption( '--recursive', True )
		revision = args.getOptionalRevision( '--revision', 'working' )
		positional_args = args.getPositionalArgs( 1, 2 )
		if len(positional_args) == 1:
			positional_args.append( '.' )
		if self.client.is_url( positional_args[0] ):
			revision = args.getOptionalRevision( '--revision', 'head' )

		all_props = self.client.propget( positional_args[0], positional_args[1], revision=revision, recurse=recurse )
		all_props.sort( self.props_by_path )

		for path, props in all_props:
			prop_names = props.keys()
			prop_names.sort()
			for name in prop_names:
				print '%s - %s' % (path, props[name])

	cmd_pg = cmd_propget

	def cmd_propset( self, args ):
		recurse = args.getBooleanOption( '--recursive', True )
		revision = args.getOptionalRevision( '--revision', 'working' )
		verbose = args.getBooleanOption( '--verbose', True )
		positional_args = args.getPositionalArgs( 2, 3 )
		if len(positional_args) == 2:
			positional_args.append( '.' )
		if self.client.is_url( positional_args[0] ):
			revision = args.getOptionalRevision( '--revision', 'head' )

		self.client.propset( positional_args[0], positional_args[1], positional_args[2], revision=revision, recurse=recurse )
	cmd_ps = cmd_propset

	def cmd_propdel( self, args ):
		recurse = args.getBooleanOption( '--recursive', True )
		revision = args.getOptionalRevision( '--revision', 'working' )
		verbose = args.getBooleanOption( '--verbose', True )
		positional_args = args.getPositionalArgs( 1, 2 )
		if len(positional_args) == 1:
			positional_args.append( '.' )
		if self.client.is_url( positional_args[0] ):
			revision = args.getOptionalRevision( '--revision', 'head' )

		self.client.propdel( positional_args[0], positional_args[1], revision=revision, recurse=recurse )
	cmd_pd = cmd_propdel

	def cmd_revproplist( self, args ):
		revision = args.getOptionalRevision( '--revision', 'working' )
		verbose = args.getBooleanOption( '--verbose', False )
		positional_args = args.getPositionalArgs( 0, 1 )
		if len(positional_args) == 0:
			positional_args.append( '.' )
		if self.client.is_url( positional_args[0] ):
			revision = args.getOptionalRevision( '--revision', 'head' )

		rev, prop_dict = self.client.revproplist( positional_args[0], revision=revision )
		print rev,repr(prop_dict)
	cmd_rpl = cmd_revproplist

	def cmd_revpropget( self, args ):
		revision = args.getOptionalRevision( '--revision', 'working' )
		positional_args = args.getPositionalArgs( 1, 2 )
		if len(positional_args) == 1:
			positional_args.append( '.' )
		if self.client.is_url( positional_args[0] ):
			revision = args.getOptionalRevision( '--revision', 'head' )

		rev, value = self.client.revpropget( positional_args[0], positional_args[1], revision=revision )
		print rev,value
	cmd_rpg = cmd_revpropget

	def cmd_revpropset( self, args ):
		force = args.getBooleanOption( '--force', False )
		revision = args.getOptionalRevision( '--revision', 'working' )
		positional_args = args.getPositionalArgs( 2, 3 )
		if len(positional_args) == 2:
			positional_args.append( '.' )
		if self.client.is_url( positional_args[0] ):
			revision = args.getOptionalRevision( '--revision', 'head' )

		rev = self.client.revpropset( positional_args[0], positional_args[1], positional_args[2], revision=revision, force=force )
	cmd_rps = cmd_revpropset

	def cmd_revpropdel( self, args ):
		force = args.getBooleanOption( '--force', False )
		revision = args.getOptionalRevision( '--revision', 'working' )
		positional_args = args.getPositionalArgs( 1, 2 )
		if len(positional_args) == 1:
			positional_args.append( '.' )
		if self.client.is_url( positional_args[0] ):
			revision = args.getOptionalRevision( '--revision', 'head' )

		self.client.revpropdel( positional_args[0], positional_args[1], revision=revision, force=force )
	cmd_rpd = cmd_revpropdel

	def cmd_remove( self, args ):
		force = args.getBooleanOption( '--force', True )
		positional_args = args.getPositionalArgs( 1, 0 )
		self.client.remove( positional_args, force=force )
	cmd_rm = cmd_remove

	def cmd_resolved( self, args ):
		recurse = args.getBooleanOption( '--recursive', True )
		positional_args = args.getPositionalArgs( 1, 1 )
		self.client.resolved( positional_args[0], recurse=recurse )

	def cmd_revert( self, args ):
		recurse = args.getBooleanOption( '--recursive', True )
		positional_args = args.getPositionalArgs( 1, 1 )
		self.client.revert( positional_args[0], recurse=recurse )

	def by_path( self, a, b ):
		return cmp( a.path, b.path )

	def cmd_status( self, args ):
		recurse = args.getBooleanOption( '--non-recursive', False )
		verbose = args.getBooleanOption( '--verbose', True )
		no_ignore = args.getBooleanOption( '--no-ignore', False )
		update = args.getBooleanOption( '--show-updates', True )

		positional_args = args.getPositionalArgs( 0 )
		if len(positional_args) == 0:
			all_files = self.client.status( '', recurse=recurse, get_all=verbose, no_ignore=no_ignore, update=update )
			self._cmd_status_print( all_files, verbose, update )
		else:
			for arg in positional_args:
				all_files = self.client.status( arg, recurse=recurse, get_all=verbose, no_ignore=no_ignore, update=update )
				self._cmd_status_print( all_files, verbose, update )

	def _cmd_status_print( self, all_files, detailed, show_committed ):
		all_files.sort( self.by_path )
		for file in all_files:
			state = '%s%s%s%s%s' % (wc_status_kind_map[ file.text_status ],
					wc_status_kind_map[ file.prop_status ],
					' L'[ file.is_locked ],
					' +'[ file.is_copied ],
					' S'[ file.is_switched ])


			if( file.repos_text_status != pysvn.wc_status_kind.none
			or  file.repos_prop_status != pysvn.wc_status_kind.none ):
				odd_status = '*'
			else:
				odd_status = ' '

			if detailed:
				print '%s %s %6d %6d %-14s %s' % (state,
					odd_status,
					file.entry.revision.number,
					file.entry.commit_revision.number,
					file.entry.commit_author,
					file.path)
			elif show_committed:
				print '%s %s %6d %s' % (state,
					odd_status,
					file.entry.revision.number,
					file.path)
			else:
				if file.text_status != pysvn.wc_status_kind.normal:
					print '%s %s' % (state, file.path)

	cmd_st = cmd_status
	cmd_stat = cmd_status

	def cmd_switch( self, args ):
		recurse = args.getBooleanOption( '--non-recursive', False )
		revision = args.getOptionalRevision( '--revision', 'head' )
		positional_args = args.getPositionalArgs( 1, 2 )
		if len(positional_args) == 1:
			positional_args.append( '.' )
		self.client.switch( positional_args[0], positional_args[1],
				recurse=recurse, revision=revision )

	def cmd_relocate( self, args ):
		recurse = args.getBooleanOption( '--non-recursive', False )
		positional_args = args.getPositionalArgs( 2, 3 )
		if len(positional_args) == 2:
			positional_args.append( '.' )
		self.client.relocate( positional_args[2], positional_args[0],
				positional_args[1], recurse=recurse )

	def cmd_update( self, args ):
		recurse = args.getBooleanOption( '--non-recursive', False )
		positional_args = args.getPositionalArgs( 0 )
		if len(positional_args) == 0:
			positional_args.append( '.' )
		rev = self.client.update( positional_args[0], recurse=recurse )
		if rev.number > 0:
			print 'Updated to revision',rev.number
		else:
			print 'Update failed'
	cmd_up = cmd_update

	def cmd_help( self, args ):
		print 'Version: pysvn %d.%d.%d-%d' % pysvn.version,'svn %d.%d.%d-%s' % pysvn.svn_version
		valid_cmd_names = [name for name in SvnCommand.__dict__.keys() if name.find('cmd_') == 0]
		valid_cmd_names.sort()
		print 'Available subcommands:'
		for index, name in enumerate(valid_cmd_names):
			print '   %-12s' % name[4:],
			if index % 4 == 3:
				print

# key is long option name, value is need for arg
long_opt_info = {
	'--auto-props': 0,	# enable automatic properties
	'--config-dir': 1,	# read user configuration files from directory ARG
	'--diff-cmd': 1,	# use ARG as diff command
	'--diff3-cmd': 1,	# use ARG as merge command
	'--dry-run': 0,		# try operation but make no changes
	'--editor-cmd': 1,	# use ARG as external editor
	'--encoding': 1,	# treat value as being in charset encoding ARG
	'--force': 0,		# force operation to run
	'--force-log': 0,	# force validity of log message source
	'--incremental': 0,	# give output suitable for concatenation
	'--new': 1,		# use ARG as the newer target
	'--no-auth-cache': 0,	# do not cache authentication tokens
	'--no-auto-props': 0,	# disable automatic properties
	'--no-diff-deleted': 0,	# do not print differences for deleted files
	'--no-ignore': 0,	# disregard default and svn:ignore property ignores
	'--non-interactive': 0,	# do no interactive prompting
	'--notice-ancestry': 0,	# notice ancestry when calculating differences
	'--old': 1,		# use ARG as the older target
	'--password': 1,	# specify a password ARG
	'--relocate': 0,	# relocate via URL-rewriting
	'--revprop': 0,		# operate on a revision property (use with -r)
	'--strict': 0,		# use strict semantics
	'--targets': 1,		# pass contents of file ARG as additional args
	'--username': 1,	# specify a username ARG
	'--version': 0,		# print client version info
	'--xml': 0,		# output in xml
	'--file': 1,		# read data from file ARG
	'--non-recursive': 0,	# operate on single directory only
	'--recursive': 0,	# descend recursively
	'--message': 1,		# specify commit message ARG
	'--quiet': 0,		# print as little as possible
	'--revision': 1,	# revision X or X:Y range.  X or Y can be one of:
	'--show-updates': 0,	# display update information
	'--verbose': 0,		# print extra information
	'--extensions': 1,	# pass ARG as bundled options to GNU diff

}

# map short name to long
short_opt_info = {
	'-F': '--file',
	'-N': '--non-recursive',
	'-R': '--recursive',
	'-m': '--message',
	'-q': '--quiet',
	'-r': '--revision',
	'-u': '--show-updates',
	'-v': '--verbose',
	'-x': '--extensions',
}


#
# Usage:
#	Construct with a commnad list
#	call getCommandName()
#	call getBooleanOption() and getOptionalValue() as needed
#	finally call getPositionalArgs()
#
#
class SvnArguments:
	def __init__( self, all_args ):
		self.positional_args = []
		self.named_options = {}
		self.used_named_options = {}

		need_next_arg = 0
		name = ''

		for arg in all_args:
			if need_next_arg:
				self.named_options[ name ] = arg
				need_next_arg = 0

			elif self._isOption( arg ):
				name, need_next_arg = self._optionInfo( arg )
				if not need_next_arg:
					self.named_options[ name ] = None

			else:
				self.positional_args.append( arg )
		if need_next_arg:
			raise CommandError, 'Missing arg to option %s' % name

	def _isOption( self, arg ):
		return arg[0] == '-'
			
	def _optionInfo( self, opt ):
		# return long_name, arg_needed
		long_opt = short_opt_info.get( opt, opt )
		if long_opt_info.has_key( long_opt ):
			return long_opt, long_opt_info[ long_opt ]
		raise CommandError, 'unknown option %s' % opt

	def _checkOptionsUsed( self ):
		# check all options have been used
		for opt_name in self.named_options.keys():
			if not self.used_named_options.has_key( opt_name ):
				raise CommandError, 'unused option %s' % opt_name

	def parsePathWithRevision( self, path_rev, default_rev ):
		if '@' in path_rev:
			path = path_rev[:path_rev.find('@')]
			rev = self._parseRevisionArg( path_rev[path_rev.find('@')+1:] )
		else:
			path = path_rev
			rev = self._parseRevisionArg( default_rev )
		return path, rev

	def _parseRevisionArg( self, rev_string ):
		if rev_string.lower() == 'base':
			return pysvn.Revision( pysvn.opt_revision_kind.base )
		if rev_string.lower() == 'head':
			return pysvn.Revision( pysvn.opt_revision_kind.head )
		if rev_string.lower() == 'working':
			return pysvn.Revision( pysvn.opt_revision_kind.working )
		if rev_string.lower() == 'committed':
			return pysvn.Revision( pysvn.opt_revision_kind.committed )
		if rev_string.lower() == 'prev':
			return pysvn.Revision( pysvn.opt_revision_kind.prev )
		if rev_string[0] == '{' and rev_string[-1] == '}':
			try:
				date = parse_datetime.parse_time( rev_string[1:-2] )
				return pysvn.Revision( pysvn.opt_revision_kind.date, date )
			except parse_datetime.DateTimeSyntaxError, e:
				raise CommandError, e.reason()
		# either a rev number or a date
		try:
			return pysvn.Revision( pysvn.opt_revision_kind.number, int(rev_string) )
		except ValueError:
			pass
		raise CommandError, 'Cannot parse %s as a revision value' % rev_string


	def _splitRevisionString( self, rev_string ):
		# split the string at the first : that is not inside a {} pair
		if rev_string[0] == '{':
			# the : may be after the closing }
			close_paren_index = rev_string.find( '}' )
			if close_paren_index == -1:
				# error leave to others to report
				return [rev_string]

			if close_paren_index == len(rev_string ):
				# its just one revision
				return [rev_string]

			if rev_string[close_paren_index+1] == ':':
				return [rev_string[:close_paren_index+1], rev_string[close_paren_index+2:]]

			# another error case
			return [rev_string]

	def getCommandName( self, default_command ):
		if len(self.positional_args) > 0:
			return self.positional_args.pop( 0 )
		else:
			return default_command

	def haveOption( self, opt_name ):
		return self.named_options.has_key( opt_name )

	def getBooleanOption( self, opt_name, present_value=True ):
		if self.named_options.has_key( opt_name ):
			self.used_named_options[ opt_name ] = None
			return present_value
		else:
			return not present_value

	def getOptionalValue( self, opt_name, default ):
		if self.named_options.has_key( opt_name ):
			self.used_named_options[ opt_name ] = None
			return self.named_options[ opt_name ]
		else:
			return default

	def getOptionalRevision( self, opt_name, start_default ):
		if self.named_options.has_key( opt_name ):
			self.used_named_options[ opt_name ] = None
			rev_string = self.named_options[ opt_name ]

			return self._parseRevisionArg( rev_string )
		else:
			return self._parseRevisionArg( start_default )

	def getMandatoryRevisionPair( self, opt_name ):
		# parse a M:N or M as revision pair
		if not self.named_options.has_key( opt_name ):
			raise CommandError,'mandatory %s required' % opt_name

		self.used_named_options[ opt_name ] = None

		rev_strings = self._splitRevisionString( self.named_options[ opt_name ] )
		if len(rev_strings) == 1:
			raise CommandError,'mandatory %s requires a pair of revisions' % opt_name

		return [self._parseRevisionArg( rev_string[0] ),
			self._parseRevisionArg( rev_string[1] )]

	def getOptionalRevisionPair( self, opt_name, start_default, end_default=None ):
		# parse a M:N or M as revision pair
		if self.named_options.has_key( opt_name ):
			self.used_named_options[ opt_name ] = None
			rev_strings = self._splitRevisionString( self.named_options[ opt_name ] )
			if len(rev_strings) == 1:
				if end_default is None:
					# M means M:M
					rev_strings.append( rev_strings[0] )
				else:
					# M means M:end_default
					rev_strings.append( end_default )

			return [self._parseRevisionArg( rev_string[0] ),
				self._parseRevisionArg( rev_string[1] )]
		else:
			return (self._parseRevisionArg( start_default ),
				self._parseRevisionArg( end_default ))

	def getPositionalArgs( self, min_args, max_args=0 ):
		# check min and max then return the list
		if len(self.positional_args) < min_args:
			raise CommandError, 'too few arguments - need atlease %d' % min_args
		if max_args != 0 and len(self.positional_args) > max_args:
			raise CommandError, 'too many arguments - need no more then %d' % max_args

		# as this is the last call on the args object we check the option where all used
		self._checkOptionsUsed()

		return self.positional_args

if __name__ == '__main__':
	sys.exit( main( sys.argv ) )
