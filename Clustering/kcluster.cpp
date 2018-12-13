/**
* @file kcluster.cpp
*
* @copyright 2018 Hailong Zeng, All rights reserved.
*/

/*****************************************************************************
* Includes and namespace
*****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <map>


using namespace std;

typedef map<const int, const double> tuple;

void Trans_to_Vector(ifstream& in, map<const int, const double>* dataset,
					 int* DimenList);

void Trail(int const seed, map<const int, const double>* const dataset,
		   int const num_clusters, double centroids[][5460], string const cf, 
		   int* const DimenList, int Cluster[][8090]);

void SSE(const map<const int, const double> point, int const num_clusters, 
		 double const centroids[][5460], int& belong_to);

void I2(const map<const int, const double> point, int const num_clusters, 
		 double const centroids[][5460], int& belong_to);


void Output(int const Cluster[][8090], ofstream& out, int* const DimenList
			ifstream& in, int const num_clusters);



int main(int const argc, char* const argv[]){
	// Initializations
	ifstream input_file, class_file;
	ofstream output_file;
	string cf = argv[2];
	int num_clusters = atoi(argv[4]);
	int num_trails = atoi(argv[5]);
	int seeds[20] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 
					 29, 31, 33, 35, 37, 39};
	map<const int, const double> dataset[21577];

	// Open inputs and output files
	input_file.open(argv[1]);
	if(!input_file.is_open()){
		cout << "The input file is not open propertly!" << endl;
		abort();
	}
	class_file.open(argv[3]);
	if(!class_file.is_open()){
		cout << "The input file is not open propertly!" << endl;
		abort();
	}
	output_file.open(argv[6]);
	if(!output_file.is_open()){
		cout << "The output file is not open propertly!" << endl;
		abort();
	}

	// Get the dataset and corresponding arrays
	int DimenList[8090];
	// 1st col is current cluster, 2nd is prev cluster
	int Cluster[2][8090] = {{0, 0}, {0, 0}} ;
	
	Trans_to_Vector(input_file, dataset, DimenList);

	// Initialize the centroids
	double centroids[num_clusters][5460] = {{0.0, 0.0}, {0.0, 0.0}};
	

	// Run 20 trails
	for(int i = 0; i < num_trails; i++){
		Trail(seeds[i], dataset, num_clusters, centroids, cf, DimenList, Cluster);
		Output(Cluster, output_file, DimenList, class_file, num_clusters);
//		break;
	}
	








	input_file.close();
	class_file.close();
	output_file.close();

}



void Trans_to_Vector(ifstream& in, map<const int, const double>* dataset,
					 int* DimenList){
	// Initializations
	string line,token;
	int NEWID, DN, cnt = 0;
	double Value;
	map<const int, const double> temp_dic;
	// Because the first line of the file is header
	getline(in, line);
	// loop to get all the data
	while(getline(in, line)){
		// Obtain the three values
		stringstream ss(line);
		getline(ss, token, ',');
		NEWID = atoi(token.c_str());
		getline(ss, token, ',');
		DN = atoi(token.c_str());
		getline(ss, token, ',');
		Value = atof(token.c_str());
		// Insert the values into dataset
		dataset[NEWID].insert(make_pair(DN, Value));
	}

	// Get all IDs that contain dimensions
	for(int i = 0; i < 21577; i++){
		if(dataset[i].size() != 0){
			DimenList[cnt] = i;
			cnt ++;
		}
	}
}


void Output(int const Cluster[][8090], ofstream& out, int* const DimenList, ifstream& in, 
			int const num_clusters){
	for(int i = 0; i < 8090; i++){
		out << DimenList[i] << "," << Cluster[0][i] << endl;
	}

}


void Trail(int const seed, map<const int, const double>* const dataset,
		   int const num_clusters, double centroids[][5460], string const cf, 
		   int* const DimenList, int Cluster[][8090]){
	// Initialize random seed
	srand(seed);
	int ini, cnt = 0, belong_to = -1;
	double min_dis = 10.0;
	
	// Get centroids
	while(cnt < num_clusters){
		ini = rand() % 8090;
		for(auto const& pair: dataset[DimenList[ini]]){
			centroids[cnt][pair.first] = pair.second;
		}
		cnt ++;
	}

	// Iteration step in Kmeans
	while(true){
		bool converage = true;
		if(cf == "SSE" || cf == "sse"){
			// Assign points to the closest initial centroid
			for(int i = 0; i < 8090; i++){
				int belong_to = -1;
				double min_dis = 10.0;
				SSE(dataset[DimenList[i]], num_clusters, centroids, belong_to);
				Cluster[1][i] = Cluster[0][i];
				Cluster[0][i] = belong_to;
			}
		} else if(cf == "I2" || cf == "i2"){
			for(int i = 0; i < 8090; i++){
				int belong_to = -1;
				double min_dis = 10.0;
				I2(dataset[DimenList[i]], num_clusters, centroids, belong_to);
				Cluster[1][i] = Cluster[0][i];
				Cluster[0][i] = belong_to;
			}
		} 


/*		for(int i = 0; i < 8090; i++){
			cout << Cluster[0][i] << " " << Cluster[1][i] << endl;
		}*/

		// Check whether converage
		for(int i = 0; i < 8090; i++){
			if(Cluster[0][i] != Cluster[1][i]){
				converage = false;
				break;
			} 
		}
		// If not converge, update centroids
		if(converage == false){
			for(int i = 0; i < num_clusters; i++){
				for(int j = 0; j < 5460; j++){
					centroids[i][j] = 0;
				}
			}
			int cls_cnt[num_clusters] = {0};
			for(int i = 0; i < 8090; i++){
				double temp_point[5460] = {0};
				for(auto const& pair: dataset[DimenList[i]]){
					temp_point[pair.first] = pair.second;
				}
				for(int j = 0; j < 5460; j++){
					centroids[Cluster[0][i]][j] += temp_point[j];
					cls_cnt[Cluster[0][i]] ++;
				}
			}
			for(int i = 0; i < num_clusters; i++){
				for(int j = 0; j < 5460; j++){
					centroids[i][j] /= cls_cnt[i];
				}
			}
		} else {
			break; // Converage
		}

/*		int same = 0;
		for(int i = 0; i < 8090; i++){
			if(Cluster[0][i] == Cluster[1][i])
				same ++;
		}
		cout << same << endl;*/
	}
}


void SSE(const map<const int, const double> point, int const num_clusters, 
		 double const centroids[][5460], int &belong_to){
	// Initialization
	double temp_point[5460] = {0};
	for(auto const& pair: point){
		temp_point[pair.first] = pair.second;
	}
	double SSE_clusters[num_clusters];
	double min_dis = 10;

	// Calculate the SSE for the point to all centroids
	for(int i = 0; i < num_clusters; i++){
		double SSE = 0.0;
		for(int j = 0; j < 5460; j++){
			SSE += pow(centroids[i][j] - temp_point[j], 2);
		}
		SSE_clusters[i] = SSE;
	}

	// Get the minimum distance and the closest cluster
	for(int i = 0; i < num_clusters; i++){
		if(min_dis > SSE_clusters[i]){
			min_dis = SSE_clusters[i];
			belong_to = i;
		}
	}
}


void I2(const map<const int, const double> point, int const num_clusters, 
		 double const centroids[][5460], int& belong_to){
	// Initialization
	double temp_point[5460] = {0};
	for(auto const& pair: point){
		temp_point[pair.first] = pair.second;
	}
	// Calculate the norm
	double temp_point_norm = 0.0;
	for(int i = 0; i < 5460; i++){
		temp_point_norm += pow(temp_point[i], 2);
	}
	temp_point_norm = sqrt(temp_point_norm);

	double I2_clusters[num_clusters];
	double min_dis = 10;

	// Calculate the I2 for the point to all centroids
	for(int i = 0; i < num_clusters; i++){
		double I2 = 0.0;
		double centroid_norm = 0.0;
		for(int j = 0; j < 5460; j++){
			centroid_norm += pow(centroids[i][j], 2);
		}
		centroid_norm = sqrt(centroid_norm);

		for(int j = 0; j < 5460; j++){
			I2 += centroids[i][j] * temp_point[j];
		}
		I2_clusters[i] = I2 / (temp_point_norm * centroid_norm);
	}

	// Get the minimum distance and the closest cluster
	for(int i = 0; i < num_clusters; i++){
		if(min_dis > I2_clusters[i]){
			min_dis = I2_clusters[i];
			belong_to = i;
		}
	}
}