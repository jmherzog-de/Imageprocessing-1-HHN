#include "pointoperator.hpp"

using namespace cv;
using namespace std;

vector<int> calcHistogram(Mat image)
{
	Mat hist;
	vector<int> histVector;

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	calcHist(&image, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
	hist.copyTo(histVector);

	return histVector;
}

void calcMeanOfPixels(Mat& image, float& meanR, float& meanG, float& meanB)
{
	unsigned sumOfPixels = (unsigned)image.total();

	vector<Mat> channels(3);
	split(image, channels);

	vector<int> histogramR = calcHistogram(channels[2]);
	vector<int> histogramG = calcHistogram(channels[1]);
	vector<int> histogramB = calcHistogram(channels[0]);

	for (unsigned i = 0; i < 256; i++)
	{
		meanR += i * histogramR[i] / (float)sumOfPixels;
		meanG += i * histogramG[i] / (float)sumOfPixels;
		meanB += i * histogramB[i] / (float)sumOfPixels;
	}

	return;
}

void calcSigmaOfPixels(Mat& image, float& sigmaR, float& sigmaG, float& sigmaB)
{
	unsigned sumOfPixels = (unsigned)image.total();

	vector<Mat> channels(3);
	split(image, channels);
	float meanR = 0.0;
	float meanG = 0.0;
	float meanB = 0.0;
	float meanQuadR = 0.0;
	float meanQuadG = 0.0;
	float meanQuadB = 0.0;

	vector<int> histogramR = calcHistogram(channels[2]);
	vector<int> histogramG = calcHistogram(channels[1]);
	vector<int> histogramB = calcHistogram(channels[0]);
	float t = 0.0;
	for (unsigned i = 0; i < 256; i++)
	{
		t = i * histogramR[i] / (float)sumOfPixels;
		meanR += t;
		meanQuadR += i * t;
		t = i * histogramG[i] / (float)sumOfPixels;
		meanG += t;
		meanQuadG += i * t;
		t = i * histogramB[i] / (float)sumOfPixels;
		meanB += t;
		meanQuadB += i * t;
	}
	sigmaR = sqrtf(meanQuadR - (meanR * meanR));
	sigmaG = sqrtf(meanQuadG - (meanG * meanG));
	sigmaB = sqrt(meanQuadB - (meanB * meanB));

	return;
}

void changePointParameters(Mat& image, float c0, int8_t cr, int8_t cg, int8_t cb)
{
	unsigned width = image.cols;
	unsigned height = image.rows;

	for (unsigned x = 0; x < width; x++)
	{
		for (unsigned y = 0; y < height; y++)
		{
			Vec3b color = image.at<Vec3b>(y, x); // row, col
			color[0] = (uchar)((int)color[0] * c0 + cb);
			color[1] = (uchar)((int)color[1] * c0 + cg);
			color[2] = (uchar)((int)color[2] * c0 + cr);
			image.at<Vec3b>(y, x) = color;
		}
	}
}