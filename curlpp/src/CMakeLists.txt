# Find source files
file(GLOB SOURCES *.cpp)

# Include header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Create shared library
add_library(${PROJECT_NAME} ${SOURCES})

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

# Install library headers
file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
install(FILES ${HEADERS} DESTINATION include)
