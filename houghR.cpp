#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace std; 
using namespace cv; 

Mat hough(Mat img, Mat Orig, int th); 
int main(int argc, char **argv)
{
	if(argc!= 3)
	{
		cout << "no" <<endl; 
		exit(0); 
	}
	Mat image = imread(argv[1], 0); 
	if(image.empty())
		exit(0); 

	Mat imageC;
	int n=5, th;
	th = atoi(argv[2]);  
	double sigma= 1.4;  
	GaussianBlur(image, imageC, Size(n,n), sigma, sigma, BORDER_CONSTANT); 
	Canny(imageC, imageC, 30, 90, 3, false); 
	Mat hImage = hough(imageC, image,th); 
	imshow("Originale", image);
	imshow("canny", imageC);  
	imshow("hough", hImage);
	waitKey(0);  
	return 0; 
}


Mat hough(Mat img, Mat Orig, int th)
{
	int distMax = hypot(Orig.rows, Orig.cols); 
	Mat voti = Mat::zeros(distMax*2, 180, CV_8U);
	double theta, i, j;
	double rho; 
	int x,y;  
	for(i=0; i<img.rows; i++)  //Mi calcolo rho e theta dello spazio dei parametri 
	{
		for(j=0; j<img.cols; j++)
		{
			if(img.at<uchar>(i,j) == 255)
			{
				for(theta=0; theta<180; theta++)
				{
					rho =j*cos((theta-90)*CV_PI/180) + i*sin((theta-90)*CV_PI/180);
					voti.at<uchar>(rho, theta) += 1;  
				}
			}
		}
	} 
	Mat imageColor = Orig.clone(); 
	cvtColor(imageColor, imageColor, COLOR_GRAY2BGR); 

	double a, b; 
	Point p1,p2; 
	for(int r=0; r<voti.rows; r++)
	{
		for(int t=0; t<voti.cols; t++)
		{
			if(voti.at<uchar>(r,t) > th)
			{
				theta = (t-90) * CV_PI/180; 
				a = sin(theta); 
				b = cos(theta); 
				x = b*r; 
				y = a*r; 

				p1.x = cvRound(x + distMax*(-a)); 
				p1.y = cvRound(y + distMax*b); 
				p2.x = cvRound(x - distMax*(-a)); 
				p2.y = cvRound(y - distMax*(b)); 
				line(imageColor, p2, p1, Scalar(0,0,255),1,0 ); 
			}
		}
	}
	return imageColor; 
}