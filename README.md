# h264_image_transport

<sup>Forked from [tilk/h264_image_transport](https://github.com/tilk/h264_image_transport).</sup>

ROS2 support for H264 subscription plugin for the ROS image transport.  
This repo has been created to be used in pair with
the [nvidia_gmsl_driver_ros](https://github.com/UT-ADL/nvidia_gmsl_driver_ros) but can be used to decode any h264
packets published with the `sensor_msgs/CompressedImage` message.

## How to use

### Build

- Create a workspace
  ```bash
  mkdir -p ros2_ws/src
  cd ros2_ws
  ```
- Clone the repo
  ```bash
  git clone git@github.com:UT-ADL/h264_image_transport.git -b ros2 src/h264_image_transport
  ```
- Build and source the workspace
  ```bash
  colcon build --symlink-install
  source install/setup.bash
  ```
- The H264 plugin for the ROS image transport is now available.

### Usage

- You can verify that the plugin is loaded correctly with :
  ```bash
  ros2 run image_transport list_transports
  ```

  You should see :
  ```
    "image_transport/h264"
  - Provided by package: h264_image_transport
  - No publisher provided
  - Subscriber:
    This plugin decodes a h264 video stream.
  ```
- You can then decompress h264 messages like so :
  ```bash
  ros2 run image_transport republish h264 --remap in/h264:=/interfacea/link0/image --remap out:=/interfacea/link0/image/repub
  ```
  For details see the image transport republish [doc](http://wiki.ros.org/image_transport#Nodes).
