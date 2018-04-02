#!/usr/bin/env python3
import sys
import winreg

def main( argv ):
    for arch, view in [('Win32', winreg.KEY_WOW64_32KEY), ('Win64', winreg.KEY_WOW64_64KEY)]:
        for level, key in [('HKLM', winreg.HKEY_LOCAL_MACHINE), ('HKCU', winreg.HKEY_CURRENT_USER)]:
            # look for python in the registry
            try:
                for vendor_key in (r'SOFTWARE\Python\PythonCore', r'SOFTWARE\Python\ContinuumAnalytics'):
                    core_key = winreg.OpenKey( key, vendor_key, 0, winreg.KEY_READ | view )
                    try:
                        index = 0
                        while True:
                            py_name = winreg.EnumKey( core_key, index )
                            py_key = winreg.OpenKey( core_key, py_name, 0, winreg.KEY_READ | view )
                            install_key = winreg.OpenKey( py_key, 'InstallPath', 0, winreg.KEY_READ | view )
                            install_path, value_type = winreg.QueryValueEx( install_key, None )
                            print( arch, py_name, install_path )

                            index += 1

                    except WindowsError:
                        pass

                    winreg.CloseKey( core_key )

            except WindowsError:
                print( '%s not found in %s for %s' % (vendor_key, level, arch) )

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
