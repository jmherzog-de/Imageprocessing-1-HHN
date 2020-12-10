#include <iostream>

#include "pointoperator.hpp"

#define CVPLOT_HEADER_ONLY
#include <CvPlot/cvplot.h>

using namespace cv;
using namespace std;

void LightninSimulation(Mat& templateImage)
{
	//split RGB color channels into vectors
	vector <Mat> templateImageChannels(3);
	split(templateImage, templateImageChannels);

	//generate histogram for each color channel of the calibration picture
	vector<int> histTemplateImageR = calcHistogram(templateImageChannels[2]);
	vector<int> histTemplateImageG = calcHistogram(templateImageChannels[1]);
	vector<int> histTemplateImageB = calcHistogram(templateImageChannels[0]);

	// calculate pixel mean (brightness) of each calibration picture color channels
	vector<float> meanTemplateImage(3);
	calcMeanOfPixels(templateImage, meanTemplateImage[0], meanTemplateImage[1], meanTemplateImage[2]);

	// calculate brightness of each color channel of the calibration picture
	vector<float> sigmaTemplateImage(3);
	calcSigmaOfPixels(templateImage, sigmaTemplateImage[0], sigmaTemplateImage[1], sigmaTemplateImage[2]);

	// display parameter for calibration picture
	cout << "---------------------------" << endl;
	cout << "Kalibrierbild Parameter" << endl;
	cout << "---------------------------" << endl;
	cout << "Mean [R]: " << meanTemplateImage[0] << endl;
	cout << "Mean [G]: " << meanTemplateImage[1] << endl;
	cout << "Mean [B]: " << meanTemplateImage[2] << endl;
	cout << "---------" << endl;
	cout << "sigma [R]: " << sigmaTemplateImage[0] << endl;
	cout << "sigma [G]: " << sigmaTemplateImage[1] << endl;
	cout << "sigma [B]: " << sigmaTemplateImage[2] << endl << endl;

	// lightning simulation with parameter c0,cr,cg,cb
	Mat simulationImage;
	templateImage.copyTo(simulationImage);
	
	float c0 = 0.8f;
	int8_t cr = 30;
	int8_t cg = 0;
	int8_t cb = 0;
	changePointParameters(simulationImage, c0, cr, cg, cb);

	// split color channels into vector
	vector <Mat> simulationImageChannels(3);
	split(simulationImage, simulationImageChannels);

	// histogram for each color channel of the simulation image
	vector<int> histSimulationImageR = calcHistogram(simulationImageChannels[2]);
	vector<int> histSimulationImageG = calcHistogram(simulationImageChannels[1]);
	vector<int> histSimulationImageB = calcHistogram(simulationImageChannels[0]);

	// calculate pixel mean (brightness) of each simulation picture color channels
	vector<float> meanSimulationImage(3);
	calcMeanOfPixels(simulationImage, meanSimulationImage[0], meanSimulationImage[1], meanSimulationImage[2]);

	// calculate brightness of each color channel of the simulation picture
	vector<float> sigmaSimulationImage(3);
	calcSigmaOfPixels(simulationImage, sigmaSimulationImage[0], sigmaSimulationImage[1], sigmaSimulationImage[2]);

	// display parameter of the simulations picture
	cout << "---------------------------" << endl;
	cout << "Simulationsbild Parameter" << endl;
	cout << "---------------------------" << endl;
	cout << "Mean [R]: " << meanSimulationImage[0] << endl;
	cout << "Mean [G]: " << meanSimulationImage[1] << endl;
	cout << "Mean [B]: " << meanSimulationImage[2] << endl;
	cout << "---------" << endl;
	cout << "sigma [R]: " << sigmaSimulationImage[0] << endl;
	cout << "sigma [G]: " << sigmaSimulationImage[1] << endl;
	cout << "sigma [B]: " << sigmaSimulationImage[2] << endl << endl;

	// calculate parameter c0 and c1
	float c0_dach = 0.0f;
	float cr_dach = 0.0f;
	float cg_dach = 0.0f;
	float cb_dach = 0.0f;
	c0_dach = ((sigmaSimulationImage[0] / sigmaTemplateImage[0]) + sigmaSimulationImage[1] / sigmaTemplateImage[1] + (sigmaSimulationImage[2] / sigmaTemplateImage[2])) / 3.0;
	cr_dach = (meanSimulationImage[0] / c0_dach) - meanTemplateImage[0];
	cg_dach = (meanSimulationImage[1] / c0_dach) - meanTemplateImage[1];
	cb_dach = (meanSimulationImage[2] / c0_dach) - meanTemplateImage[2];

	// calculate inverse image
	Mat inverseImage;
	simulationImage.copyTo(inverseImage);
	unsigned width = (unsigned)templateImage.cols;
	unsigned height = (unsigned)templateImage.rows;
	for (unsigned x = 0; x < height; x++)
	{
		for (unsigned y = 0; y < width; y++)
		{
			Vec3b color = simulationImage.at<Vec3b>(x, y);
			color[0] = (uchar)((int)color[0] / c0_dach - cb_dach);
			color[1] = (uchar)((int)color[1] / c0_dach - cg_dach);
			color[2] = (uchar)((int)color[2] / c0_dach - cr_dach);
			inverseImage.at<Vec3b>(x, y) = color;
		}
	}

	imshow("Calibration picture", templateImage);
	imshow("Simulation picture", simulationImage);
	imshow("Inverse picture", inverseImage);
}

void Autoscaling(Mat& image)
{
	uchar a = 54;
	uchar b = 100;

	// convert to image to grayscale
	Mat grayImage;
	cvtColor(image, grayImage, COLOR_BGR2GRAY);
	
	// display histogram
	vector<int> histogramGrayImage = calcHistogram(grayImage);
	auto axes1 = CvPlot::plot(histogramGrayImage);
	Mat outputHist = axes1.render(400, 400);
	imshow("Hist", outputHist);

	vector<double> cumsum = calcCumSum(grayImage);

	auto axes = CvPlot::plot(cumsum,"-0");
	Mat matOutput = axes.render(400, 400);
	imshow("CumSum", matOutput);

	for (int j = 0; j < grayImage.rows; j++)
	{
		for (int i = 0; i < grayImage.cols; i++)
		{
			uchar e = grayImage.at<uchar>(j, i);
			if (e < a) {
				grayImage.at<uchar>(j, i) = 0;
			}
			else if (e <= b)
			{
				grayImage.at<uchar>(j, i) *= 255 / (255 - a - b);
			}
			else {
				grayImage.at<uchar>(j, i) = 255;
			}
		}
	}

	imshow("Gray Autoscaled", grayImage);

	// Display new hist
	histogramGrayImage = calcHistogram(grayImage);
	axes1 = CvPlot::plot(histogramGrayImage);
	outputHist = axes1.render(400, 400);
	imshow("Autoscaled Hist", outputHist);

}

int main()
{
	Mat templateImage = imread("optic.bmp", IMREAD_COLOR);

	if (templateImage.empty())
	{
		cout << "Could not read the image." << endl;
		return -1;
	}

	if (templateImage.channels() != 3)
	{
		cout << "The input file isn't a 3 channel color image." << endl;
		return -1;
	}

	//LightninSimulation(templateImage);
	Autoscaling(templateImage);

	waitKey(0);

	return 0;
}