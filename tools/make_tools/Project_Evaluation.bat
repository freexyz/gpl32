cd ..
mkdir %1\plutommi\customer\customize\
mkdir %1\plutommi\mmi\debug\
copy customer\customize\GCML.txt %1\plutommi\customer\customize\GCML.txt
copy mmi\debug\PixtelMMI.exe %1\plutommi\mmi\debug\.
copy mmi\debug\mmicustomizer.dll %1\plutommi\mmi\debug\mmicustomizer.dll
copy mmi\debug\mmiresource.dll %1\plutommi\mmi\debug\mmiresource.dll
copy mmi\debug\NWSimulator.dll %1\plutommi\mmi\debug\NWSimulator.dll
copy mmi\debug\QMMINWSimulator.dll %1\plutommi\mmi\debug\QMMINWSimulator.dll
xcopy mmi\Framework\NVRAMManager\NVRAM_DATA\*.* %1\plutommi\mmi\Framework\NVRAMManager\NVRAM_DATA\*.*

cd tool