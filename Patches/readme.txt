This directory contains any patches that need to be applied to 3rd party code
to allow pysvn to build.

svncpp.patch
	- patch against r7073 (also 0.6.0)
	- fix crash with pysvn.Client().log('.')

svncpp2.patch
	- patch against r7073 (also 0.6.0)
	- fix cat to allow binary return data

svncpp3.patch
	- patch against r7073 (also 0.6.0)
	- remove code that causes the default config dir to be created
          when not commanded to do it
        - rapidsvn issue 218

svncpp4.patch
	- patch against r7073 (also 0.6.0)
	- tell the svn auth code where the config_dir is
          otherwise the default location is used rather then 
          the user specified dir
        - rapidsvn issue 219

