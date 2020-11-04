#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

using namespace std; 
using namespace cv; 

Mat kmeans(Mat image, int cluster); 

int main (int argc, char ** argv)
{
	if(argc!=3)
		exit(0); 
	srand(time(NULL)); 
	Mat image = imread(argv[1], 1); 
	int cluster = atoi(argv[2]); 
	Mat clus = kmeans(image, cluster);
	imshow("Immagine", image); 
	imshow("Cluster", clus); 
	waitKey(0); 
	return 0;  
}


Mat kmeans(Mat image, int cluster)
{
	Mat final = Mat(image.rows, image.cols, CV_8U); 
	/* Abbiamo bisogno di un vettore per le occorrenze, le somme e i cluster */
	vector<Vec3d> centriOccorenze (cluster,0); 
	vector<Vec3d> VecClust(cluster, 0); 
	vector<Vec3d> sommeCluster(cluster,0);
	int x,y, centroAssegnato; 
	int rows = image.rows; 
	int cols = image.cols;
	bool flag=false; 
	int n=0; 
	bool flagMedia = false;
	int distanza, distanzaMin;  
	for(int i=0; i<cluster; i++)  //Inizializziamo randomicamente i centroidi dei cluster 
	{
		do
		{
			flag=false; 
			x = rand()%rows; 
			y = rand()%cols; 

			for(int k=0; k<i; k++)
			{
				if(image.at<Vec3b>(x,y)[0] == VecClust.at(k)[0] && 
					image.at<Vec3b>(x,y)[1] == VecClust.at(k)[1] &&
					image.at<Vec3b>(x,y)[2] == VecClust.at(k)[2])
					flag=true; 	
			}
			if(flag == false)
			{
				VecClust.at(i)[0] = image.at<Vec3b>(x,y)[0]; 
				VecClust.at(i)[1] = image.at<Vec3b>(x,y)[1]; 
				VecClust.at(i)[2] = image.at<Vec3b>(x,y)[2]; 
			}
		}
		while(flag); 
	}

	while(n<100 && flagMedia!=true)
	{
		/* Assegniamo ai pixel il centroide giusto */

		for(int i=0; i<image.rows; i++)
		{
			for(int j=0; j<image.cols; j++)
			{
				auto b = image.at<Vec3b>(i,j)[0]; 
				auto g = image.at<Vec3b>(i,j)[1]; 
				auto r = image.at<Vec3b>(i,j)[2]; 

				distanza = pow(b-VecClust.at(0)[0],2) + pow(g - VecClust.at(0)[1],2) + pow(r - VecClust.at(0)[2],2);
				distanzaMin = distanza;
				centroAssegnato = 0;   
				for(int k=1; k<cluster; k++)
				{
					distanza = pow(b - VecClust.at(k)[0],2) +
								pow(g - VecClust.at(k)[1],2)+
								pow(r - VecClust.at(k)[2],2); 
					if(distanza < distanzaMin)
					{
						distanzaMin = distanza; 
						centroAssegnato = k; 
					}
				}
				/* trovato il centro giusto assegno all'immagine finale e mi segno l'occorrenza */
				sommeCluster.at(centroAssegnato)[0] += b;
				sommeCluster.at(centroAssegnato)[1] += g; 
				sommeCluster.at(centroAssegnato)[2] += r; 

				centriOccorenze.at(centroAssegnato)[0] += 1; 
				centriOccorenze.at(centroAssegnato)[1] += 1; 
				centriOccorenze.at(centroAssegnato)[2] += 1; 
				
				final.at<uchar>(i,j) = centroAssegnato; 

			}
		}

		//Mi salvo i vecchi centri che fanno da media precedente 
		vector<Vec3d> mediaPrec(VecClust); 

		for(int i=0; i<cluster; i++)
		{
			for(int k=0; k<3; k++)
			{
				VecClust.at(i)[k] = sommeCluster.at(i)[k]/centriOccorenze.at(i)[k];  //Aggiorniamo i centroidi
				if( VecClust.at(i)[k] == mediaPrec.at(i)[k]) //basta che al più uno sia uguale 
					flagMedia = true; 

				/* Nel momento che ho aggiornato i centroidi vado ad azzerrare la somma e l'occorrenze dei centri */
				sommeCluster.at(i)[k] = 0; 
				centriOccorenze.at(i)[k] = 0; 
			}
		}
		n++; 
	} //fine while 


	/*Andiamo a sistemare l'immagine di output */	
	for(int i=0; i<final.rows; i++)
	{
		for(int j=0; j<final.cols; j++)
		{
			final.at<uchar>(i,j) *= 255/cluster; 
		}
	}

	return final; 
}