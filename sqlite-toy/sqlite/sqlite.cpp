// sqlite.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <string>
#include "sqlite.h"

using namespace std;

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

typedef struct
{
	uint32_t id;
	char username[COLUMN_USERNAME_SIZE];
	char email[COLUMN_EMAIL_SIZE];
} Row;

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
	PREPARE_SUCCESS, 
	PREPARE_UNRECOGNIZED_STATEMENT,
	PREPARE_SYNTAX_ERROR,
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
	Row row_to_insert; // only used by insert statement
} Statement;

const uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct {
  uint32_t num_rows;
  void* pages[TABLE_MAX_PAGES];
} Table;

typedef enum
{
	EXECUTE_SUCCESS,
	EXECUTE_TABLE_FULL,
} ExecuteResult;

PrepareResult prepare_statement(const string input_buffer, Statement& statement)
{
	if (input_buffer.substr(0, 6) == "insert")
	{
		int args_assigned = sscanf_s(input_buffer.c_str(), "insert %d %s %s", &statement.row_to_insert.id, statement.row_to_insert.username, sizeof(statement.row_to_insert.username), statement.row_to_insert.email, sizeof(statement.row_to_insert.email));
		if (args_assigned < 3)
		{
			return PREPARE_SYNTAX_ERROR;
		}
		else
		{
			statement.type = STATEMENT_INSERT;
			return PREPARE_SUCCESS;
		}
	}
	if (input_buffer.substr(0, 6) == "select")
	{
		statement.type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}
	return PREPARE_UNRECOGNIZED_STATEMENT;
}

void* row_slot(Table& table, const uint32_t row_num)
{
	uint32_t page_num = row_num / ROWS_PER_PAGE;
	void* page = table.pages[page_num];
	if (page == NULL)
	{
		// Allocate memory only when we try to access page
		page = table.pages[page_num] = malloc(PAGE_SIZE);
	}
	uint32_t row_offset = row_num % ROWS_PER_PAGE;
	uint32_t byte_offset = row_offset * ROW_SIZE;
	return (char*)page + byte_offset;
}

void serialize_row(const Row& source, void* destination)
{
	memcpy((char*)destination + ID_OFFSET, &(source.id), ID_SIZE);
	memcpy((char*)destination + USERNAME_OFFSET, &(source.username), USERNAME_SIZE);
	memcpy((char*)destination + EMAIL_OFFSET, &(source.email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row& destination)
{
	memcpy(&(destination.id), (char*)source + ID_OFFSET, ID_SIZE);
	memcpy(&(destination.username), (char*)source + USERNAME_OFFSET, USERNAME_SIZE);
	memcpy(&(destination.email), (char*)source + EMAIL_OFFSET, EMAIL_SIZE);
}

ExecuteResult execute_insert(Statement& statement, Table& table)
{
	if (table.num_rows >= TABLE_MAX_ROWS)
	{
		return EXECUTE_TABLE_FULL;
	}
	serialize_row(statement.row_to_insert, row_slot(table, table.num_rows));
	table.num_rows += 1;
	return EXECUTE_SUCCESS;
}

void print_row(const Row& row)
{
	printf("(%d, %s, %s)\n", row.id, row.username, row.email);
}

ExecuteResult execute_select(Statement& statement, Table& table)
{
	Row row;
	for (uint32_t i = 0; i < table.num_rows; i++)
	{
		deserialize_row(row_slot(table, i), row);
		print_row(row);
	}
	return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement& statement, Table& table)
{
	switch (statement.type)
	{
		case (STATEMENT_INSERT):
			return execute_insert(statement, table);
		case (STATEMENT_SELECT):
			return execute_select(statement, table);
	}
}

Table new_table()
{
	Table table;
	table.num_rows = 123;
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
	{
		table.pages[i] = NULL;
	}
	return table;
}

void free_table(Table* table)
{
	for (int i = 0; table->pages[i]; i++)
		free(table->pages[i]);

	free(table);
}

int main(int argc, char* argv[])
{
	string input_buffer;
	Table table = new_table();
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

		execute_statement(statement, table);
		cout << "Executed." << endl;
	}
}
