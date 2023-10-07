cmake_minimum_required(VERSION 3.21)

project(
        spustream
        LANGUAGES    C ASM
        VERSION      1.0.0
        DESCRIPTION  "PSn00bSDK SPU audio streaming example"
        HOMEPAGE_URL "http://lameguy64.net/?page=psn00bsdk"
)
add_subdirectory(src/controller)
add_subdirectory(src/collision)
add_subdirectory(src/initialize)

file(GLOB _sources src/*.c)
psn00bsdk_add_executable(collision GPREL ${_sources})
#psn00bsdk_add_cd_image(spustream_iso spustream iso.xml DEPENDS spustream)

psn00bsdk_target_incbin(collision PRIVATE ball_effect 3dfx.vag)
psn00bsdk_target_incbin(collision PRIVATE ball_tile ball16c.tim)

target_link_libraries(collision PUBLIC controller_subsystem collision_subsystem initialize_subsystem)

install(FILES ${PROJECT_BINARY_DIR}/teste.exe TYPE BIN)