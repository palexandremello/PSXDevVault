
cmake_minimum_required(VERSION 3.21)

project(
	PSn00bSDK-template
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  "TILE Example"
	HOMEPAGE_URL "https://github.com/palexandremello/PSXDevVault"
)

psn00bsdk_add_executable(tile GPREL main.c)

psn00bsdk_target_incbin(tile PRIVATE tim_igu_tile ball16c.tim)
#psn00bsdk_add_cd_image(
#	iso      # Target name
#	tile # Output file name (= template.bin + template.cue)
#	iso.xml  # Path to config file
#	DEPENDS tile system.cnf
#)
