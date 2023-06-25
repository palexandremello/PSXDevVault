cmake_minimum_required(VERSION 3.21)

project(
    Psn00SDK-template
    LANGUAGES C CXX ASM
    VERSION 1.0.0
    DESCRIPTION "Collisions example"
 	HOMEPAGE_URL "https://github.com/palexandremello/PSXDevVault"
   
)

psn00bsdk_add_executable(collision GPREL main.c initialize.h initialize.c)

#psn00bsdk_target_incbin(tile PRIVATE tim_igu_tile ball16c.tim)
