# .. cmake_module::
#
# This module's content is executed whenever a Dune module requires or
# suggests ewoms-eclgrids!
#

find_package(Zoltan)
if(Zoltan_FOUND)
  dune_register_package_flags(
    LIBRARIES "${Zoltan_LIBRARIES}"
    INCLUDE_DIRS "${Zoltan_INCLUDE_DIRS}")
endif()

find_package(Boost
  COMPONENTS filesystem system date_time
  REQUIRED)
