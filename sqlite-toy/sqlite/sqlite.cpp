// sqlite.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <string>
#include "sqlite.h"

using namespace std;

int main(int argc, char* argv[])
{
	string input_buffer;
	while (true) {
		cout << "db > ";
		getline(cin, input_buffer);

		if (input_buffer == ".exit") {
			exit(EXIT_SUCCESS);
		}
		else {
			cout << "Unrecognized command: " << input_buffer << endl;
		}
	}
}
