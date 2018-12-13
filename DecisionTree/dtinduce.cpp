#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <numeric>
#include <algorithm>
#include <cmath>

using namespace std;

// Decision Tree node
struct Node{
	int attribute;
	int Level;
	double split_point;
	vector<int> NotUsedAttributes{};
	string position;
	struct Node* left;
	struct Node* right;
	Node(): attribute(-1), Level(), split_point(-1.0), 
	position("Root"), left(NULL), right(NULL){}
};

// The number of records and dimensions (N = 60000, P = 784, L = 10)
static int N = 0, P = 0, L = 0;
// Initialize the Main DataMatrix and its corresponding label
static vector<map<int, int>> DataMatrix;
static vector<int> Label;


void Set_Matrix(ifstream &in, vector<int> &Label, vector<map<int, int>> &DataMatrix, 
	vector<int> &SepAttr);

double Attr_Gini_Compute(vector<pair<int, int>> &SelectAttr, const vector<int> &Label, 
	double &sp){
	// Initializations
	double Gini = 2;
	int idx = 0;
	// Sort the 2D vector and 
	sort(SelectAttr.begin(), SelectAttr.end());
	// Using temp to get how many unique elements in the vector
	vector<int> temp;
	for(int i = 0; i < SelectAttr.size(); i++){
		temp.push_back(SelectAttr[i].first);
	}
	temp.erase(unique(temp.begin(), temp.end()), temp.end());
	// All records for this attribute is the same value (not separatble)
	if(temp.size() == 1){
		return Gini;
	}
	// Initialization and since we have 10 labels and 2 partitions
	double split_point = -1.0;
	double GiniN1, GiniN2, Gini_temp;
	// Iterate all separatable points
	for(int i = 0; i < temp.size(); ++i){
		// Iniitalize the computing matrix each time
		int ComputingMatrix[L][2] = {{0,0}, {0,0}};
		int N1Ttl = 0, N2Ttl = 0;
		double temp_val1 = 0.0, temp_val2 = 0.0;
		// Fill the Computing matirx
		sp = (temp[i] + temp[i+1]) / 2.0;
		for(int j = 0; j < SelectAttr.size(); j++){
			if(SelectAttr[j].first <= sp){
				ComputingMatrix[Label[SelectAttr[j].second]][0]++;
				N1Ttl++;
			} else {
				ComputingMatrix[Label[SelectAttr[j].second]][1]++;
				N2Ttl++;
			}
		}
		// Calculate the Gini index
		for(int j = 0; j < L; j++){
			temp_val1 += pow(ComputingMatrix[j][0]/(double)N1Ttl, 2);
			temp_val2 += pow(ComputingMatrix[j][1]/(double)N2Ttl, 2);
		}
		GiniN1 = 1 - temp_val1;
		GiniN2 = 1 - temp_val2;
		Gini_temp = ((N1Ttl/(double)N) * GiniN1) + ((N2Ttl/(double)N) * GiniN2);
		// Get the minimun Gini index and its split point
		if(Gini_temp < Gini){
			Gini = Gini_temp;
			split_point = sp;
		}
	}
	return Gini;
}

void Split_Feture(const vector<int> &Label, vector<map<int, int>> &DataMatrix, 
	int const minfreq, vector<int> Record, Node *node){
	// Initializations
	Node* Left = new Node;
	Node* Right = new Node;
	vector<int> LeftRecord;
	vector<int> RightRecord;
	double MinGini = 2.0, sp;
	int MinAttr = -1, del_temp;
cout << "The record length: " << Record.size() << endl;
cout << "Not Used Attributes: " << node->NotUsedAttributes.size() << endl;
	// Fill the Gini computing matrix and calculate Gini
	for(int i = 0; i < node->NotUsedAttributes.size(); i++){
		// 2D vector (1 : attribut value; 2 : record label)
		vector<pair<int, int>> SelectAttr;
		// Fill the attribute (2D vector)
		int NowAttr = node->NotUsedAttributes[i];
		double Gini_temp = 0.0; // Store the temperary Gini index calculated by function
		// The map will return 0 if key is not in the map
		for(int j = 0; j < Record.size(); j++){
			SelectAttr.push_back(make_pair(DataMatrix[Record[j]][NowAttr], Record[j]));
		}
		// Calculate Gini index and store it with attribute index
		Gini_temp = Attr_Gini_Compute(SelectAttr, Label, sp);
		if(MinGini > Gini_temp){ // Get the minimum Gini index and its attribute index
			MinGini = Gini_temp;
			MinAttr = NowAttr;
			del_temp = i;
		}		
	}
cout << "minimum Gini: " << MinGini << " | " << "The Attr index: " << MinAttr;
cout << " | " << "The split point: " << sp << endl;
	// Upate the node
	node->attribute = MinAttr;
	node->split_point = sp;
	node->left = Left;
	node->right = Right;
	Left->position = "Left";
	Right->position = "Right";
	Left->Level = node->Level + 1;
	Right->Level = node->Level + 1;
	// Update the records and not used attributes separated to left and right
	vector<int> NotUsedAttributes_temp;
	vector<pair<int, int>> SelectAttr;
	NotUsedAttributes_temp = node->NotUsedAttributes;
	NotUsedAttributes_temp.erase(NotUsedAttributes_temp.begin()+del_temp);
	Left->NotUsedAttributes = NotUsedAttributes_temp;
	Right->NotUsedAttributes = NotUsedAttributes_temp;
	for(int i = 0; i < Record.size(); i++){
		if(DataMatrix[Record[i]][MinAttr] <= sp){
			LeftRecord.push_back(Record[i]);
		} else {
			RightRecord.push_back(Record[i]);
		}

	}
cout << "The left record size: " << LeftRecord.size() << " | ";
cout << "The right record size: " << RightRecord.size() << endl;
cout << "------------------------------------------------------------------------------" << endl;

	// Recursive call
	// The frequence has to be greater than minfreq
	if(LeftRecord.size() >= minfreq && RightRecord.size() >= minfreq){
		Split_Feture(Label, DataMatrix, minfreq, LeftRecord, node->left);
		Split_Feture(Label, DataMatrix, minfreq, RightRecord, node->right);
	} else {
		return;
	}


}

void Print_Model(ofstream &out, Node *node){
	if(node->left == NULL && node->right == NULL) return;
	out << node->Level << " " << node->position << " ";
	out << node->attribute << " " << node->split_point << endl;
	
	Print_Model(out, node->left);
	Print_Model(out, node->right);
	
}

int main(int const argc, char* const argv[]){
	// Initializations
	ifstream file_in;
	ofstream file_out;
	int minfreq = atoi(argv[2]);

	// Open input and output files
	file_in.open(argv[1]);
	if(!file_in.is_open()){
		cout << "The input file is not open propertly!" << endl;
		abort();
	}
	file_out.open(argv[3]);
	if(!file_out.is_open()){
		cout << "The output file is not open propertly!" << endl;
		abort();
	}

	// Initialization for data matrix and find separatable attributes
	vector<int> SepAttr;
	Set_Matrix(file_in, Label, DataMatrix, SepAttr);

//	cout << N << "   " << P << "   " << L << endl;
//	cout << SepAttr.size() << endl;

	// Initialize the root, especially the not used attributes (ttl 771)
	vector<int> v(N);
	iota(v.begin(), v.end(), 0);
	Node* root = new Node;
	root->NotUsedAttributes = SepAttr;
	Split_Feture(Label, DataMatrix, minfreq, v, root);
	Print_Model(file_out, root);


/*	cout << Label.size() << "  " << DataMatrix.size() << endl;
	sort(Label.begin(), Label.end());
	for(int i = 0; i < 60000; i++){
		cout << Label[i] << endl;
	}
	map<int, int>::iterator itr;
	for(int i = 0; i < 10; i++){
		for(itr = DataMatrix[i].begin(); itr != DataMatrix[i].end(); ++itr){
			cout << itr->first << " " << itr->second << "  |  ";
		}
		cout << endl;
	}*/



	file_in.close();
	file_out.close();
	return 0;
}



//#include <unistd.h>
/*void Get_Input_File(ifstream& in){
	// Go to the data directory
	chdir("data/rep1")
	// Open input files
	file_in.open(argv[1]);
	if(!file_in.is_open()){
		cout << "The input file is not open propertly!" << endl;
		abort();
	}
}*/
/*	// Check the node whether it could be splited
	if(LeftNode.size() > minfreq){

	}*/


void Set_Matrix(ifstream &in, vector<int> &Label, vector<map<int, int>> &DataMatrix,
	vector<int> &SepAttr){
	// Initializations
	int arr[1000] = {0};
	map<int, int> record;
	string line, token;
	// Loop through the file to build the DataMatrix (p = 784 (no label))
	while(getline(in, line)){
		stringstream ss(line);
		int p = -1; // Because the first entry is label, and reset p
		// Loop through each line, only get attributes != 0
		while(getline(ss, token, ',')){
			if(p == -1){ // Get label
				Label.push_back(atoi(token.c_str()));
			} else if(atoi(token.c_str()) != 0){ // Get attributes != 0
				record.insert(pair<int, int>(p, atoi(token.c_str())));
				arr[p] = 1;
			}
			p++;
			if(P < p){ // Get the largest dimension of the record
				P = p;
			}
		}
		// Push each record to the DataMatrix and clear(clear to size 0) the record
		DataMatrix.push_back(record);
		record.clear();
		N++;
	}
	// Check how many unique labels
	vector<int> temp = Label;
	sort(temp.begin(), temp.end());
	temp.erase(unique(temp.begin(), temp.end()), temp.end());
	L = temp.size();
	// Get separatable attributes
	for(int i = 0; i < P; i++){
		if(arr[i] != 0){
			SepAttr.push_back(i);
		}
	}
}