#include <iostream>
#include <string>
#include <vector>
#include <tuple>

using namespace std;

int main() {
	string buffer;
	vector<tuple<int, string, int>> tuples;
	
	while(1) {
		getline(cin, buffer);
		
		// If no chars then we're done
		if (!buffer[0])
			break;
		
		// find index of second comma
		int index;
		for (index = 7; index < buffer.length(); index++)
			if (buffer[index] == ',') break;
		
		// Get id, topic and value
		int id = stoi(buffer.substr(1,4));
		string topic = buffer.substr(6, index-6);
		int value = stoi(buffer.substr(index+1,buffer.length()-index+1));
		
		// Update tuples and clear flag
		int flag = 1;
		for (tuple<int,string,int> &t : tuples) {
			if (id == get<0>(t)) {
				if (!topic.compare(get<1>(t))) {
					get<2>(t) += value;
					flag = 0;
				}
			}
		}
		
		// If flag not cleared make new tuple
		if (flag) tuples.push_back(make_tuple(id, topic, value));
		
	}
	
	for (tuple<int,string,int> t : tuples) cout << "(" << get<0>(t) << "," << get<1>(t) << "," << get<2>(t) << ")" << endl;
	cout << endl;
	
	return 0;
}