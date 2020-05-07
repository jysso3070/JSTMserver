#pragma once
#include "header.h"
#include "struct.h"

class Database_manager
{
public:
	Database_manager();
	~Database_manager();

	void show_sql_error(const char * err_info);
	void sql_HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
	void sql_load_database();
	void sql_update_data(int key_id, short level);
	void sql_insert_new_data(int key_id, string name);

	char* widechar_to_char(SQLWCHAR *str);

	void show_all_db();

public:
	list<PLAYER_DB> m_list_player_db;
};

