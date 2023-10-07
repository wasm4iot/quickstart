import os
import sys
Import("env")

def callback(*args, **kwargs):
    os.chdir("lib/build")
    os.system("cmake .. -DCMAKE_BUILD_TYPE=Release")
    os.system("make")
    os.system("cp libvmlib.a ../libvm.a")

callback()