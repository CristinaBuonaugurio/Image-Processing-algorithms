#include <opencv2/opencv.hpp>

#include <iostream>
#include <stack>

using namespace std; 
using namespace cv; 

Mat NoMaximaSuppression(Mat imageMag, Mat imagePhase); 
Mat Isteresi(Mat imageNoMax, int th1, int th2); 

int main(int argc, char **argv)
{
	if(argc!=2)
	{
		cout << "numero argomenti errato" << endl; 
		exit(0); 
	}
	Mat image = imread(argv[1], 0); 
	if(image.empty())
		exit(0); 
	int n=5; 
	double sigma=1.4; 
	int th1, th2; 
	th1 = 20; 
	th2 = 60; 
	Mat imageC, sobelX, sobelY, imageMag, imagePhase, imageNoMax, imageFinal; 
	/*Passo 1 Gaussian Blur*/
	GaussianBlur(image, imageC, Size(n,n), sigma, sigma, BORDER_DEFAULT); 

	/* Passo 2 Calcolare Sobel X e Y*/
	Sobel(image, sobelX, CV_32F, 1,0); 
	Sobel(image, sobelY, CV_32F, 0,1); 

	/* Passo 3 calcolare magnitudo */
	magnitude(sobelX, sobelY, imageMag); 
	normalize(imageMag, imageMag, 0, 255, NORM_MINMAX, CV_8U); 
	
	/* Passo 4 calcolare gli angoli di fase */
	phase(sobelX, sobelY, imagePhase, true); 

	/* Passo 5 NoMaximaSuppression */
	imageNoMax = NoMaximaSuppression(imageMag, imagePhase); 
	imshow("Nomaximafoto", imageNoMax);
	
	/* Passo 6 Isteresi */
	imageFinal = Isteresi(imageNoMax, th1, th2); 

	imshow("Originale", image); 
	imshow("Canny", imageFinal);
	waitKey(0);  
	return 0; 
}

Mat NoMaximaSuppression(Mat imageMag, Mat imagePhase)
{
	Mat imageMax = Mat(imageMag.rows, imageMag.cols, CV_8U); 
	for(int i=0; i<imagePhase.rows; i++)
	{
		for(int j=0; j<imagePhase.cols; j++)
		{
			auto angolo = imagePhase.at<float>(i,j); 
			auto pixel = imageMag.at<uchar>(i,j); 

			if(angolo > 180)
				angolo -= 180; 

			if((angolo>=0 && angolo<22.5 )|| (angolo >=157.5 && angolo<=180))
			{
				auto p1 = imageMag.at<uchar>(i, j+1); 
				auto p2 = imageMag.at<uchar>(i, j-1);

				if(pixel>p1 && pixel>p2)
					imageMax.at<uchar>(i,j) = pixel;  
			}

			if(angolo >=22.5 && angolo <67.5)
			{
				auto p1 = imageMag.at<uchar>(i+1,j+1);
				auto p2 = imageMag.at<uchar>(i-1,j-1);

				if(pixel>p1 && pixel>p2)
					imageMax.at<uchar>(i,j) = pixel;  
			}
			if(angolo >= 67.5 && angolo<112.5)
			{
				auto p1 = imageMag.at<uchar>(i+1, j); 
				auto p2 = imageMag.at<uchar>(i-1, j); 

				if(pixel>p1 && pixel > p2)
					imageMax.at<uchar>(i,j) = pixel; 
			}
			if(angolo >= 112.5 && angolo<157.5)
			{
				auto p1 = imageMag.at<uchar>(i+1, j-1); 
				auto p2 = imageMag.at<uchar>(i-1, j+1); 

				if(pixel>p1 && pixel > p2)
					imageMax.at<uchar>(i,j) = pixel; 
			}
		}
	}
	normalize(imageMax, imageMax, 0, 255, NORM_MINMAX, CV_8U);

	return imageMax; 
}

Mat Isteresi(Mat imageNoMax, int th1, int th2)
{
	Mat final = Mat::zeros(imageNoMax.rows, imageNoMax.cols, CV_8U); 
	int rows = imageNoMax.rows; 
	int cols = imageNoMax.cols;
	Point pixel;  
	stack<Point> edges; 
	int x, y; 
	for(int i=0; i<rows -1; i++)
	{
		for(int j=0; j<cols -1; j++)
		{
			auto p = imageNoMax.at<uchar>(i,j); 
			if(p >= th2)
			{
				final.at<uchar>(i,j) = 255; 
				edges.push(Point(i,j)); 
			}
			else if(p<th1)
				final.at<uchar>(i,j) = 0; 
		} 
	}

	while(!edges.empty())
	{
		pixel = edges.top(); 
		edges.pop(); 

		/*poppo il pixel e vado a vedere il suo intorno 3x3 */
		for(int k=-1; k<2; k++)
		{
			for(int t=-1; t<2; t++)
			{
				x = pixel.x + k; 
				y = pixel.y + t; 

				if(x>=0 && x<rows && y>=0 && y<cols)
				{
					auto p = imageNoMax.at<uchar>(x,y); 
					if(p!=0 && p!=255)
					{
						imageNoMax.at<uchar>(x,y) = 255; 
						edges.push(Point(x,y)); 
					}
				}
			}
		}
	}

	cout << "esco dal while" <<endl; 
	threshold(final, final, 254, 255, THRESH_BINARY); 
	return final; 
}
