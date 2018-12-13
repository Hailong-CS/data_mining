#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>

using namespace std;

bool sortcol( const vector<int>& v1, 
               const vector<int>& v2 ) { 
 return v1[0] < v2[0]; 
} 

int main(){
/*	vector<int> x = {2, 1, 1, 1, 1};
	if(equal(x.begin()+1, x.end(), x.begin())){
		cout << "All elements are the same." << endl;
	} else {
		cout << "Not the same." << endl;
	}

	vector<vector<int>> v{{3, 4}, {2, 1}, {4, 2}, {6, 3}, {5, 6}, {1, 5}};
	sort(v.begin(), v.end(), sortcol);

	for(int i = 0; i < v.size(); i++){
		cout << v[i][0] << " " << v[i][1] << "  |  ";
	}
	cout << endl;*/


	vector<map<int, int>> vm;
	map<int, int> m;
	m.insert(pair<int, int>(3, 4));
	m.insert(pair<int, int>(2, 1));
	m.insert(pair<int, int>(4, 2));
	m.insert(pair<int, int>(6, 3));
	m.insert(pair<int, int>(5, 6));
	m.insert(pair<int, int>(1, 5));
	vm.push_back(m);
	for(int i = 1; i < 6; ++i){
		cout << vm[0][i] << endl;
	}

}