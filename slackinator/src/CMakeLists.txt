# suppose you have created a ui file called configwin.ui
cmake_minimum_required (VERSION 2.6)

project(slackinator)
message("BUILD_SHARED_LIBS" ${BUILD_SHARED_LIBS})

set (CMAKE_PREFIX_PATH /opt/Qt/5.7)
set(Qt5_DIR /opt/Qt/5.7/gcc_64/lib/cmake/Qt5)
#set(Qt5Widgets_DIR /opt/Qt/5.7/gcc_64/lib/cmake/Qt5Widgets)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# Include header files
include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# give a name to all cpp files
file(GLOB SOURCES *.cpp)

# give a name to all h files
file(GLOB MOC_HEADERS *.h)

QT5_WRAP_CPP(tst_hdr_moc ${MOC_HEADERS})

# Create shared library
add_library(${PROJECT_NAME} ${tst_hdr_moc} ${SOURCES})
qt5_use_modules(${PROJECT_NAME} Widgets Core)

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

# Install library headers
file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
install(FILES ${HEADERS} DESTINATION include)
