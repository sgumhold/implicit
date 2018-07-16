%~d0
cd %~p0
%CGV_BUILD%\bin\cgv_viewer_%CGV_COMPILER:~2%.exe plugin:cg_fltk.dll plugin:crg_stereo_view.dll plugin:crg_grid.dll plugin:crg_light.dll plugin:cg_ext.dll plugin:cg_icons.dll plugin:implicit.dll config:"implicit.def" type(shader_config):shader_path='S:\develop\projects\git\cgv/libs/cgv_gl/glsl' type(scene):file_name='%1'
                                  
pause