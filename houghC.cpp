#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace std; 
using namespace cv; 

Mat houghCerchi(Mat image, Mat canny, int th, int Rmin, int Rmax);
int main(int argc, char** argv)
{
	if(argc!=3)
	{
		cout << "Insirire nome n sigma e threshold"; 
		exit(0); 
	}
	Mat image = imread(argv[1], 0);
	if(image.empty())
		exit(0); 
	int n= 5; 
	double sigma = 1.4; 
	int thres = atoi(argv[2]);
	int Rmin = 70; 
	int Rmax = 120;   
	Mat gaus, canny, final;
 
	GaussianBlur(image, gaus, Size(n,n),sigma, sigma, BORDER_CONSTANT); 
	Canny(gaus, canny, 30, 90, 3, false); 

	final = houghCerchi(image, canny, thres, Rmin, Rmax);
	imshow("Originale", canny); 
	imshow("hough", final); 
	waitKey(0);  
	return 0; 
}


Mat houghCerchi(Mat image, Mat canny, int th, int Rmin, int Rmax)
{
	Mat final = image.clone(); 
	int dim[3] = {image.rows, image.cols, Rmax-Rmin}; 
	Mat voti = Mat(3, dim, CV_8U, Scalar(0)); 
	int r,a, b; 
	double theta; 
	for(int x=0; x<image.rows; x++)
	{
		for(int y=0; y<image.cols; y++)
		{
			auto pixel = canny.at<uchar>(x,y);
			if(pixel == 255) 
			{
				for(theta = 0; theta<360; theta++)
				{
					for(r=Rmin; r<Rmax; r++)
					{
						a = x - (r*(cos(theta * CV_PI/180))); 
						b = y - (r*(sin(theta * CV_PI/180))); 
						if(a>0 && a<image.rows && b>0 && b<image.cols) //ci assicuriamo che a e b non siano valori negativi e che non sforino l'immagine
							voti.at<uchar>(a, b, r-Rmin) += 1; 
					}
				}
			}
		}
	}

	cvtColor(final, final, COLOR_GRAY2BGR); 

	for(a=0; a<dim[0]; a++)
	{
		for(b=0; b<dim[1]; b++)
		{
			for(r=0; r<dim[2]; r++)
			{
				auto pixel = voti.at<uchar>(a,b,r); 
				if(pixel >= th)
				{
					circle(final, Point(b,a), r+Rmin, Scalar(0,0,255), 1, LINE_AA); 
				}
			}
		}
	}

	return final; 
}