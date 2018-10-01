#ifndef ASTAR_ON_ENV
#define ASTAR_ON_ENV

#include <string>
#include <vector>
#include <argos3/core/utility/math/vector2.h>

//OpenCV libraries
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


std::vector<argos::CVector2> astar_on_env(const std::string& env_path);

std::vector<argos::CVector2> astar_on_env(const cv::Mat& mat_img, const argos::CVector2 start_position,
                                                                  const argos::CVector2 goal_position);

#endif
