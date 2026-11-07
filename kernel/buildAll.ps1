& make all -B -C "attiny816/Release"
& make all -B -C "attiny841/Release"
& make all -B -C "avr16eb32/Release"
& make all -B -C "attiny841_4-channelDimmer/Release"
& make all -B -C "attiny841_par-16/Release"
& make all -B -C "attiny841_ringLightDriver/Release"
& make all -B -C "attiny841_teelicht/Release"
& make all -B -C "avr16eb32_notAus/Release"

& make all -B -C "updater_attiny841/Release"
& make all -B -C "updater_avr16eb32/Release"

$hexMerger = 'C:/Users/Christian/Projekte/HEX-Merger/build/distribution/HEX-Merger.exe'

start-process -FilePath $hexMerger -ArgumentList '-N -C "attiny841_4-channelDimmer/kernelUpdater-release.hexmerger"' -wait
start-process -FilePath $hexMerger -ArgumentList '-N -C "attiny841_par-16/kernelUpdater-release.hexmerger"' -wait
start-process -FilePath $hexMerger -ArgumentList '-N -C "attiny841_ringLightDriver/kernelUpdater-release.hexmerger"' -wait
start-process -FilePath $hexMerger -ArgumentList '-N -C "attiny841_teelicht/kernelUpdater-release.hexmerger"' -wait
start-process -FilePath $hexMerger -ArgumentList '-N -C "avr16eb32_notAus/kernelUpdater-release.hexmerger"' -wait

Copy-Item -Path "attiny841_4-channelDimmer/Release/tinyloader.hex" -Destination "_release/4-channelDimmer_kernel.hex" -Force
Copy-Item -Path "attiny841_4-channelDimmer/Release/4-channel-dimmer_kernelUpdate.hex" -Destination "_release/4-channelDimmer_kernelUpdate.hex" -Force

Copy-Item -Path "attiny841_par-16/Release/tinyloader.hex" -Destination "_release/par-16_kernel.hex" -Force
Copy-Item -Path "attiny841_par-16/Release/par-16_kernelUpdate.hex" -Destination "_release/par-16_kernelUpdate.hex" -Force

Copy-Item -Path "attiny841_ringLightDriver/Release/tinyloader.hex" -Destination "_release/ringLightDriver_kernel.hex" -Force
Copy-Item -Path "attiny841_ringLightDriver/Release/ringLightDriver_kernelUpdate.hex" -Destination "_release/ringLightDriver_kernelUpdate.hex" -Force

Copy-Item -Path "attiny841_teelicht/Release/tinyloader.hex" -Destination "_release/teelicht_kernel.hex" -Force
Copy-Item -Path "attiny841_teelicht/Release/teelicht_kernelUpdate.hex" -Destination "_release/teelicht_kernelUpdate.hex" -Force

Copy-Item -Path "avr16eb32_notAus/Release/tinyloader.hex" -Destination "_release/notAus_kernel.hex" -Force
Copy-Item -Path "avr16eb32_notAus/Release/notAus_kernelUpdate.hex" -Destination "_release/notAus_kernelUpdate.hex" -Force