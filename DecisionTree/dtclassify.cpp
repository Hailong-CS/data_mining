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




int main(int argc, char const *argv[])
{
	
	return 0;
}