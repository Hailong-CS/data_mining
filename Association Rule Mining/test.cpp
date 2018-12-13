#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <vector>
#include <assert.h>

using namespace std;

// Globle variales
int ttl_trans = 9, ttl_items = 5, CNT = 0;

// Struct node for Projection Based method
struct Node{
	vector<int> itemset;
	int frequence;
	struct vector<Node*> next;
	Node() : itemset(), frequence(0), next() {};
};

void Projection(vector<int>* const matrix, const vector<int>& items_ID, 
				const vector<int>& freq, int const minsup, Node* node){
	// When the node is null and expand all the first level
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

}

void Rule_Generation(Node* node, vector<int> rule){
	int k = Node->itemset.size();
	
}


int main(){
	vector<int> matrix[ttl_trans+1];
	vector<int> items_ID;
	vector<int> freq;

	// Initialize the matrix
	matrix[0].push_back(1); matrix[0].push_back(2);
	matrix[1].push_back(2); matrix[1].push_back(3); matrix[1].push_back(4);
	matrix[2].push_back(1); matrix[2].push_back(3); matrix[2].push_back(4); matrix[2].push_back(5);
	matrix[3].push_back(1); matrix[3].push_back(4); matrix[3].push_back(5);
	matrix[4].push_back(1); matrix[4].push_back(2); matrix[4].push_back(3);
	matrix[5].push_back(1); matrix[5].push_back(2); matrix[5].push_back(3); matrix[5].push_back(4);
	matrix[6].push_back(2); matrix[6].push_back(3);
	matrix[7].push_back(1); matrix[7].push_back(2); matrix[7].push_back(3);
	matrix[8].push_back(1); matrix[8].push_back(2); matrix[8].push_back(4);
	matrix[9].push_back(2); matrix[9].push_back(3); matrix[9].push_back(5);

/*	for(int i = 0; i <= ttl_trans; i++){
		for(int j = 0; j < matrix[i].size(); j++){
			cout << setw(7) << matrix[i][j] << setw(7);
		}
		cout << endl;
	}*/

	// Initialize the items_ID and freq
	for(int i = 0; i <= ttl_items; i++){
		if(i == 0)
			items_ID.push_back(-1);
		else
			items_ID.push_back(i);
	}
	freq.push_back(-1); freq.push_back(7); freq.push_back(8); freq.push_back(7); 
	freq.push_back(5); freq.push_back(3); 

/*	for(int i = 0; i <= ttl_items; i++){
		cout << setw(7) << items_ID[i] << setw(7);
		if(i == ttl_items)
			cout << endl;
	}

	for(int i = 0; i <= ttl_items; i++){
		cout << setw(7) << freq[i] << setw(7);
		if(i == ttl_items)
			cout << endl;
	}*/

	Node* root = new Node();
	Node* pre_node = new Node();
	vector<int> rule;
	Projection(matrix, items_ID, freq, 4, root);
	Get_Itemset(root);

}