// sqlite.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <string>
#include "sqlite.h"

using namespace std;

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
	PREPARE_SUCCESS, 
	PREPARE_UNRECOGNIZED_STATEMENT,
} PrepareResult;

MetaCommandResult do_meta_command(const string input_buffer)
{
	if (input_buffer == ".exit")
	{
		exit(EXIT_SUCCESS);
	}
	else
	{
		return META_COMMAND_UNRECOGNIZED_COMMAND;
	}
}

typedef enum
{
	STATEMENT_INSERT,
	STATEMENT_SELECT,
} StatementType;

typedef struct
{
	StatementType type;
} Statement;

PrepareResult prepare_statement(const string input_buffer, Statement& statement)
{
	if (input_buffer.substr(0, 6) == "insert")
	{
		statement.type = STATEMENT_INSERT;
		return PREPARE_SUCCESS;
	}
	if (input_buffer.substr(0, 6) == "select")
	{
		statement.type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}
	return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement& statement)
{
	switch (statement.type)
	{
		case (STATEMENT_INSERT):
			cout << "This is where we would do an insert." << endl;
			break;
		case (STATEMENT_SELECT):
			cout << "This is where we would do a select." << endl;
			break;
	}
}

int main(int argc, char* argv[])
{
	string input_buffer;
	while (true) {
		cout << "db > ";
		getline(cin, input_buffer);

		if (input_buffer[0] == '.')
		{
			switch (do_meta_command(input_buffer))
			{
				case (META_COMMAND_SUCCESS):
					continue;
				case (META_COMMAND_UNRECOGNIZED_COMMAND):
					cout << "Unrecognized command: " << input_buffer << endl;
					continue;
			}
		}

		Statement statement;
		switch (prepare_statement(input_buffer, statement))
		{
			case (PREPARE_SUCCESS):
				break;
			case (PREPARE_UNRECOGNIZED_STATEMENT):
				cout << "Unrecognized keyword at start of " << input_buffer << endl;
				continue;
		}

		execute_statement(statement);
		cout << "Executed." << endl;
	}
}
