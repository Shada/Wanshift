@echo off

copy lib\Renderer.dll Debug
copy lib\Renderer.lib Debug
copy lib\WanInput.dll Debug
copy lib\WanInput.lib Debug

mkdir Debug\Fonts
copy StortSpelprojekt\Fonts\Myriad-Pro.png Debug\Fonts
copy StortSpelprojekt\Fonts\Myriad-Pro.txt Debug\Fonts

mkdir Debug\Models
copy StortSpelprojekt\Models\119498958977780800stop_sign_right_font_mig_.svg.med.png Debug\Models
copy StortSpelprojekt\Models\Candycane.fbx Debug\Models
copy StortSpelprojekt\Models\cone.fbx Debug\Models
copy StortSpelprojekt\Models\legoBrick.fbx Debug\Models
copy StortSpelprojekt\Models\Rock1.fbx Debug\Models
copy StortSpelprojekt\Models\saladfingers.FBX Debug\Models
copy StortSpelprojekt\Models\StopSign.fbx Debug\Models
copy StortSpelprojekt\Models\Talia.fbx Debug\Models

mkdir Debug\x86\debug
copy lib\x86\debug\libfbxsdk.dll Debug\x86\debug

copy "C:\Program Files (x86)\Windows Kits\8.0\Redist\D3D\x86\d3dcompiler_46.dll" Debug

cd D:\Bamboo\xml-data\build-dir\WAN-NB-JOB1\Debug
StortSpelprojekt.exe --gtest_output=xml:testresult.xml

exit 0