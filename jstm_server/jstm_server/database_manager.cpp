#include "database_manager.h"


Database_manager::Database_manager()
{
}


Database_manager::~Database_manager()
{
}

void Database_manager::show_sql_error(const char * err_info)
{
	std::cout << err_info << "\n";
}

void Database_manager::sql_HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	} while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT *)NULL) == SQL_SUCCESS) {
		// Hide data truncated.. 
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

void Database_manager::sql_load_database()
{
	
	SQLHENV henv;		// 데이터베이스에 연결할때 사옹하는 핸들
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0; // sql명령어를 전달하는 핸들
	SQLRETURN retcode;  // sql명령어를 날릴때 성공유무를 리턴해줌

	// DB에서 값을 받아올 변수
	SQLINTEGER nKey, nLevel;	// 인티저
	SQLWCHAR szName[11];	// 문자열
	SQLLEN cbName = 0, cbKey = 0, cbLevel = 0;

	setlocale(LC_ALL, "korean"); // 오류코드 한글로 변환
	//std::wcout.imbue(std::locale("korean"));

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0); // ODBC로 연결

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0); // 5초간 연결 5초넘어가면 타임아웃

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"JSTM_DB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
				//retcode = SQLConnect(hdbc, (SQLWCHAR*)L"jys_gameserver", SQL_NTS, (SQLWCHAR*)NULL, SQL_NTS, NULL, SQL_NTS);
				if (retcode == SQL_ERROR) {
					show_sql_error("ODBC access fail");
				}

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); // SQL명령어 전달할 한들

					retcode = SQLExecDirect(hstmt, (SQLWCHAR *)L"SELECT i_key_id, c_name, i_level FROM user_table ORDER BY 1, 2, 3", SQL_NTS); // 모든 정보 다 가져오기
					//retcode = SQLExecDirect(hstmt, (SQLWCHAR *)L"EXEC select_highlevel 90", SQL_NTS); // 90레벨 이상만 가져오기

					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

						// Bind columns 1, 2, and 3  
						retcode = SQLBindCol(hstmt, 1, SQL_C_LONG, &nKey, 4, &cbKey);	// 아이디 키
						retcode = SQLBindCol(hstmt, 2, SQL_UNICODE_CHAR, szName, 11, &cbName); // 이름 유니코드경우 SQL_UNICODE_CHAR 사용
						retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &nLevel, 4, &cbLevel);	// 레벨

						// Fetch and print each row of data. On an error, display a message and exit.  
						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);  // hstmt 에서 데이터를 꺼내오는거
							if (retcode == SQL_ERROR)
								show_sql_error("행 데이터 로드 실패");
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
								wprintf(L"%d: %d %lS %d \n", i + 1, nKey, szName, nLevel);

								// wchar -> char로 변환
								char *temp;
								temp = widechar_to_char(szName);
								cout << "(" << temp << ")\n";
								
								PLAYER_DB *temp_player_db = new PLAYER_DB;

								temp_player_db->DB_key_id = (int)nKey;
								strcpy_s(temp_player_db->name, sizeof(temp), temp);
								temp_player_db->level = (short)nLevel;
								m_list_player_db.push_back(*temp_player_db);

								delete temp_player_db;
								temp_player_db = nullptr;

							}
							else
								break;
						}
					}
					else {
						sql_HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt); // 핸들캔슬
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
						cout << "DataBase access complete. \n";
					}

					SQLDisconnect(hdbc);
				}

				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
}

void Database_manager::sql_update_data(int key_id, short level)
{
	SQLHENV henv;		// 데이터베이스에 연결할때 사옹하는 핸들
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0; // sql명령어를 전달하는 핸들
	SQLRETURN retcode;  // sql명령어를 날릴때 성공유무를 리턴해줌
	SQLWCHAR query[1024];
	wsprintf(query, L"UPDATE user_table SET i_level = %d WHERE i_key_id = %d", level, key_id);


	setlocale(LC_ALL, "korean"); // 오류코드 한글로 변환
	//std::wcout.imbue(std::locale("korean"));

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0); // ODBC로 연결

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0); // 5초간 연결 5초넘어가면 타임아웃

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"JSTM_DB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
				//retcode = SQLConnect(hdbc, (SQLWCHAR*)L"jys_gameserver", SQL_NTS, (SQLWCHAR*)NULL, SQL_NTS, NULL, SQL_NTS);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); // SQL명령어 전달할 한들

					retcode = SQLExecDirect(hstmt, (SQLWCHAR *)query, SQL_NTS); // 쿼리문
					//retcode = SQLExecDirect(hstmt, (SQLWCHAR *)L"EXEC select_highlevel 90", SQL_NTS); // 90레벨 이상만 가져오기

					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						cout << "DataBase update success. \n";
					}
					else {
						sql_HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt); // 핸들캔슬
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}

				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
}

void Database_manager::sql_insert_new_data(int key_id, string name)
{
	SQLHENV henv;		// 데이터베이스에 연결할때 사옹하는 핸들
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0; // sql명령어를 전달하는 핸들
	SQLRETURN retcode;  // sql명령어를 날릴때 성공유무를 리턴해줌
	SQLWCHAR query[1024];
	SQLWCHAR tempid[10];
	int new_level = 1;

	//MultiByteToWideChar(CP_ACP, 0, name, strlen(name), tempid, lstrlen(tempid));

	MultiByteToWideChar(CP_ACP, 0, name.c_str(), name.length() + 1, tempid, lstrlen(tempid));

	wsprintf(query, L"INSERT INTO user_table (i_key_id, c_name, i_level) VALUES (%d, '%s', %d)", key_id, tempid, new_level);


	setlocale(LC_ALL, "korean"); // 오류코드 한글로 변환
	//std::wcout.imbue(std::locale("korean"));

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0); // ODBC로 연결

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0); // 5초간 연결 5초넘어가면 타임아웃

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"JSTM_DB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
				//retcode = SQLConnect(hdbc, (SQLWCHAR*)L"jys_gameserver", SQL_NTS, (SQLWCHAR*)NULL, SQL_NTS, NULL, SQL_NTS);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); // SQL명령어 전달할 한들

					retcode = SQLExecDirect(hstmt, (SQLWCHAR *)query, SQL_NTS); // 쿼리문

					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						cout << "insert success \n";
					}
					else {
						sql_HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt); // 핸들캔슬
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}

				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
}

char* Database_manager::widechar_to_char(SQLWCHAR *str)
{
	char *temp;
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	temp = new char[11];
	WideCharToMultiByte(CP_ACP, 0, str, -1, temp, strSize, 0, 0);
	if (isdigit(temp[strlen(temp) - 1]) == 0) {
		//cout << "문자열공백제거\n";
		temp[strlen(temp) - 1] = 0; // 무슨이유에선진 모르겟지만 아이디마지막문자가 영문일 경우 맨뒤에 공백하나가 추가됨
	}
	//cout << "(" << temp << ")\n";
	return temp;
}

void Database_manager::show_all_db()
{
	for (auto d : m_list_player_db) {
		cout << "name: " << d.name << endl;
		cout << "level: " << d.level << endl;
	}
}

int Database_manager::check_nameLogin(const char * name)
{
	for (auto db : m_list_player_db) {
		if (strcmp(db.name, name) == 0) {
			cout << "name exist" << endl;
			return db.DB_key_id;
		}
	}
	return -1;
}
