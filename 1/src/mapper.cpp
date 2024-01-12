#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
	char buffer[24];
	char output[24];
	
	while(1) {
		int index = 0;
		cin.getline(buffer, 24);
		
		// If no chars then we're done
		if (!buffer[0])
			break;
		
		// id
		for (int i = 0; i < 6; i++)
			output[index++] = buffer[i];
		
		// topic
		for (int i = 8; i < 24 && buffer[i] != ')'; i++)
			output[index++] = buffer[i];
		
		// Depending on the action character which is always at index 6 we change the number
		output[index++] = ',';
		
		switch (buffer[6]) {
			case 'P':
				output[index++] = '5';
				break;
			case 'L':
				output[index++] = '2';
				break;
			case 'D':
				output[index++] = '-';
				output[index++] = '1';
				break;
			case 'C':
				output[index++] = '3';
				break;
			case 'S':
				output[index++] = '4';
				break;
			default:
				// error
				break;
		}
		
		output[index++] = '0';
		output[index++] = ')';
		output[index] = 0;
		
		cout << output << endl;
	}
	
	cout << endl;
	return 0;
}