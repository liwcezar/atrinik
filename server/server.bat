rem Run server script.

if not exist "lib" (
	echo Creating lib directory...
	md lib
)

if not exist "data" (
	echo Creating data directory...
	xcopy /s/e install data
)

copy ..\arch\*.* lib\*.*
del data\tmp\*

atrinik_server.exe -log logfile.log