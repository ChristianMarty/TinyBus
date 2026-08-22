& make all -B -C "attiny816/Debug"
& make all -B -C "attiny841/Debug"
& make all -B -C "avr16eb32/Debug"
& make all -B -C "attiny841_4-channelDimmer/Debug"
& make all -B -C "attiny841_par-16/Debug"
& make all -B -C "attiny841_ringLightDriver/Debug"
& make all -B -C "attiny841_teelicht/Debug"
& make all -B -C "avr16eb32_notAus/Debug"

& make all -B -C "updater_attiny841/Debug"
& make all -B -C "updater_avr16eb32/Debug"

$hexMerger = 'C:/Users/Christian/Projekte/HEX-Merger/build/distribution/HEX-Merger.exe'

start-process -FilePath $hexMerger -ArgumentList '-N -C "attiny841_4-channelDimmer/kernelUpdater.hexmerger"' -wait
start-process -FilePath $hexMerger -ArgumentList '-N -C "attiny841_par-16/kernelUpdater.hexmerger"' -wait
start-process -FilePath $hexMerger -ArgumentList '-N -C "attiny841_ringLightDriver/kernelUpdater.hexmerger"' -wait
start-process -FilePath $hexMerger -ArgumentList '-N -C "attiny841_teelicht/kernelUpdater.hexmerger"' -wait
start-process -FilePath $hexMerger -ArgumentList '-N -C "avr16eb32_notAus/kernelUpdater.hexmerger"' -wait

Copy-Item -Path "attiny841_4-channelDimmer/Debug/tinyloader.hex" -Destination "_release/4-channelDimmer_kernel.hex" -Force
Copy-Item -Path "attiny841_4-channelDimmer/Debug/4-channel-dimmer_kernelUpdate.hex" -Destination "_release/4-channelDimmer_kernelUpdate.hex" -Force

Copy-Item -Path "attiny841_par-16/Debug/tinyloader.hex" -Destination "_release/par-16_kernel.hex" -Force
Copy-Item -Path "attiny841_par-16/Debug/par-16_kernelUpdate.hex" -Destination "_release/par-16_kernelUpdate.hex" -Force

Copy-Item -Path "attiny841_ringLightDriver/Debug/tinyloader.hex" -Destination "_release/ringLightDriver_kernel.hex" -Force
Copy-Item -Path "attiny841_ringLightDriver/Debug/ringLightDriver_kernelUpdate.hex" -Destination "_release/ringLightDriver_kernelUpdate.hex" -Force

Copy-Item -Path "attiny841_teelicht/Debug/tinyloader.hex" -Destination "_release/teelicht_kernel.hex" -Force
Copy-Item -Path "attiny841_teelicht/Debug/teelicht_kernelUpdate.hex" -Destination "_release/teelicht_kernelUpdate.hex" -Force

Copy-Item -Path "avr16eb32_notAus/Debug/tinyloader.hex" -Destination "_release/notAus_kernel.hex" -Force
Copy-Item -Path "avr16eb32_notAus/Debug/notAus_kernelUpdate.hex" -Destination "_release/notAus_kernelUpdate.hex" -Force