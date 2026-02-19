#ifndef H264_IMAGE_TRANSPORT_SUBSCRIBER_H
#define H264_IMAGE_TRANSPORT_SUBSCRIBER_H

#include <image_transport/simple_subscriber_plugin.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <sensor_msgs/msg/image.hpp>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavutil/samplefmt.h"
#include "libswscale/swscale.h"
}

namespace h264_image_transport
{

class H264Subscriber
  : public image_transport::SimpleSubscriberPlugin<sensor_msgs::msg::CompressedImage>
{
public:
  H264Subscriber();
  virtual ~H264Subscriber();

  virtual std::string getTransportName() const
  {
    return "h264";
  }

protected:
  void subscribeImpl(
    rclcpp::Node * node, const std::string & base_topic, const Callback & callback,
    rmw_qos_profile_t custom_qos, rclcpp::SubscriptionOptions options) override
  {
    subscribeImplWithOptions(node, base_topic, callback, custom_qos, options);
  }

  virtual void internalCallback(const sensor_msgs::msg::CompressedImage::ConstSharedPtr & message,
                                const Callback & user_cb);

private:
  struct SwsContext * convert_ctx_ = nullptr;
  const AVCodec * codec_;
  AVCodecContext * context_;
  AVFrame * picture_;
  AVPacket * avpkt_;

  sensor_msgs::msg::Image::SharedPtr out_;
  uint8_t * dst_ = nullptr;
};

}  // namespace h264_image_transport

#endif
