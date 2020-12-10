#ifndef __POINTOPERATOR_HPP__
#define __POINTOPERATOR_HPP__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

std::vector<int> calcHistogram(cv::Mat image);

void calcMeanOfPixels(cv::Mat& image, float& meanR, float& meanG, float& meanB);

void calcSigmaOfPixels(cv::Mat &image, float &sigmaR, float &sigmaG, float &sigmaB);

void changePointParameters(cv::Mat& image, float c0, int8_t cr, int8_t cg, int8_t cb);

std::vector<double> calcCumSum(cv::Mat& image);

#endif