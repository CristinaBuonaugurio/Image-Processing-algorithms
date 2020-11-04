#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

#include <vector>
#include <iostream>
#include <cmath>

using namespace cv; 
using namespace std; 



Mat otsuSan(Mat image); //Con due classi
vector<float> getHIst(Mat img); 
Mat otsuSenpai(Mat image); //Con tre classi


int main(int argc, char**argv)
{
	if(argc!=4)
		exit(0); 

	Mat image = imread(argv[1], IMREAD_GRAYSCALE); 

	if(image.empty())
		exit(0); 
	int n = atoi(argv[2]);
	double sigma = atof(argv[3]);  
	imshow("Immagine originale", image); 
	GaussianBlur(image,image,Size(n,n), sigma, sigma, BORDER_CONSTANT); 
	normalize(image, image, 0, 255, NORM_MINMAX, CV_8U); 
	Mat final = otsuSan(image);
	imshow("Immagine filtrata con Otsu", final);

	Mat senpai = otsuSenpai(image); 
	imshow("Immagine con due soglie Otsu", senpai);   
	waitKey(0); 
	return 0; 
}


Mat otsuSan(Mat image)
{
	Mat final;
	final = Mat(image.rows, image.cols,CV_8U); 
	/* Procedimento */
	float mG = 0;
	float P, mL, varianza, varMax; 
	int kstar = 0;
	float sep;    
	/* Passo 1: Calcoliamo e normalizziamo l'istogramma dell'immagine */
	vector<float> ist = getHIst(image); 	

	/* Passo 2: Calcoliamo la media globale dell'intensità */
	for(int i=0; i<256; i++)
		mG += (ist[i])*i; 

	/* Passo 3: Calcoliamo la media locale, le somme cumulative e la varianza interclasse */
	P = mL = varianza = varMax = 0; 
	for(int k=0; k<256; k++)
	{
		P += ist[k]; 
		mL += ist[k]*k; 
		varianza = pow((mG*P - mL),2)/(P*(1-P));
		if(varMax < varianza)
		{
			kstar = k; 
			varMax = varianza; 
		} 
	}
	/* Passo 4: Sogliare l'immagine con il kstar trovata */ 
	threshold(image, final, kstar, 255, THRESH_BINARY_INV); 
	return final; 
}


vector<float> getHIst(Mat img)
{
	vector<float> ist (256, 0.0f);
	int R = img.rows; 
	int C = img.cols;
	int intensityPixel;
	float p;    
	for(int i=0; i<img.rows; i++)
	{
		for(int j=0; j<img.cols; j++)
		{
			intensityPixel = img.at<uchar>(i,j); 
			ist[intensityPixel] += 1; 

		}
	}	

	/* Calcoliamo la normalizzazione */
	for(int i=0; i<256; i++)
	{	
		p = ist[i]; 
		p = p/(R*C); 
		ist[i] = p; 
	}
	return ist; 
}


Mat otsuSenpai(Mat image)
{
	Mat final = Mat(image.rows, image.cols, CV_8U); 
	float mG,m1,m2,m3,P1,P2,P3,varianza, varMax; 
	mG = m1 = m2 = m3 = P1 = P2 = P2 = varianza = varMax = 0; //Inizializziamo tutte le variabili che ci servono 
	int kstar1, kstar2; 
	/* Passo 1: Calcoliamo e normalizziamo l'istogramma dell'immagine */
	vector<float> ist = getHIst(image);

	/* Passo 2: Calcoliamo la media globale dell'intensità */
	for(int i=0; i<256; i++)
		mG += (ist[i])*i; 
	/* Passo 3: calcoliamo i due k* max */

	for(int i=0; i<256-2; i++)
	{

		P1 += ist[i]; 
		m1 += ist[i]*i;

		for(int j=i+1; j<256-1; j++)
		{
			P2 += ist[j]; 
			m2 += ist[j]*j; 

			for(int z=j+1; z<256; z++)
			{
				P3 += ist[z]; 
				m3 += ist[z]*z; 
				varianza = P1*pow(m1 - mG,2) + P2*pow(m2-mG,2) + P3*pow(m3-mG,2); 
				if(varianza > varMax)
				{
					varMax = varianza;
					kstar1 = i; 
					kstar2 = j; 
				}
			}
			P3=0; 
			m3=0; 
		}
		P2=0; 
		m2=0; 
	}

	cout << "soglia 1: " << kstar1 << "soglia 2 " << kstar2; 
	/* Sogliatura per Isteresi */
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			auto pixel = image.at<uchar>(i,j); 
			if(pixel < kstar1)
				final.at<uchar>(i,j) = 0; 
			else if(pixel >= kstar2)
				final.at<uchar>(i,j) = 255; 
			else
				final.at<uchar>(i,j) = 127; 
		}
	}
	return final; 
}
