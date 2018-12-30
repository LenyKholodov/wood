@echo Rebuild %1
@cd ..\%1 
@call rebuild.bat 
@cd ..\.Build
@echo End of rebuild %1