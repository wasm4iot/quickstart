import sys
import os
from os.path import basename
Import("env")

platform = env.PioPlatform()

def dfu_upload(source, target, env):
    firmware_path = str(source[0])
    firmware_name = basename(firmware_path)

    print("Upload to nrf52840 dongle.")
    print("firmware_path = ",firmware_path)
    print("firmware_name = ",firmware_name)

    genpkg = "".join(["nrfutil pkg generate --hw-version 52 --sd-req=0x00 --application ", firmware_path, " --application-version 1 firmware.zip"])
    dfupkg = "nrfutil dfu serial -pkg firmware.zip -p /dev/ttyACM0 -b 115200"
    print( genpkg )
    os.system( genpkg )
    os.system( dfupkg )

    print("Uploading done.")


# Custom upload command and program name
env.Replace(PROGNAME="firmware", UPLOADCMD=dfu_upload)