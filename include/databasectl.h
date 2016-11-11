#ifndef __DATABASECTL_H_
#define __DATABASECTL_H_

#include <sqlite3.h>

// 1 for yes, 0 for no
int verify_user(const char *username, const char *passwd);

// 1 for yes, 0 for no
int user_isexist(const char *username);

int add_user(const char *username, const char *passwd);

int create_filetable(const char *username);

//int create_passwdtable(const char *tablename);

// 1 for yes, 0 for no
int file_isexist(const char *username, const char *filename);

int add_filename(const char *username, const char *filename, const int filesize);

int get_files(const char *username, char *buf);

int check_file(const char *filename, char *srcfilename);
#endif
