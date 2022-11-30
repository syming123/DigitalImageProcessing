#include<opencv2/opencv.hpp>
#include<iostream>
#include<cmath>
using namespace cv;
double* gaussianCore(double sigma) {
    int size = (int)(6 * sigma - 1) / 2 * 2 + 1;
    double* matrix = new double[size];
    const double pi = 3.1415926;
    int center = size / 2;
    for (int i = 0; i < size; i++) {
        double x2 = pow(i - center, 2);
        double t = exp(-x2 / (2 * sigma * sigma));
        matrix[i] = t / (pow(2 * pi, 0.5) * sigma);
    }
    //归一化
    double k = 1.0 / matrix[0];
    for (int i = 0; i < size; i++) {
        matrix[i] = matrix[i] * k;
        //std::cout << matrix[i] << std::endl;
    }
    return matrix;
}
Mat Gaussian(const Mat& input, double sigma) {
    int size = (int)(6 * sigma - 1) / 2 * 2 + 1;
    double* core = gaussianCore(sigma);
    double sum = 0;
    for (int i = 0; i < size; i++){
        sum += core[i];
    }
    Mat output(input.rows, input.cols, CV_8UC3);
    //横向卷积
    for (int l = 0; l < 3; l++) {
        for (int i = 0; i < input.rows; i++) {
            for (int j = 0; j < input.cols; j++) {
                double s = 0;
                for (int k = 0; k < size; k++) {
                    int t = j - size / 2 + k;
                    if (t < 0) t = abs(t);
                    if (t >= input.cols) t = 2 * input.cols - t - 2;
                    s += input.at<Vec3b>(i, t)[l] * core[k];
                }
                output.at<Vec3b>(i, j)[l] = s / sum;
            }
        }
    }
    //纵向卷积
    for (int l = 0; l < 3; l++) {
        for (int i = 0; i < input.cols; i++) {
            for (int j = 0; j < input.rows; j++) {
                double s = 0;
                for (int k = 0; k < size; k++) {
                    int t = j - size / 2 + k;
                    if (t < 0) t = abs(t);
                    if (t >= input.rows) t = 2 * input.rows - t - 2;
                    s += output.at<Vec3b>(t, i)[l] * core[k];
                }
                output.at<Vec3b>(j, i)[l] = s / sum;
            }
        }
    }
    return output;
}

//双线性插值缩放，见实验2
Mat scale(Mat& input, double sx, double sy);

double jbf_fg(double d, double sigma) {
    return exp((-d * d) / (2 * sigma * sigma));
}
Mat jbf(Mat& D, Mat& C, int w, double sigma_f, double sigma_g) {
    int rows = min(D.rows, C.rows);
    int cols = min(D.cols, C.cols);
    Mat res(rows, cols, CV_8UC4);
    for (int l = 0; l < 4; l++) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                double a = 0;
                double b = 0;
                for (int m = 0; m < w; m++) {
                    for (int n = 0; n < w; n++) {
                        int qx = i - w / 2 + m;
                        int qy = j - w / 2 + n;
                        qx = max(0, qx);
                        qx = min(rows - 1, qx);
                        qy = max(0, qy);
                        qy = min(cols - 1, qy);

                        double under = jbf_fg(pow(pow(i - qx, 2) + pow(j - qy, 2), 0.5), sigma_f) *
                            jbf_fg(C.at<Vec4b>(i, j)[l] - C.at<Vec4b>(qx, qy)[l], sigma_g);
                        a += under * D.at<Vec4b>(qx, qy)[l];
                        b += under;
                    }
                }
                res.at<Vec4b>(i, j)[l] = a / b;
            }
        }
    }
    return res;
}
//int main(int argc, char** argv)
//{
//	Mat img = imread("pics/3/a.jpg",-1);
//    Mat img2 = imread("pics/3/2_2.png", -1);
//	if (img.empty() || img2.empty())
//	{
//		printf("could not load image…\n");
//		return -1;
//	}
//
//    Mat res = Gaussian(img, 4);
//
//   /* Mat lr = scale(img2, 0.5, 0.5);
//    Mat introduce = scale(lr, 2, 2);
//    Mat res = jbf(img2, introduce, 7, 10, 20);*/
//
//	imshow("test opencv setup", res);
//	waitKey(0);
//	return 0;
//}