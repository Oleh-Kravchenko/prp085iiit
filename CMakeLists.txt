CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(rasterto_prp085iiit)

ADD_EXECUTABLE(rasterto_prp085iiit source/rasterto_prp085iiit.c)

TARGET_LINK_LIBRARIES(rasterto_prp085iiit cupsimage)
