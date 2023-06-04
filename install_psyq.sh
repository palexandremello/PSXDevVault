#!/bin/bash

gpg --recv-key 13FCEF89DD9E3C4F && gpg --recv-key 3AB00996FC26A641

git clone https://aur.archlinux.org/cross-mipsel-linux-gnu-binutils.git && \
cd  cross-mipsel-linux-gnu-binutils && \
makepkg -si

git clone https://aur.archlinux.org/cross-mipsel-linux-gnu-gcc.git  && \
cd cross-mipsel-linux-gnu-gcc && \
makepkg -si

