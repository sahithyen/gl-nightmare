#!/bin/bash

. /opt/fslc-x11-eaton/4.0/environment-setup-cortexa9t2hf-neon-fslc-linux-gnueabi

if [ ! -d build-sdk ]
then
  meson setup build-sdk .
fi

cd build-sdk
ninja

scp nightmare-gles1 root@192.168.119.148:~
scp nightmare-gles2 root@192.168.119.148:~
