
# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(OS_MACOSX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(OS_LINUX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
endif()

if(NOT DEFINED PROJECT_ARCH)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    set(PROJECT_ARCH "x86")
  endif()
  if(OS_MACOSX)
    # PROJECT_ARCH should be specified on Mac OS X.
    message(WARNING "No PROJECT_ARCH value specified, using ${PROJECT_ARCH}")
  endif()
endif()

if(${CMAKE_GENERATOR} STREQUAL "Ninja")
  set(GEN_NINJA 1)
elseif(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  set(GEN_MAKEFILES 1)
endif()

# Determine the build type.
if(NOT CMAKE_BUILD_TYPE AND (GEN_NINJA OR GEN_MAKEFILES))
  # CMAKE_BUILD_TYPE should be specified when using Ninja or Unix Makefiles.
  set(CMAKE_BUILD_TYPE Release)
  message(WARNING "No CMAKE_BUILD_TYPE value selected, using ${CMAKE_BUILD_TYPE}")
endif()

#
# Linux configuration.
#

if(OS_LINUX)
  # Platform-specific compiler/linker flags.
  list(APPEND EUSTIA_COMPILER_FLAGS
    -fno-strict-aliasing            # Avoid assumptions regarding non-aliasing of objects of different types
    -fPIC                           # Generate position-independent code for shared libraries
    -fstack-protector               # Protect some vulnerable functions from stack-smashing (security feature)
    -pipe                           # Use pipes rather than temporary files for communication between build stages
    -Wall                           # Enable all warnings
    -Werror                         # Treat warnings as errors
    )
  list(APPEND EUSTIA_C_COMPILER_FLAGS
    -std=c99                        # Use the C99 language standard
    )
  list(APPEND EUSTIA_CXX_COMPILER_FLAGS
    -fno-rtti                       # Disable real-time type information
    -std=c++14                      # Use the C++11 language standard including GNU extensions
    -Wsign-compare                  # Warn about mixed signed/unsigned type comparisons
    )
  list(APPEND EUSTIA_COMPILER_FLAGS_DEBUG
    -O0                             # Disable optimizations
    -g                              # Generate debug information
    )
  list(APPEND EUSTIA_COMPILER_FLAGS_RELEASE
    -O2                             # Optimize for maximum speed
    -fdata-sections                 # Enable linker optimizations to improve locality of reference for data sections
    -ffunction-sections             # Enable linker optimizations to improve locality of reference for function sections
    -fno-ident                      # Ignore the #ident directive
    )
  list(APPEND EUSTIA_LINKER_FLAGS
    -fPIC                           # Generate position-independent code for shared libraries
    -Wl,--fatal-warnings            # Treat warnings as errors
    -Wl,-rpath,.                    # Set rpath so that libraries can be placed next to the executable
    -Wl,-z,noexecstack              # Mark the stack as non-executable (security feature)
    -Wl,-z,relro                    # Mark relocation sections as read-only (security feature)
    )
  list(APPEND EUSTIA_LINKER_FLAGS_RELEASE
    -Wl,-O1                         # Enable linker optimizations
    -Wl,--as-needed                 # Only link libraries that export symbols used by the binary
    -Wl,--gc-sections               # Remove unused code resulting from -fdata-sections and -function-sections
    )
  list(APPEND EUSTIA_COMPILER_DEFINES
    _FILE_OFFSET_BITS=64            # Allow the Large File Support (LFS) interface to replace the old interface
    )
  list(APPEND EUSTIA_COMPILER_DEFINES_RELEASE
    NDEBUG                          # Not a debug build
    )

  include(CheckCCompilerFlag)
  include(CheckCXXCompilerFlag)

  if(PROJECT_ARCH STREQUAL "x86_64")
    # 64-bit architecture.
    list(APPEND EUSTIA_COMPILER_FLAGS
      -m64
      -march=x86-64
      )
    list(APPEND EUSTIA_LINKER_FLAGS
      -m64
      )
  elseif(PROJECT_ARCH STREQUAL "x86")
    # 32-bit architecture.
    list(APPEND EUSTIA_COMPILER_FLAGS
      -msse2
      -mfpmath=sse
      -mmmx
      -m32
      )
    list(APPEND EUSTIA_LINKER_FLAGS
      -m32
      )
  endif()

endif()


#
# Windows configuration.
#

if(OS_WINDOWS)
  if (GEN_NINJA)
    # When using the Ninja generator clear the CMake defaults to avoid excessive
    # console warnings (see issue #2120).
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
  endif()

  # Consumers who run into LNK4099 warnings can pass /Z7 instead (see issue #385).
  set(EUSTIA_DEBUG_INFO_FLAG "/Zi" CACHE STRING "Optional flag specifying specific /Z flag to use")

  # Consumers using different runtime types may want to pass different flags
  set(EUSTIA_RUNTIME_LIBRARY_FLAG "/MT" CACHE STRING "Optional flag specifying which runtime to use")
  if (EUSTIA_RUNTIME_LIBRARY_FLAG)
    list(APPEND EUSTIA_COMPILER_FLAGS_DEBUG ${EUSTIA_RUNTIME_LIBRARY_FLAG}d)
    list(APPEND EUSTIA_COMPILER_FLAGS_RELEASE ${EUSTIA_RUNTIME_LIBRARY_FLAG})
  endif()

  # Platform-specific compiler/linker flags.
  list(APPEND EUSTIA_COMPILER_FLAGS
    /MP           # Multiprocess compilation
    /Gy           # Enable function-level linking
    /GR-          # Disable run-time type information
    /W4           # Warning level 4
    /WX           # Treat warnings as errors
    ${EUSTIA_DEBUG_INFO_FLAG}
    )
  list(APPEND EUSTIA_COMPILER_FLAGS_DEBUG
    /Od           # Disable optimizations
    /RTC1         # Enable basic run-time checks
    )
  list(APPEND EUSTIA_COMPILER_FLAGS_RELEASE
    /O2           # Optimize for maximum speed
    /Ob2          # Inline any suitable function
    /GF           # Enable string pooling
    )
  list(APPEND EUSTIA_LINKER_FLAGS_DEBUG
    /DEBUG        # Generate debug information
    )
  list(APPEND EUSTIA_EXE_LINKER_FLAGS
    /MANIFEST:NO        # No default manifest (see ADD_WINDOWS_MANIFEST macro usage)
    /LARGEADDRESSAWARE  # Allow 32-bit processes to access 3GB of RAM
    )
  list(APPEND EUSTIA_COMPILER_DEFINES
    WIN32 _WIN32 _WINDOWS             # Windows platform
    UNICODE _UNICODE                  # Unicode build
    WINVER=0x0601 _WIN32_WINNT=0x601  # Targeting Windows 7
    WIN32_LEAN_AND_MEAN               # Exclude less common API declarations
    )
  list(APPEND EUSTIA_COMPILER_DEFINES_RELEASE
    NDEBUG _NDEBUG                    # Not a debug build
    )

endif()
