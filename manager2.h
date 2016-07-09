#pragma once

#include <winsock.h> // For MySQL
#include <MYSQL/mysql.h> // For MySQL
#include <tchar.h>
#include <windows.h>
#include <string.h>
#include "resource.h"
#include "utilitaire.h"

typedef struct Student{
    int id;
    char* name;
    char* first_name;
    char* email;
} Student;

typedef struct Group{
    int id;
    char* name;
} Group;

typedef struct Subject{
    int id;
    char* name;
    char* shortName;
    char* color;
} Subject;

typedef struct Teacher{
    int id;
    char* name;
    int id_subjects;
} Teacher;

typedef struct Kholleur{
    int id;
    char* name;
    int id_subjects;
    int duration;
    int preparation;
    int pupils;
} Kholleur;

extern MYSQL* mysql;

// ==============  GENERAL MANAGER  ============== //
char* GetItemText(HWND hwnd, int idText);
void updateList(HWND hwnd, const char* request, char* generateStr(const MYSQL_ROW), void* setData(const MYSQL_ROW));
//BOOL CALLBACK DB_Manager_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam, int typeDialog);
BOOL CALLBACK AboutUpdateUserDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// ==============  STUDENTS MANAGER  ============== //
extern Student* selectedStudent;
BOOL CALLBACK DB_ManagerStudents_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
char* generateStrSt(const MYSQL_ROW row);
void* setDataSt(const MYSQL_ROW row);
void updateListStudents(HWND hwnd);
int addStudent(HWND hwnd);
int updateStudent(HWND hwnd);
int deleteStudent(HWND hwnd);

// ==============  STUDENTS MANAGER  ============== //
BOOL CALLBACK DB_ManagerGroups_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
char* generateStrGr(const MYSQL_ROW row);
void* setDataGr(const MYSQL_ROW row);
void updateListGroups(HWND hwnd);
int addGroup(HWND hwnd);
int deleteGroup(HWND hwnd);

// ==============  SUBJECTS MANAGER  ============== //
BOOL CALLBACK DB_ManagerSubjects_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
char* generateStrSbj(const MYSQL_ROW row);
void* setDataSbj(const MYSQL_ROW row);
void updateListSubjects(HWND hwnd);
int addSubject(HWND hwnd);
int deleteSubject(HWND hwnd);

// ==============  KHOLLEURS MANAGER  ============== //
BOOL CALLBACK DB_ManagerKholleurs_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
char* generateStrTch(const MYSQL_ROW row);
void* setDataTch(const MYSQL_ROW row);
void updateListTeachers(HWND hwnd);
char* generateStrKll(const MYSQL_ROW row);
void* setDataKll(const MYSQL_ROW row);
void updateListKholleurs(HWND hwnd);
int addTeacher(HWND hwnd);
int addKholleur(HWND hwnd);
extern Teacher* selectedTeacher;
int updateTeacher(HWND hwnd);
BOOL CALLBACK AboutUpdateTeacherDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern Kholleur* selectedKholleur;
int updateKholleur(HWND hwnd);
BOOL CALLBACK AboutUpdateKholleurDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
