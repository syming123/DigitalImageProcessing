#include<opencv2/opencv.hpp>
#include<iostream>
#include<cmath>
using namespace cv;
Mat scale(Mat& input, double sx, double sy) {
	Mat output(input.rows * sx, input.cols * sy, CV_8UC4);
	for (int i = 0; i < output.rows; i++)
	{
		for (int j = 0; j < output.cols; j++)
		{
			double x = i / sx;
			double y = j / sy;
			int x1 = floor(x);
			int x2 = ceil(x);
			int y1 = floor(y);
			int y2 = ceil(y);
			x2 = min(x2, input.rows-1);
			y2 = min(y2, input.cols-1);
			for (int k = 0; k < 4; k++)
			{
				if(x1==x2||y1==y2)
					output.at<Vec4b>(i, j)[k] = input.at<Vec4b>(i / sx, j / sy)[k];
				else
				{
					int fr1 = 1.0 * (x2 - x) / (x2 - x1) * input.at<Vec4b>(x1, y1)[k] + (x - x1) / (x2 - x1) * input.at<Vec4b>(x2, y1)[k];
					int fr2 = 1.0 * (x2 - x) / (x2 - x1) * input.at<Vec4b>(x1, y2)[k] + (x - x1) / (x2 - x1) * input.at<Vec4b>(x2, y2)[k];
					output.at<Vec4b>(i, j)[k] = (y2 - y) / (y2 - y1) * fr1 + (y - y1) / (y2 - y1) * fr2;
				}
			}
		}
	}
	return output;
}
Mat transform(Mat &input) {
	Mat output(input.rows, input.cols, CV_8UC4);
	for (int i = 0; i < output.rows; i++)
	{
		for (int j = 0; j < output.cols; j++)
		{
			double x = (i - 0.5 * output.rows) / (0.5 * output.rows);
			double y = (j - 0.5 * output.cols) / (0.5 * output.cols);
			double r = pow(x * x + y * y, 0.5);
			double theta = pow(1 - r, 2);
			for (int k = 0; k < 4; k++)
			{
				if(r>=1)
					output.at<Vec4b>(i, j)[k] = input.at<Vec4b>(i, j)[k];
				else {
					double x0 = cos(theta) * x - sin(theta) * y;
					double y0 = sin(theta) * x + cos(theta) * y;
					output.at<Vec4b>(i, j)[k] = input.at<Vec4b>(0.5 * output.rows * x0 + 0.5 * output.rows, 0.5 * output.cols * y0 + 0.5 * output.cols)[k];
				}
			}
		}
	}
	return output;
}
//int main(int argc, char** argv)
//{
//	Mat img = imread("pics/2/lab2.png", -1);
//	if (img.empty())
//	{
//		printf("could not load image¡­\n");
//		return -1;
//	}
//	//Mat res = scale(img, 0.5, 2);
//	Mat res = transform(img);
//	imshow("test", res);
//	waitKey(0);
//	return 0;
//}