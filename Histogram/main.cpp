#include <iostream>
#include <iomanip>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define CVPLOT_HEADER_ONLY
#include <CvPlot/cvplot.h>

using namespace cv;
using namespace std;

void calcAndPlotHist(std::string windowName, Mat image, int posX = 0, int posY = 0, int sizeX = 400, int sizeY = 400)
{
	Mat hist;
	vector<int> histVector;

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	calcHist(&image, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
	hist.copyTo(histVector);

	auto axes = CvPlot::plot(histVector, "-0");
	Mat matOutput = axes.render(sizeX, sizeY);
	imshow(windowName, matOutput);
	moveWindow(windowName, posX, posY);
	return;
}

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

void calcBrightnessFromHist(vector<int> histogram, int sumOfPixels, float &brightness, float &brightness_quad)
{

	float t;
	brightness = 0.0;
	brightness_quad = 0.0;
	for (unsigned i = 0; i < 256; i++)
	{
		t = i * histogram[i] / (float)sumOfPixels;
		brightness += t;
		brightness_quad += t * i;
	}
	return;
}

void calcContrastFromHist(vector<int> histogram, int sumOfPixels, float &contrast_dest)
{
	float brightness, brightness_quad;
	calcBrightnessFromHist(histogram, sumOfPixels, brightness, brightness_quad);
	contrast_dest = sqrtf(brightness_quad - powf(brightness, 2));
	return;
}

int main()
{
	Mat colorImage = imread("optic.bmp", IMREAD_COLOR);
	Mat grayImage;

	if (colorImage.empty())
	{
		cout << "Could not read the image." << endl;
		return -1;
	}

	if (colorImage.channels() != 3)
	{
		cout << "The input file isn't a 3 channel color image." << endl;
		return -1;
	}

	cvtColor(colorImage, grayImage, COLOR_BGR2GRAY);

	calcAndPlotHist("Gray Image Histogram", grayImage, 100, 100);


	vector<Mat> channels;
	split(colorImage, channels);
	calcAndPlotHist("R", channels[2], 500, 100);
	calcAndPlotHist("G", channels[1], 900, 100);
	calcAndPlotHist("B", channels[0], 1300, 100);
	
	// display images
	imshow("Color Image", colorImage);
	imshow("Gray Image", grayImage);

	waitKey(0);

	return 0;
}