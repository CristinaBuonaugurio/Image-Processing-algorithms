#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std; 
using namespace cv; 

Mat otsu(Mat image); 
vector<float> getHist(Mat image); 

int main(int argc, char **argv)
{
	if(argc!=2)
	{
		cout<< "no" << endl; 
		exit(0); 
	}
	Mat image = imread(argv[1], 0);
	if(image.empty()) 
		exit(0); 
	normalize(image, image, 0, 255, NORM_MINMAX, CV_8U); 
	imshow("immagine", image); 

	Mat final = otsu(image);
	imshow("otsu",final);
	waitKey(0);  

}

vector<float> getHist(Mat image)
{
	vector<float> ist (256, 0);
	int value;  
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			value = image.at<uchar>(i,j); 
			ist[value] += 1; 
		}
	}
	for(int i=0; i<ist.size(); i++)
	{
		ist[i] /= (image.rows * image.cols); 
	}
	return ist; 
}

Mat otsu(Mat image)
{
	Mat final = Mat(image.rows, image.cols, CV_8U); 
	vector<float> ist = getHist(image);
	/* Ottenuto l'istogramma normalizzato */

	float mG, m1, m2, m3, P1, P2, P3, varianza, varMax; 
	mG = m1 = m2 = m3 = P1 = P2 = P3 = varianza = varMax = 0;
	int kstar1, kstar2; 

	/* Calcoliamo la media globale */
	for(int i=0; i<ist.size(); i++)
	{
		mG += ist[i]*i; 
	} 

	/* Andiamo ora a ciclare su i j e z per trovare le due soglie giuste che massimizzano la varianza interclasse */
	for(int i=0; i<ist.size()-2; i++)
	{
		P1 += ist[i]; 
		m1 += ist[i]*i;

		for(int j=i+1; j<ist.size()-1; j++)
		{
			P2 += ist[j]; 
			m2 += ist[j]*j; 

			for(int z=j+1; z<ist.size(); z++)
			{
				P3 += ist[z]; 
				m3 += ist[z]*z; 

				/*Calcoliamo la varianza */
				varianza = P1*(pow(m1 - mG, 2)) + P2*(pow(m2 - mG, 2)) + P3*(pow(m3 - mG, 2)); 
				if(varianza > varMax)
				{
					varMax = varianza; 
					kstar1 = i; 
					kstar2 = j; 
				}
			}
			P3 = 0; 
			m3 = 0; 
		}
		P2 = 0; 
		m2 = 0; 
	}

	/* trovati i kstar andiamo a fare la sogliatura per isteresi */
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			auto pixel = image.at<uchar>(i, j); 

			if(pixel >= kstar2)
				final.at<uchar>(i,j) = 255; 
			else if(pixel > kstar1)
				final.at<uchar>(i,j) = 127; 
			else
				final.at<uchar>(i,j) = 0; 
		}
	}
	return final; 
}