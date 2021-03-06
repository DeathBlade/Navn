# Find all the *.cpp files within the current source directory, and sort the list
file(GLOB SRC_SRCS_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(SRC_SRCS ${SRC_SRCS_C} ${SRC_SRCS_CPP})

# If using Windows, add the windows specific sources
if(WIN32)
  file(GLOB WINDOWS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "windows/*.cpp")
  append_to_list(SRC_SRCS ${WINDOWS_SRCS})
endif(WIN32)

sort_list(SRC_SRCS)

# Set all the files to use C++ as well as set their compile flags (use the Module-specific compile flags, though)
set_source_files_properties(${SRC_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

# Create an empty list to store extra include directories
set(EXTRA_INCLUDES)
# Iterate through all the source files
foreach(SRC ${SRC_SRCS})
  # Temporary variable for the current source's include directories
  set(TEMP_INCLUDES)
  # Create unused skip variable
  set(SKIP)
  # Calculate the header file dependencies for the given source file
  calculate_depends(${SRC} SKIP TEMP_INCLUDES)
  # If there were some extra include directories, add them to the list
  if(TEMP_INCLUDES)
    append_to_list(EXTRA_INCLUDES ${TEMP_INCLUDES})
  endif(TEMP_INCLUDES)
endforeach(SRC)
# If there were extra include directories, remove the duplicates and add the directories to the include path
if(EXTRA_INCLUDES)
  remove_list_duplicates(EXTRA_INCLUDES)
  include_directories(${EXTRA_INCLUDES})
endif(EXTRA_INCLUDES)

# # Under Windows, we also include a resource file to the build
# if(WIN32)
#   # Make sure that the resource file is seen as an RC file to be compiled with a resource compiler, not a C++ compiler
#   set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc LANGUAGE RC)
#   # Add the resource file to the list of sources
#   append_to_list(SRC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc)
#   # For MinGW, we have to change the compile flags
#   if(MINGW)
#     set(RC_CFLAGS "-DMINGW -Ocoff -I${navn_SOURCE_DIR}/include")
#     # If any sort of debugging is being enabled, add a _DEBUG define to the flags for the resource compiler
#     if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
#       set(RC_CFLAGS "${RC_CFLAGS} -D_DEBUG")
#     endif(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
#     set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc COMPILE_FLAGS "${RC_CFLAGS}")
#   # For anything else, assumingly Visual Studio at this point, use a different set of compile flags
#   else(MINGW)
#     set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc COMPILE_FLAGS "/i\"${navn_SOURCE_DIR}/include\"")
#   endif(MINGW)
# endif(WIN32)

# If compiling with Visual Studio, create a static library out of win32/win32_memory.cpp to be included with everything else, needed to override its override of new/delete operators
if(MSVC)
  set_source_files_properties(windows/win32_memory.cpp PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  add_library(win32_memory STATIC windows/win32_memory.cpp)
  set(WIN32_MEMORY win32_memory)
else(MSVC)
  set(WIN32_MEMORY)
endif(MSVC)

# Generate the Navn executable and set it's linker flags, also set it to export it's symbols even though it's not a Module
add_executable(${PROGRAM_NAME} ${SRC_SRCS})
set_target_properties(${PROGRAM_NAME} PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}" ENABLE_EXPORTS ON)
# On Windows, also link Navn to the wsock32 and Ws2_32 library, as well as set the version
if(WIN32)
  target_link_libraries(${PROGRAM_NAME} wsock32 Ws2_32)
  set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
else(WIN32)
  target_link_libraries(${PROGRAM_NAME} ${LINK_LIBS})
endif(WIN32)

# Get the filename of the Navn executable as it is in on this system
get_target_property(BOT_BINARY ${PROGRAM_NAME} LOCATION)
get_filename_component(BOT_BINARY ${BOT_BINARY} NAME)

# Generate conf.h from the earlier configuration
configure_file(${navn_SOURCE_DIR}/include/config.h.cmake ${navn_SOURCE_DIR}/include/config.h)

# Set Navn to be installed to the bin directory
install(TARGETS ${PROGRAM_NAME}
  DESTINATION bin
)