# -*-cmake-*-
#
# Try to find the libzoltan graph partioning library. This module
# first tries to use a system-supplied Zoltan cmake module, and does
# it the tradiotional way if this fails.
#
# This module reads hints about search locations from variables::
#
#  Zoltan_DIR          - path where Trilinos/ZOLTAN was installed to
#
# Once done, this will define:
#
#  Zoltan_FOUND        - system has the libzoltan graph partioning library
#  HAVE_ZOLTAN         - like ZOLTAN_FOUND, but for the inclusion in config.h
#  Zoltan_INCLUDE_DIR  - include paths to use libzoltan
#  Zoltan_LIBRARIES    - Link these to use libzoltan

# don't be picky about the Zoltan_DIR variable
if (ZOLTAN_DIR AND NOT Zoltan_DIR)
  set(Zoltan_DIR "${ZOLTAN_DIR}")
elseif(Zoltan_ROOT AND NOT Zoltan_DIR)
  set(Zoltan_DIR "${Zoltan_ROOT}")
elseif(ZOLTAN_ROOT AND NOT Zoltan_DIR)
  set(Zoltan_DIR "${ZOLTAN_ROOT}")
endif()

# try to find the zoltan libraries using the hip way...
find_package(Zoltan QUIET)

# ... unfortunately the hip way likes fail for many common linux
# distributions, so we check if it worked and if it didn't we try to
# go the manual route.
if(Zoltan_FOUND)
  find_path("ZOLTAN_INCLUDE_WORKS" PATHS "zoltan.h" "${Zoltan_INCLUDE_DIRS}" NO_DEFAULT_PATH)
  find_library("ZOLTAN_LIBRARY_WORKS" "zoltan" PATHS "${Zoltan_LIBRARY_DIRS}" NO_DEFAULT_PATH)

  if (NOT ZOLTAN_INCLUDE_WORKS OR NOT ZOLTAN_LIBRARY_WORKS)
    message(WARNING "It seems like the system-provided cmake module for ZOLTAN does not work properly. Trying to detect ZOLTAN manually.")
    set(Zoltan_FOUND 0)
    unset(Zoltan_INCLUDE_DIRS)
    unset(Zoltan_LIBRARY_DIRS)

    if (Zoltan_DIR_TMP)
      set(Zoltan_DIR ${Zoltan_DIR_TMP})
    else ()
      unset(Zoltan_DIR)
    endif()
  endif()
endif()

if(Zoltan_FOUND)
    set(HAVE_ZOLTAN 1)
else()
  set(Zoltan_SEARCH_PATH "/usr" "/usr/local" "/opt" "/opt/local")
  set(Zoltan_NO_DEFAULT_PATH "")
  if(Zoltan_DIR)
    list(INSERT Zoltan_SEARCH_PATH 0 "${Zoltan_DIR}/../../..")
    list(INSERT Zoltan_SEARCH_PATH 0 "${Zoltan_DIR}/../..")
    list(INSERT Zoltan_SEARCH_PATH 0 "${Zoltan_DIR}/..")
    list(INSERT Zoltan_SEARCH_PATH 0 "${Zoltan_DIR}")
    set(Zoltan_NO_DEFAULT_PATH "NO_DEFAULT_PATH")
  endif()

  # Make sure we have checked for the underlying partitioners.
  find_package(PTScotch QUIET)
  find_package(ParMETIS QUIET)

  # search for files which implements this module
  find_path (Zoltan_INCLUDE_DIRS
    NAMES "zoltan.h"
    PATHS ${Zoltan_SEARCH_PATH}
    PATH_SUFFIXES include trilinos include/trilinos trilinos/include
    ${Zoltan_NO_DEFAULT_PATH})

  # only search in architecture-relevant directory
  if (CMAKE_SIZEOF_VOID_P)
    math (EXPR _BITS "8 * ${CMAKE_SIZEOF_VOID_P}")
  endif (CMAKE_SIZEOF_VOID_P)

  find_library(Zoltan_LIBRARIES
    NAMES zoltan trilinos_zoltan
    PATHS ${Zoltan_SEARCH_PATH}
    PATH_SUFFIXES "lib/.libs" "lib" "lib${_BITS}" "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
    ${Zoltan_NO_DEFAULT_PATH})

  set (Zoltan_FOUND FALSE)
  if (Zoltan_INCLUDE_DIRS AND Zoltan_LIBRARIES)
    set(Zoltan_FOUND TRUE)
    set(HAVE_ZOLTAN 1)
    set(Zoltan_LIBRARIES ${Zoltan_LIBRARIES} ${PARMETIS_LIBRARIES} ${PTSCOTCH_LIBRARIES})
    set(Zoltan_INCLUDE_DIRS ${Zoltan_INCLUDE_DIRS} ${PARMETIS_INCLUDE_DIRS} ${PTSCOTCH_INCLUDE_DIRS})
  else()
    unset(Zoltan_FOUND)
    unset(HAVE_ZOLTAN)
    unset(Zoltan_LIBRARIES)
    unset(Zoltan_INCLUDE_DIRS)
  endif()
endif()

# print a message to indicate status of this package
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(Zoltan
  DEFAULT_MSG
  Zoltan_LIBRARIES
  Zoltan_INCLUDE_DIRS)

if (Zoltan_FOUND)
  # prevent dune from producing a confusing not found message. this
  # solution is still not optimal because the package appears as
  # "ZoltanEwoms" but it's better than nothing...
  set(ZoltanEwoms_FOUND 1)
endif()
