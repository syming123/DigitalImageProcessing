//#include<opencv2/opencv.hpp>
//#include<iostream>
//using namespace cv;
//int main(int argc, char** argv)
//{
//	Mat img = imread("pics/1/a.png",-1);
//	Mat bg = imread("pics/1/bg.png",-1);
//	if (img.empty() || bg.empty())
//	{
//		printf("could not load image¡­\n");
//		return -1;
//	}
//	Mat res(img.rows, img.cols, CV_8UC4);
//	for (int i = 0; i < img.rows; i++)
//	{
//		for (int j = 0; j < img.cols; j++)
//		{
//			double opaque = 1.0 * img.at<Vec4b>(i, j)[3] / 255;
//			for (int k = 0; k < 3; k++)
//			{	
//				res.at<Vec4b>(i,j)[k] = opaque*img.at<Vec4b>(i, j)[k] + (1-opaque)*bg.at<Vec3b>(i, j)[k];
//			}
//			res.at<Vec4b>(i, j)[3] = img.at<Vec4b>(i, j)[3];
//		}
//	}
//	imshow("test", res);
//	waitKey(0);
//	return 0;
//}