QEMUFLAGS =	-no-reboot 														\
			-no-shutdown 													\
			-boot d 														\
			-M smm=off 														\
			-serial stdio 													\
			-machine q35 													\
			-cpu qemu64 													\
			-smp 8 															\
			-cdrom ./Bin/kot.iso											\
			-drive file=./Bin/kot.img										\
			-m 3G															\
			-rtc base=localtime												\
			-usb                                            				\
    		-device usb-ehci,id=ehci                        				\
			-device ich9-intel-hda -device hda-output


build:
	bash ./Build/build.sh 

# debug with your own qemu build : ../qemu/bin/debug/native/x86_64-softmmu/
run:
	qemu-system-x86_64 $(QEMUFLAGS)

debug:
	qemu-system-x86_64 $(QEMUFLAGS) -s -S

deps-llvm:
	wget https://apt.llvm.org/llvm.sh
	chmod +x llvm.sh
	sudo ./llvm.sh 14 all
	rm -f llvm.sh

deps-debian: deps-llvm
	sudo apt update
	sudo apt install kpartx nasm xorriso mtools grub-common grub-efi-amd64 grub-pc-bin build-essential qemu-system-x86 ovmf  -y

clean:
	sudo rm -rf ./Bin ./Sysroot ./Sources/*/*/*/*/*/Lib ./Sources/*/*/*/*/Lib ./Sources/*/*/*/Lib ./Sources/*/*/Lib ./Sources/*/Lib

deps-github-action: deps-llvm
	sudo apt update
	sudo apt install kpartx nasm xorriso mtools

github-action: deps-github-action build

.PHONY: build run deps-llvm deps-debian
