if "%1" == "" goto ERROR

echo Copy application
xcopy "..\..\application\app_audio_codec" 			"%1\application\app_audio_codec" /s /i /y
xcopy "..\..\application\app_image_codec" 			"%1\application\app_image_codec" /s /i /y
xcopy "..\..\application\app_video_codec" 			"%1\application\app_video_codec" /s /i /y
xcopy "..\..\application\card_detect" 					"%1\application\card_detect" /s /i /y
xcopy "..\..\application\gpdemo" 								"%1\application\gpdemo" /s /i /y
xcopy "..\..\application\app_power_down" 			"%1\application\app_power_down" /s /i /y
xcopy "..\..\application\gpid\game_mode\inc" 		"%1\application\gpid\game_mode\inc" /s /i /y
xcopy "..\..\application\gpid\inc" 							"%1\application\gpid\inc" /s /i /y
xcopy "..\..\application\gpid\res_bin_file\inc" 	"%1\application\gpid\res_bin_file\inc" /s /i /y
xcopy "..\..\application\gpid\src" 							"%1\application\gpid\src" /s /i /y
xcopy "..\..\application\inc" 									"%1\application\inc" /s /i /y
xcopy "..\..\application\msg_manager" 					"%1\application\msg_manager" /s /i /y
xcopy "..\..\application\multimedia_platform" 	"%1\application\multimedia_platform" /s /i /y
xcopy "..\..\application\task_audio_dac" 				"%1\application\task_audio_dac" /s /i /y
xcopy "..\..\application\task_audio_decoder" 		"%1\application\task_audio_decoder" /s /i /y
xcopy "..\..\application\task_audio_record" 		"%1\application\task_audio_record" /s /i /y
xcopy "..\..\application\task_avi_encoder" 			"%1\application\task_avi_encoder" /s /i /y
xcopy "..\..\application\task_filesrv" 					"%1\application\task_filesrv" /s /i /y
xcopy "..\..\application\task_image" 						"%1\application\task_image" /s /i /y
xcopy "..\..\application\task_umi" 							"%1\application\task_umi" /s /i /y
xcopy "..\..\application\task_video_decoder" 		"%1\application\task_video_decoder" /s /i /y

echo Copy driver1
xcopy "..\..\driver_l1\common" 									"%1\driver_l1\common" /s /i /y
xcopy "..\..\driver_l1\gif\inc" 								"%1\driver_l1\gif\inc" /s /i /y
xcopy "..\..\driver_l1\inc" 										"%1\driver_l1\inc" /s /i /y
xcopy "..\..\driver_l1\efuse\inc" 							"%1\driver_l1\efuse\inc" /s /i /y
xcopy "..\..\driver_l1\jpeg\inc" 								"%1\driver_l1\jpeg\inc" /s /i /y
xcopy "..\..\driver_l1\mipi" 										"%1\driver_l1\mipi" /s /i /y
xcopy "..\..\driver_l1\mpeg4\inc" 							"%1\driver_l1\mpeg4\inc" /s /i /y
xcopy "..\..\driver_l1\msc\inc" 								"%1\driver_l1\msc\inc" /s /i /y
xcopy "..\..\driver_l1\mscpro\inc" 							"%1\driver_l1\mscpro\inc" /s /i /y
xcopy "..\..\driver_l1\nand" 										"%1\driver_l1\nand" /s /i /y
xcopy "..\..\driver_l1\ppu\inc" 								"%1\driver_l1\ppu\inc" /s /i /y
xcopy "..\..\driver_l1\ppu\src" 								"%1\driver_l1\ppu\src" /s /i /y
xcopy "..\..\driver_l1\scaler\inc" 							"%1\driver_l1\scaler\inc" /s /i /y
xcopy "..\..\driver_l1\sdc\inc" 								"%1\driver_l1\sdc\inc" /s /i /y
xcopy "..\..\driver_l1\spu\inc" 								"%1\driver_l1\spu\inc" /s /i /y
xcopy "..\..\driver_l1\usbD\inc" 								"%1\driver_l1\usbD\inc" /s /i /y
xcopy "..\..\driver_l1\usbD_20\inc" 						"%1\driver_l1\usbD_20\inc" /s /i /y
xcopy "..\..\driver_l1\usbD_20\user" 						"%1\driver_l1\usbD_20\user" /s /i /y
xcopy "..\..\driver_l1\usbD_20_Cam\inc" 				"%1\driver_l1\usbD_20_Cam\inc" /s /i /y
xcopy "..\..\driver_l1\usbD_Cam\inc" 						"%1\driver_l1\usbD_Cam\inc" /s /i /y
xcopy "..\..\driver_l1\usbH\inc" 								"%1\driver_l1\usbH\inc" /s /i /y
xcopy "..\..\driver_l1\usbH_20\inc" 						"%1\driver_l1\usbH_20\inc" /s /i /y
xcopy "..\..\driver_l1\usbH_20_UVC\inc" 				"%1\driver_l1\usbH_20_UVC\inc" /s /i /y

echo Copy driver_l2
xcopy "..\..\driver_l2\ad_key" 									"%1\driver_l2\ad_key" /s /i /y
xcopy "..\..\driver_l2\ad_keyscan" 							"%1\driver_l2\ad_keyscan" /s /i /y
xcopy "..\..\driver_l2\inc" 										"%1\driver_l2\inc" /s /i /y
xcopy "..\..\driver_l2\ir" 											"%1\driver_l2\ir" /s /i /y
xcopy "..\..\driver_l2\itouch" 									"%1\driver_l2\itouch" /s /i /y
xcopy "..\..\driver_l2\keyscan" 								"%1\driver_l2\keyscan" /s /i /y
xcopy "..\..\driver_l2\nand_flash" 							"%1\driver_l2\nand_flash" /s /i /y
xcopy "..\..\driver_l2\sd\inc" 									"%1\driver_l2\sd\inc" /s /i /y
xcopy "..\..\driver_l2\spi_flash" 							"%1\driver_l2\spi_flash" /s /i /y
xcopy "..\..\driver_l2\src" 										"%1\driver_l2\src" /s /i /y
xcopy "..\..\driver_l2\system" 									"%1\driver_l2\system" /s /i /y
xcopy "..\..\driver_l2\usb_host\inc" 						"%1\driver_l2\usb_host\inc" /s /i /y
xcopy "..\..\driver_l2\usb_host_20\inc" 				"%1\driver_l2\usb_host_20\inc" /s /i /y
xcopy "..\..\driver_l2\usb_host_20_UVC\inc" 		"%1\driver_l2\usb_host_20_UVC\inc" /s /i /y

echo Copy gplib
xcopy "..\..\gplib\bmp\inc" 										"%1\gplib\bmp\inc" /s /i /y
xcopy "..\..\gplib\calendar" 										"%1\gplib\calendar" /s /i /y

xcopy "..\..\gplib\fs\driver" 									"%1\gplib\fs\driver" /s /i /y
xcopy "..\..\gplib\fs\include" 									"%1\gplib\fs\include" /s /i /y
xcopy "..\..\gplib\fs\nls" 											"%1\gplib\fs\nls" /s /i /y
copy "..\..\gplib\fs\fs.h" 											"%1\gplib\fs\fs.h" 
copy "..\..\gplib\fs\fs_get_version.c" 					"%1\gplib\fs\fs_get_version.c" 
copy "..\..\gplib\fs\fs_os.c" 									"%1\gplib\fs\fs_os.c" 
copy "..\..\gplib\fs\fsglobalvar.c" 						"%1\gplib\fs\fsglobalvar.c" 
copy "..\..\gplib\fs\swap_byte.c" 							"%1\gplib\fs\swap_byte.c" 
copy "..\..\gplib\fs\vfs.h" 										"%1\gplib\fs\vfs.h" 

xcopy "..\..\gplib\gif\inc" 										"%1\gplib\gif\inc" /s /i /y
xcopy "..\..\gplib\gpfs_bg_unlink\inc" 					"%1\gplib\gpfs_bg_unlink\inc" /s /i /y
xcopy "..\..\gplib\gpid_api\inc" 								"%1\gplib\gpid_api\inc" /s /i /y
xcopy "..\..\gplib\gpzp\inc" 										"%1\gplib\gpzp\inc" /s /i /y
xcopy "..\..\gplib\graphics" 										"%1\gplib\graphics" /s /i /y
xcopy "..\..\gplib\id3\inc" 										"%1\gplib\id3\inc" /s /i /y
xcopy "..\..\gplib\inc" 												"%1\gplib\inc" /s /i /y
xcopy "..\..\gplib\jpeg\inc" 										"%1\gplib\jpeg\inc" /s /i /y
xcopy "..\..\gplib\liba" 												"%1\gplib\liba" /s /i /y
xcopy "..\..\gplib\mm" 													"%1\gplib\mm" /s /i /y
xcopy "..\..\gplib\ppu\inc" 										"%1\gplib\ppu\inc" /s /i /y
xcopy "..\..\gplib\print_string" 								"%1\gplib\print_string" /s /i /y
xcopy "..\..\gplib\pwkey_process" 							"%1\gplib\pwkey_process" /s /i /y
xcopy "..\..\gplib\spu\inc" 										"%1\gplib\spu\inc" /s /i /y
xcopy "..\..\gplib\src" 												"%1\gplib\src" /s /i /y
xcopy "..\..\gplib\storages" 										"%1\gplib\storages" /s /i /y
xcopy "..\..\gplib\FaceDetect" 										"%1\gplib\FaceDetect" /s /i /y
xcopy "..\..\gplib\comair_tx\inc" 										"%1\gplib\comair_tx\inc" /s /i /y

echo Copy gpstdlib
xcopy "..\..\gpstdlib" 													"%1\gpstdlib" /s /i /y

echo Copy liba
xcopy "..\..\liba" 															"%1\liba" /s /i /y

echo Copy os
xcopy "..\..\os\inc" 														"%1\os\inc" /s /i /y

echo Copy project
xcopy "..\..\project" 													"%1\project" /s /i /y
del "%1\project\gpl32_platform_demo\gpl32_platform_demo.mcp"
del "%1\project\gpl326xx_platform_demo\gpl326xx_platform_demo.mcp"
del "%1\project\gpl327xx_platform_demo\gpl327xx_platform_demo.mcp"
del "%1\project\gpl326xxb_platform_demo\gpl326xxb_platform_demo.mcp"

echo Copy tools
xcopy "..\..\tools"															"%1\tools" /s /i /y

goto OK
:ERROR
echo Using %0 [dest_dir] to copy to destination dir
:OK