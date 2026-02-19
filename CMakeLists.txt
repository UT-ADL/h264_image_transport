cmake_minimum_required(VERSION 3.8)
project(h264_image_transport)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavcodec libavutil libswscale)

add_library(h264_image_transport SHARED
  src/h264_subscriber.cpp
  src/manifest.cpp
)

target_include_directories(h264_image_transport PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${FFMPEG_INCLUDE_DIRS}
)

ament_target_dependencies(h264_image_transport
  cv_bridge
  image_transport
  pluginlib
  rclcpp
  sensor_msgs
)

target_link_libraries(h264_image_transport
  ${FFMPEG_LIBRARIES}
)

pluginlib_export_plugin_description_file(image_transport h264_plugins.xml)

install(
  TARGETS h264_image_transport
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(FILES h264_plugins.xml
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
