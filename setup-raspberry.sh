#!/bin/bash

#sudo apt install libbluetooth-dev
#sudo apt install bluetooth

# Install required tools
sudo apt install              \
  g++     figlet              \
  gcc     openssl             \
  tar     texinfo             \
  vim     apt-file            \
  fish    autoconf            \
  flex    automake            \
  gawk    gfortran            \
  pigz    symlinks            \
  ruby    pkg-config          \
  bison   libncurses-dev      \
  cmake   build-essential     \
  gperf   wayland-scanner++   \
  unzip   extra-cmake-modules

# install required libraries
sudo apt install                                                         \
  libpq-dev      libvpx-dev       libinput-dev      libavcodec-dev       \
  libts-dev      freetds-dev      libmtdev-dev      libsqlite3-dev       \
  libbz2-dev     libjpeg-dev      libpulse-dev      libswscale-dev       \
  libcap-dev     libnss3-dev      libsrtp2-dev      libatspi2.0-dev      \
  libdrm-dev     libudev-dev      libxslt1-dev      libavformat-dev      \
  libgbm-dev     libxslt-dev      libsnappy-dev     libatkmm-1.6-dev     \
  libicu-dev     libaudio-dev     libdbus-1-dev     libboost-all-dev     \
  libpci-dev     libcups2-dev     libiodbc2-dev     libfreetype6-dev     \
  libssl-dev     firebird-dev     libasound2-dev    libfontconfig1-dev

# Install OpenGL libraries
sudo apt install                     \
  mesa-common-dev  libegl1-mesa-dev  \
  libglib2.0-dev   libgles2-mesa-dev

# Install GStreamer libraries
sudo apt install gstreamer1.0-alsa libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev

# Install X11 libraries
sudo apt install       \
  libxi6               \
  libxi-dev            \
  libx11-dev           \
  libx11-xcb1          \
  libx11-xcb-dev       \
  libxss-dev           \
  libxext-dev          \
  libxtst-dev          \
  libxrandr-dev        \
  libxcursor-dev       \
  libxdamage-dev       \
  libxrender-dev       \
  libxcomposite1       \
  libxcomposite-dev    \
  libxkbcommon-dev     \
  libxkbcommon-x11-0   \
  libxkbcommon-x11-dev

# Install XCB libraries
sudo apt install                                                       \
  libxcb1                libxcb-image0         libxcb-keysyms1         \
  libxcb1-dev            libxcb-image0-dev     libxcb-keysyms1-dev     \
  libxcb-glx0-dev        libxcb-icccm4         libxcb-xinerama0        \
  libxcb-shm0            libxcb-icccm4-dev     libxcb-xinerama0-dev    \
  libxcb-shm0-dev        libxcb-randr0-dev     libxcb-render-util0     \
  libxcb-sync1           libxcb-shape0-dev     libxcb-render-util0-dev \
  libxcb-sync-dev        libxcb-xfixes0-dev    "^libxcb.*"

echo "#!/bin/bash

# ~/.profile: executed by the command interpreter for login shells.
# This file is not read by bash(1), if ~/.bash_profile or ~/.bash_login
# exists.
# see /usr/share/doc/bash/examples/startup-files for examples.
# the files are located in the bash-doc package.

# the default umask is set in /etc/profile; for setting the umask
# for ssh logins, install and configure the libpam-umask package.
#umask 022

if [[ -n \"\$(printenv TERM)\" ]]; then
   exec -l /usr/bin/fish -il
fi

# if running bash
if [ -n \"\$BASH_VERSION\" ]; then
   # include .bashrc if it exists
   if [ -f \"\$HOME/.bashrc\" ]; then
       . \"\$HOME/.bashrc\"
   fi
fi

# set PATH so it includes user\'s private bin if it exists
if [ -d \"\$HOME/bin\" ] ; then
   PATH=\"\$HOME/bin:\$PATH\"
fi

# set PATH so it includes user's private bin if it exists
if [ -d \"\$HOME/.local/bin\" ] ; then
   PATH=\"\$HOME/.local/bin:\$PATH\"
fi
" > ~/.profile
