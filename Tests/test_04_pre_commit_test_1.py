import sys
import pysvn
print( 'Info: pre commit test 1' )

print( 'Info: Transaction( %s, %s) ...' % (sys.argv[1], sys.argv[2]) )
t = pysvn.Transaction( sys.argv[1], sys.argv[2] )

print( 'Info: revproplist() ...' )
all_props = t.revproplist()
for name, value in all_props.items():
    print( '%s: %s' % (name, value) )

print(  'Info: changed() ...' )
changes = t.changed()
change_list = list( changes.items() )
change_list.sort()
for name, (action, kind, text_mod, prop_mod) in change_list:
    print( '%s: action=%r, kind=%r, text_mod=%r, prop_mod=%r' % (name, action, kind, text_mod, prop_mod) )
    if action != 'D':
        all_props = t.proplist( name )
        for prop_name, prop_value in all_props.items():
            print( '     %s: %s' % (prop_name, prop_value) )
        if kind == pysvn.node_kind.file:
            print( '     contents: %r' % t.cat( name ) )

print( 'Info: changed( copy_info=True ) ...' )
changes = t.changed( copy_info=True )
change_list = list( changes.items() )
change_list.sort()
for name, (action, kind, text_mod, prop_mod, copyfrom_rev, copyfrom_path) in change_list:
    print( '%s: action=%r, kind=%r, text_mod=%r, prop_mod=%r copyfrom_rev=%r copyfrom_path=%r' %
            (name, action, kind, text_mod, prop_mod, copyfrom_rev, copyfrom_path) )
    if action != 'D':
        all_props = t.proplist( name )
        for prop_name, prop_value in all_props.items():
            print( '     %s: %s' % (prop_name, prop_value) )
        if kind == pysvn.node_kind.file:
            print( '     contents: %r' % t.cat( name ) )

sys.exit( 0 )
