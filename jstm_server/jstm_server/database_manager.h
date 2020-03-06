#pragma once
#include "header.h"
class database_manager
{
public:
	database_manager();
	~database_manager();

	void show_sql_error();
	void sql_HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
	void sql_load_database();

	char* widechar_to_char(SQLWCHAR *str);

	void show_all_db();

public:
	list<PLAYER_DB> m_list_player_db;
};

