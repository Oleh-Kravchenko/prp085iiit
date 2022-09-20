CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(rasterto_prp085iiit)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

FIND_PACKAGE(CUPS REQUIRED)

ADD_EXECUTABLE(rasterto_prp085iiit
source/rasterto_prp085iiit.c
source/prp085iiit.ppd
)

TARGET_COMPILE_OPTIONS(rasterto_prp085iiit
PRIVATE
	-Wno-error=unused-parameter
	-Wno-error=deprecated-declarations
	-pedantic
	-std=gnu99
	-Wall
	-Wextra
	-Werror
)

TARGET_INCLUDE_DIRECTORIES(rasterto_prp085iiit PRIVATE ${CUPS_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(rasterto_prp085iiit PRIVATE ${CUPS_IMAGE_LIBS})

INSTALL(TARGETS rasterto_prp085iiit DESTINATION "${CUPS_SERVERBIN}/filter")
INSTALL(FILES source/prp085iiit.ppd DESTINATION "${CUPS_DATADIR}/model")
