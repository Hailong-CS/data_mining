/**
* @file hcrminer.cpp
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
#include <assert.h>

using namespace std;

// Struct node for Projection Based method
struct Node{
	Node() : itemset(), frequence(0), next() {};
	vector<int> itemset;
	int frequence;
	struct vector<Node*> next;
};

// Globle variales
int ttl_trans, ttl_items, CNT=0;

// Some seter and geter function for Node


// Get the total transaction number and total item number
void Total_nums(ifstream& in, int& ttl_trans, int& ttl_items);

// Set the data matrix with a fixed array of vecotrs
void Set_Matrix(ifstream& in, vector<int>* matrix, vector<int>* items_occur, 
				vector<int>& items_ID, vector<int>& freq, int const opt);

// Using recursive to implement the projection based algorithm
void Projection(vector<int>* const matrix, const vector<int> items_ID, 
				const vector<int> freq, int const minsup, Node* node);

// Retrieve the tree structure to get all the qulified frequent itemsets
void Get_Itemset(ofstream& out, const Node* node);

int main(int const argc, char* const argv[]){
	// Check the inputs and option
	if(argc != 6) {
		cout << "The input parameters are not correct!" << endl;
		abort();
	} else if(argv[5] == string("1") || argv[5] == string("2") 
				|| argv[5] == string("3")){
		;
	} else {
		cout << "The input option is illegal!" << endl;
		abort();
	}

	// Initializations
	int minsup = atoi(argv[1]), minconf = stof(argv[2]), opt = atoi(argv[5]);
	ifstream file_in;
	ofstream file_out;

	// Open input and output files
	file_in.open(argv[3]);
	if(!file_in.is_open()){
		cout << "The input file is not open propertly!" << endl;
		abort();
	}
	file_out.open(argv[4]);
	if(!file_out.is_open()){
		cout << "The output file is not open propertly!" << endl;
		abort();
	}

	// Initializations and set the data matrix
	Total_nums(file_in, ttl_trans, ttl_items);
	vector<int> matrix[ttl_trans+1];
	vector<int> items_occur[ttl_items+1];
	vector<int> items_ID;
	vector<int> freq;
	Set_Matrix(file_in, matrix, items_occur, items_ID, freq, opt);

	// Construct the projection-based tree
	Node* root = new Node();
	Projection(matrix, items_ID, freq, minsup, root);

	

/*	cout << freq[0] << setw(5) << freq[1] << setw(5) << freq[38] << setw(5) << freq[39] << setw(5);
	cout << freq[40] << setw(5) << freq[41] << setw(5) << freq[42] << setw(5) << freq[43] << setw(5);
	cout << freq[44] << setw(5) << freq[45] << endl;*/

	// Check Matrix
	for(int i = 0; i <= 30; i++){
		cout << setw(5) << i << setw(5) << "|| ";
		for(int j = 0; j < matrix[i].size(); j++){
			cout << matrix[i][j] << setw(6);
			if((j > 10) || (j == matrix[i].size()-1)){
				cout << endl;
				break;
			}
		}
	}

/*	// Check items
	for(int i = 1; i <= 45; i++){
		cout << items_ID[i] << " || ";
		for(int j = 0; j < items_occur[i].size(); j++){
			cout << items_occur[i][j] << " | ";
			if(j > 10){
				cout << endl;
				break;
			}
			if(j == items_occur[i].size()-1){
				cout << endl;
			}
		}
	}*/

/*	for(int i = 1; i <= ttl_items; i++){
		cout << items_ID[i] << setw(7);
		if(i % 10 == 0)
			cout << endl;
		if(i == ttl_items)
			cout << endl;
	}*/
	
//cout << items_ID[0] << endl;




	// Close the files
	file_in.close();
	file_out.close();
	return 0;
}


void Total_nums(ifstream& in, int& ttl_trans, int& ttl_items){
	// Initializations
	string stid, siid;
	int comp = -1, temp;
	// Loop the whole file
	while(!in.eof()){
		in >> stid >> siid;
		// Get the largest item ID
		temp = stoi(siid);
		if(comp < temp){comp = temp;}
	}
	// Clear the flags and back to the begining of file
	in.clear();
	in.seekg(0, ios::beg);

	ttl_trans = stoi(stid);
	ttl_items = comp;
}

void Set_Matrix(ifstream& in, vector<int>* matrix, vector<int>* items_occur, 
				vector<int>& items_ID, vector<int>& freq, int const opt){
	// Initializations
	int count = 0, comp = -1, tid, iid;
	string temp1, temp2;
	
	// Loop through the file and put the items into the corresponding TID
	while(!in.eof()){
		// Get the first string and the second string and convert into integer
		in >> temp1 >> temp2;
		tid = stoi(temp1);
		iid = stoi(temp2);
		// Since the while loop will loop one more time the set the flag, I terminate the loop manually
		if(in.eof()){break;}
		// Set the items to corresponding TID and get the list of item
		if(count == tid){
			matrix[count].push_back(iid);
			items_occur[iid].push_back(tid);
		} else {
			count ++;
			matrix[count].push_back(iid);
			items_occur[iid].push_back(tid);
		}
		// Obtain the largest item ID
		if(comp < iid){comp = iid;}
	}
	// Clear the flags and back to the begining of file
	in.clear();
	in.seekg(0, ios::beg);

	// Set the items ID
	for(int i = 0; i <= ttl_items; i++){
		if(i == 0){
			items_ID.push_back(-1);
		} else{
			items_ID.push_back(i);
		}
	}

	// Set the frequence list
	vector<int> new_freq;
	for(int i = 0; i <= comp; i++){
		if(i == 0){
			freq.push_back(-1);
			new_freq.push_back(-1);
		} else{
			freq.push_back(items_occur[i].size());
			new_freq.push_back(items_occur[i].size());
		}
	}


	// Set the matrix as the optional commands
	if(opt == 2){
		for(int i = 0; i <= count; i++){
			// Using bubble sort since data is not large
			for(int j = 0; j < matrix[i].size()-1; j++){
				for(int k = 0; k < matrix[i].size()-j-1; k++){
					if(freq[matrix[i][k]] > freq[matrix[i][k+1]]){
						swap(matrix[i][k], matrix[i][k+1]);
					}
				}
			}
		}
		for(int i = 1; i <= comp; i++){
			for(int j = 1; j <= comp-i; j++){
				if(new_freq[j] > new_freq[j+1]){
					swap(new_freq[j], new_freq[j+1]);
					swap(items_ID[j], items_ID[j+1]);
				}
			}
		}
	} else if(opt == 3){
		for(int i = 0; i <= count; i++){
			// Using bubble sort since data is not large
			for(int j = 0; j < matrix[i].size()-1; j++){
				for(int k = 0; k < matrix[i].size()-j-1; k++){
					if(freq[matrix[i][k]] < freq[matrix[i][k+1]]){
						swap(matrix[i][k], matrix[i][k+1]);
					}
				}
			}
		}
		for(int i = 1; i <= comp; i++){
			for(int j = 1; j <= comp-i; j++){
				if(new_freq[j] < new_freq[j+1]){
					swap(new_freq[j], new_freq[j+1]);
					swap(items_ID[j], items_ID[j+1]);
				}
			}
		}
	} else {
		return;
	}
}

void Projection(vector<int>* const matrix, const vector<int> items_ID, 
				const vector<int> freq, int const minsup, Node* node){
	// When the node is null and expand all the first level
	cout << ++CNT << endl;
	if((node->itemset.size() == 0) && (node->next.size() == 0)){
		for(int i = 1; i < items_ID.size(); i++){
			if(freq[items_ID[i]] > minsup){
				Node* current = new Node();
				current->itemset.push_back(items_ID[i]);
				current->frequence = freq[items_ID[i]];
				node->next.push_back(current);
			}
		}
	} else {
	// Initializations for new node
		for(int i = 1; i < items_ID.size(); i++){
			Node* current = new Node();
			for(int j = 0; j < node->itemset.size(); j++){
				current->itemset.push_back(node->itemset[j]);
			}
			current->itemset.push_back(items_ID[i]);
			node->next.push_back(current);
		}
	}

	// Initialize the deducting database and items_ID
	vector<int> new_matrix[ttl_trans+1];
	vector<int> new_items_ID;
	for(int i = 0; i < items_ID.size(); i++){
		new_items_ID.push_back(items_ID[i]);
	}

	for(int nn = 0; nn < node->next.size(); nn++){
		// When iterate the for loop, we need the original matrix
		for(int i = 0; i <= ttl_trans; i++){
			new_matrix[i].clear();
		}
		for(int i = 0; i <= ttl_trans; i++){
			for(int j = 0; j < matrix[i].size(); j++){
				new_matrix[i].push_back(matrix[i][j]); 
			}
		}
		// Initialize the current
		Node* current = new Node();
		current = node->next[nn];

		if(current->frequence > 0){
			// Deducte the database(matrix) and the items_ID
			for(int i = 0; i <= ttl_trans; i++){
				for(int j = 0; j < new_matrix[i].size(); j++){
					if(new_matrix[i][j] == current->itemset[0]){
						new_matrix[i].erase(new_matrix[i].begin()+j);
						break;
					} else {
						new_matrix[i].erase(new_matrix[i].begin()+j);
						j--;
					}
				}
			}
			new_items_ID.erase(new_items_ID.begin()+1);

			int length = 0;
			for(int i = 0; i <= ttl_trans; i++){
				if(length < new_matrix[i].size()){
					length = new_matrix[i].size();
				}
			}
			if(current->frequence == 0){
			// Back to the forloop when the itemset's frequence is 0
				continue;
			} else if(length == 0){
			// Back to the forloop when the itemset cannot expand
				continue;
			} else {
				Projection(new_matrix, new_items_ID, freq, minsup, current);
			}
		} else {
			// Count the itemset's frequent count
			int compare = current->itemset.back();
			for(int i = 0; i <= ttl_trans; i++){
				int checker = 0;
				for(int j = 0; j < new_matrix[i].size(); j++){
					// All items in itemset should in one transection
					if(new_matrix[i][j] == compare){
						checker = 1;
						break;
					} else {
						checker = 0;
					}
				}
				if(checker == 1){
					current->frequence ++;
					// Need to deduct the database(matrix)
					for(int j = 0; j < current->itemset.size(); j++){
						for(int k = 0; k < new_matrix[i].size(); k++){
							if(new_matrix[i][k] == current->itemset[j]){
								new_matrix[i].erase(new_matrix[i].begin()+k);
							}
						}
					}
				} else {
					new_matrix[i].clear();
				}
			}
			// Deduct the items_ID set
			new_items_ID.erase(new_items_ID.begin()+1);

			int length = 0;
			for(int i = 0; i <= ttl_trans; i++){
				if(length < new_matrix[i].size()){
					length = new_matrix[i].size();
				}
			}
			if(current->frequence == 0){
			// Back to the forloop when the itemset's frequence is 0
				continue;
			} else if(length == 0){
			// Back to the forloop when the itemset cannot expand
				continue;
			} else {
				Projection(new_matrix, new_items_ID, freq, minsup, current);
			}
		}
		// Prune when the node's next nodes is infrequent
		for(int i = 0; i < current->next.size(); i++){
			if(current->next[i]->frequence < minsup){
				current->next.erase(current->next.begin()+i);
				i--;
			}
		}
	}
	return;
}
/*
void Get_Itemset(const Node* node){
	for(int k = 0; k < node->next.size(); k++){
		for(int i = 0; i < node->next[k]->itemset.size(); i++){
			if(i == 0)
				cout << "The itemset is ";
			cout << node->next[k]->itemset[i] << "  ";
		}
		cout << endl;
		cout << "The frequence is " << node->next[k]->frequence << endl << endl;
		Get_Itemset(node->next[k]);
	}

}*/
void Get_Itemset(ofstream& out, const Node* node){
	for(int i = 0; i < node->next.size(); i++){
		for(int j = 0; j < node->next[i]->itemset.size(); j++){
			out << node->next[i]->itemset[i] << " ";
		}
		out << node->next[i]->frequence << endl;
		Get_Itemset(out, node->next[i]);
	}
}