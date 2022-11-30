#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<cmath>
#include<vector>

#define MIN_H_W_RAT 0.6
#define MAX_H_W_RAT 2.0
#define MAX_AREA_SIZE 10000
#define MIN_AREA_SIZE 50

using namespace cv;

Mat Gaussian(const Mat& input, double sigma);

int main(int argc, char** argv)
{
	Mat img = imread("pics/4/Orical1.jpg", -1);
	//Mat img = imread("pics/4/Orical2.jpg", -1);
	if (img.empty())
	{
		printf("could not load image…\n");
		return -1;
	}

	// 高斯滤波去除椒盐噪声
	Mat gaussImg = Gaussian(img, 1);

	Mat YCrCbImg,binImg,binImgCov;
	YCrCbImg.create(img.size(), img.type());
	binImg.create(img.size(), img.type());
	binImgCov.create(img.size(), img.type());
	cvtColor(gaussImg, YCrCbImg, COLOR_BGR2YCrCb);
	cvtColor(gaussImg, binImg, COLOR_BGR2GRAY);
	cvtColor(gaussImg, binImgCov, COLOR_BGR2GRAY);

	// 根据肤色生成二值图像
	for (int i = 0; i < YCrCbImg.rows; i++) {
		for (int j = 0; j < YCrCbImg.cols; j++) {
			int Y = YCrCbImg.at<Vec3b>(i, j)[0];
			int Cr = YCrCbImg.at<Vec3b>(i, j)[1];
			int Cb = YCrCbImg.at<Vec3b>(i, j)[2];
			if (Y > 70 && Cr > 133 && Cr < 173 && Cb > 77 && Cb < 127) {
				binImg.at<uchar>(i, j) = 255;
				binImgCov.at<uchar>(i, j) = 0;
			}
			else { 
				binImg.at<uchar>(i, j) = 0; 
				binImgCov.at<uchar>(i, j) = 255;
			}
		}
	}

	// 寻找脸部连通区域
	Mat labels,stats,centers;
	int number = connectedComponentsWithStats(binImg,labels,stats,centers);

	// 寻找眼睛连通区域
	Mat labelsCov, statsCov, centersCov;
	int numberCov = connectedComponentsWithStats(binImgCov, labelsCov, statsCov, centersCov);

	for (int i = 0; i < number; i++) {
		int x = stats.at<int>(i, CC_STAT_LEFT);
		int y = stats.at<int>(i, CC_STAT_TOP);
		int w = stats.at<int>(i, CC_STAT_WIDTH);
		int h = stats.at<int>(i, CC_STAT_HEIGHT);
		int size = stats.at<int>(i, CC_STAT_AREA);
		double rat = 1.0 * h / w;
		// 选出高宽比在 (0.6,2) 之间的脸部连通区域
		if (size > MIN_AREA_SIZE && size < MAX_AREA_SIZE && rat > MIN_H_W_RAT && rat < MAX_H_W_RAT) {
			// 寻找眼部特征
			bool leftEye = false, rightEye = false;
			for (int j = 0; j < numberCov; j++) {
				double centerX = centersCov.at<double>(j, 0);
				double centerY = centersCov.at<double>(j, 1);
				double dx = 1.0 * w / 5;
				double dy = 1.0 * h / 3;
				// 根据五眼特征寻找符合条件的眼部连通区域
				if (centerX > x + dx && centerX < x + 2 * dx && centerY > y && centerY < y + h)
					leftEye = true;
				if (centerX > x + 3 * dx && centerX < x + 4 * dx && centerY > y && centerY < y + h)
					rightEye = true;
			}
			// 判定为人脸
			if (leftEye && rightEye) {
				rectangle(img, Point(x, y), Point(x+w, y+h), Scalar(0, 0, 255));
			}
		}
	}
	
	imshow("test opencv setup", img);
	waitKey(0);
	return 0;
}