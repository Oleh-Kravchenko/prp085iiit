CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(rasterto_prp085iiit)

ADD_EXECUTABLE(rasterto_prp085iiit
source/rasterto_prp085iiit.c
source/prp085iiit.ppd
)

TARGET_LINK_LIBRARIES(rasterto_prp085iiit cupsimage)

IF(NOT CUPS_FILTER_DIR)
	FIND_PATH(CUPS_FILTER_DIR pstoraster
	PATHS /usr/libexec/cups/filter/
	PATHS /usr/lib/cups/filter/
	PATHS /usr/local/lib/cups/filter/
	)
ENDIF(NOT CUPS_FILTER_DIR)

IF(NOT CUPS_PPD_DIR)
	FIND_PATH(CUPS_PPD_DIR zebra.ppd
	PATHS /usr/share/cups/model/
	PATHS /usr/local/share/cups/model/
	)
ENDIF(NOT CUPS_PPD_DIR)

INSTALL(TARGETS rasterto_prp085iiit RUNTIME DESTINATION ${CUPS_FILTER_DIR})

INSTALL(FILES source/prp085iiit.ppd DESTINATION ${CUPS_PPD_DIR})

CONFIGURE_FILE(
"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
IMMEDIATE @ONLY
)

ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}"
-P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
)
