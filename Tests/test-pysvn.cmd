rem test 01 test commands
call test-01.cmd
rem test 02 is not a regression test
rem can only run 03 on barry's LAN
if "%USERNAME%" == "barry" call test-03.cmd
