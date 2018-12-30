echo Recompile all models
@for %%a in (*.mmk) do 3ds2mdl.exe %%a
echo Recompile sprites
makesprite sprite1.txt models/sprite1.mdl