%~d0
cd %~p0
%CGV_BUILD%\bin\release64\implicit64_%CGV_COMPILER:~2%.exe config:implicit.def type(scene):file_name='%1'
pause