#@PURPOSE: Provide access to Chris Kohlhoff's ASIO library.
#
#@TARGETS:
#   ASIO::ASIO Chris Kohlhoff's ASIO library
#
#@DESCRIPTION: This module produces a single target 'ASIO::ASIO', that can be
# used to link to the asio library. The behavior of this module is undefined
# unless the asio library is in '../../asio' relative to this file.
#
# The 'asio' target generated by this module uses the "standalone" version of
# asio in that it doesn't depend on any Boost libraries.

set(ASIO_FOUND FALSE)

if(TARGET ASIO::ASIO)
  set(ASIO_FOUND TRUE)
else()
  find_package(Threads QUIET)
  if(NOT Threads_FOUND)
    if(ASIO_FIND_REQUIRED)
      message(FATAL_ERROR "ASIO requires thread support, but it wasn't found.")
    else()
      if(ASIO_FIND_QUIETLY)
        message(STATUS "ASIO requires thread support, but it wasn't found.")
      endif()
    endif()
  else()
    find_path(ASIO_ROOT include/asio.hpp
      PATHS
        # Where asio lives relative to it's official repository
        ${CMAKE_CURRENT_LIST_DIR}/../../asio/asio

        # Where asio should live
        ${CMAKE_CURRENT_LIST_DIR}/../../asio
    )
    if(NOT ASIO_ROOT)
      if(ASIO_FIND_REQUIRED)
        message(FATAL_ERROR "ASIO headers could not be found.")
      else()
        if(ASIO_FIND_QUIETLY)
          message(STATUS "ASIO headers could not be found.")
        endif()
      endif()
    else()
      add_library(ASIO::ASIO INTERFACE IMPORTED GLOBAL)
      set_target_properties(ASIO::ASIO PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE"
        INTERFACE_INCLUDE_DIRECTORIES "${ASIO_ROOT}/include"
      )
      target_link_libraries(ASIO::ASIO INTERFACE Threads::Threads)
      set(ASIO_FOUND TRUE)
    endif()
  endif()
endif()

# ----------------------------------------------------------------------------
# Copyright 2017 Bloomberg Finance L.P.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.