# Install script for directory: C:/Users/Fredrik/Desktop/PhysicsLib/bullet3-2.86.1/test

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/BULLET_PHYSICS")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Fredrik/Source/Repos/Lilla-Spelprojektet-Grupp-2/Bullet/test/InverseDynamics/cmake_install.cmake")
  include("C:/Users/Fredrik/Source/Repos/Lilla-Spelprojektet-Grupp-2/Bullet/test/SharedMemory/cmake_install.cmake")
  include("C:/Users/Fredrik/Source/Repos/Lilla-Spelprojektet-Grupp-2/Bullet/test/gtest-1.7.0/cmake_install.cmake")
  include("C:/Users/Fredrik/Source/Repos/Lilla-Spelprojektet-Grupp-2/Bullet/test/collision/cmake_install.cmake")
  include("C:/Users/Fredrik/Source/Repos/Lilla-Spelprojektet-Grupp-2/Bullet/test/RobotLogging/cmake_install.cmake")
  include("C:/Users/Fredrik/Source/Repos/Lilla-Spelprojektet-Grupp-2/Bullet/test/BulletDynamics/pendulum/cmake_install.cmake")

endif()

