#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream> 

using namespace std; 
using namespace cv; 

// miglior output è il seguente ->  ./log building.png 21 2 0.20
// Altro output buono - > ./log building.png 21 3 0.10


Mat zeroCrossing(Mat lap, float percent); 

int main(int argc, char** argv)
{
	if(argc!=5)
	{
		cout << "non è stato possibile" << endl; 
		exit(0); 
	}
	Mat image = imread(argv[1], 0); 
	int n = atoi(argv[2]); 
	double sigma = atof(argv[3]);
	float percent = atof(argv[4]);  
	Mat lap, gaus; 
	GaussianBlur(image, gaus, Size(n,n), sigma, sigma, BORDER_DEFAULT); 
	Laplacian(gaus, lap, CV_32F, n); 
	Mat final = zeroCrossing(lap, percent);
	normalize(final, final, 0, 255, NORM_MINMAX, CV_8U);  
	imshow("Immagine originale", image); 
	imshow("Immagine LoG", final); 
	waitKey(0); 
	return 0; 
}

Mat zeroCrossing(Mat lap, float percent)
{
	double maxLap;
	minMaxLoc(lap, NULL, &maxLap, NULL, NULL);
	bool flag;
	float thres = maxLap * percent;  
	double minI, maxI;  
	Mat final = Mat::zeros(lap.rows, lap.cols, CV_8U); 
	Mat padded, intorno;
	int range = 2;

	copyMakeBorder(lap, padded, range, range, range, range, BORDER_CONSTANT, Scalar(0)); 

	for(int i=range; i<lap.rows + range; i++)
	{
		for(int j=range; j<lap.cols + range; j++)
		{
			intorno = padded(cv::Range(i-1, i+2), cv::Range(j-1, j+2)); 
			minMaxLoc(intorno, &minI, &maxI, NULL, NULL); 
			auto pixel = lap.at<float>(i,j); 

			if(pixel>0)  //Se è più grande vediamo se il valore minimo nel suo intorno è minore di zero che nel caso lo sia allora   
			{             //allora ci troviamo in uno zero crossing 
				if(minI<0)
					flag = true; 
				else 
					flag = false; 
			}
			if(pixel<0) //Se è più piccolo vediamo se il valore massimo nel suo intorno è maggiore di zero che nel caso lo sia allora
			{			//ci troviamo in uno zero crossing 
				if(maxI>0)
					flag = true; 
				else
					flag= false; 
			}

			if(abs(maxI - minI) > thres && flag == true) //Ora se la differenza è maggiore del thresholding e il flag ci dice che è uno zerocrossing
			{
				final.at<uchar>(i-range, j-range) = 255; //Allora quel pixel è un punto di edge

			}

		}
	}
	return final; 

}