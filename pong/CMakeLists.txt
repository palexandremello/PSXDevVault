cmake_minimum_required(VERSION 3.21)

project(
        pong
        LANGUAGES    C ASM
        VERSION      1.0.0
        DESCRIPTION  "Pong Game"
        HOMEPAGE_URL "https://github.com/palexandremello"
)

file(GLOB _sources *.c)
psn00bsdk_add_executable(pong GPREL ${_sources})

psn00bsdk_target_incbin(pong PRIVATE ball_effect paddle_pong.vag)
psn00bsdk_target_incbin(pong PRIVATE ball_tile ball16c.tim)

install(FILES ${PROJECT_BINARY_DIR}/pong.exe TYPE BIN)

psn00bsdk_add_cd_image(pong_iso pong iso.xml DEPENDS pong)
