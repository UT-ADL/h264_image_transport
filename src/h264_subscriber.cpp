#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/image_encodings.hpp>

#include "h264_image_transport/h264_subscriber.h"

namespace h264_image_transport
{

H264Subscriber::H264Subscriber()
{
  av_log_set_level(AV_LOG_FATAL);
  codec_ = avcodec_find_decoder(AV_CODEC_ID_H264);
  if (!codec_) {
    throw std::runtime_error("H264 codec not found, check that your ffmpeg is built with H264 support");
  }
  context_ = avcodec_alloc_context3(codec_);
  if (avcodec_open2(context_, codec_, nullptr) < 0) {
    throw std::runtime_error("Cannot open H264 codec");
  }
  picture_ = av_frame_alloc();
  avpkt_ = av_packet_alloc();
  out_ = std::make_shared<sensor_msgs::msg::Image>();
}

H264Subscriber::~H264Subscriber()
{
  av_packet_free(&avpkt_);
  avcodec_free_context(&context_);
  av_frame_free(&picture_);
  sws_freeContext(convert_ctx_);
}

void H264Subscriber::internalCallback(
  const sensor_msgs::msg::CompressedImage::ConstSharedPtr & message, const Callback & user_cb)
{
  avpkt_->size = message->data.size();
  avpkt_->data = const_cast<uint8_t *>(message->data.data());

  if (avcodec_send_packet(context_, avpkt_) != 0) {
    static rclcpp::Clock clk(RCL_STEADY_TIME);
    RCLCPP_ERROR_THROTTLE(rclcpp::get_logger("h264_image_transport"), clk, 5000,
                          "Failed to feed h264 frame to ffmpeg");
  }

  if (avcodec_receive_frame(context_, picture_) != 0) {
    // No frame yet, happens with SPS and PPS frames.
    return;
  }

  convert_ctx_ =
    sws_getCachedContext(convert_ctx_, picture_->width, picture_->height, AV_PIX_FMT_YUV420P,
                         picture_->width, picture_->height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR,
                         nullptr, nullptr, nullptr);
  out_->data.resize(picture_->width * picture_->height * 3);
  int stride = 3 * picture_->width;
  dst_ = out_->data.data();
  sws_scale(convert_ctx_, picture_->data, picture_->linesize, 0, picture_->height, &dst_, &stride);

  out_->width = picture_->width;
  out_->height = picture_->height;
  out_->step = 3 * picture_->width;
  out_->encoding = sensor_msgs::image_encodings::BGR8;
  out_->header = message->header;
  user_cb(out_);
}

}  // namespace h264_image_transport
