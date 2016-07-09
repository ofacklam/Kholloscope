#include "manager2.h"

// ==============  GENERAL MANAGER  ============== //
char* GetItemText(HWND hwnd, int idText) {
    int len = GetWindowTextLength(GetDlgItem(hwnd, idText));
    char* text;
    text = (char*)GlobalAlloc(GPTR, len + 1);   GetDlgItemText(hwnd, idText, text, len + 1);
    return text;
}

void updateList(HWND hwnd, const char* request, char* generateStr(const MYSQL_ROW), void* setData(const MYSQL_ROW)) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST);
    unsigned int i = 0;

    // Clear the list
    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    // Make the request
    mysql_query(mysql, request);
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    result = mysql_store_result(mysql);

    // Treat the results
    for(i=0; (row = mysql_fetch_row(result)); i++) {
        int pos = SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM) generateStr(row));
        SendMessage(hList, LB_SETITEMDATA, pos, (LPARAM) setData(row));
    }
    mysql_free_result(result);
}

// ==============  STUDENTS MANAGER  ============== //
Student* selectedStudent = NULL;

BOOL CALLBACK DB_ManagerStudents_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_INITDIALOG:
            updateListStudents(hwnd);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_ADD:
                    addStudent(hwnd);
                    break;
                case ID_UPDATE:
                    updateStudent(hwnd);
                    break;
                case ID_DELETE:
                    deleteStudent(hwnd);
                    break;
                case IDC_LIST:
                    // It's our listbox, check the notification code
                    switch(HIWORD(wParam)) {
                        case LBN_DBLCLK:
                            updateStudent(hwnd);
                            break;
                    }
                    break;
                case ID_CLOSE:
                    EndDialog(hwnd, 0);
                    break;
            }
            break;
        case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
        default:
            return FALSE;
    }
    return TRUE;
}

char* generateStrSt(const MYSQL_ROW row) {
    return strMultiCat(200, 5, row[1], ", ", row[2], ", ", row[3]);
}

void* setDataSt(const MYSQL_ROW row) {
    Student* data = (Student*) malloc(sizeof(Student));
    data->id = atoi(row[0]);
    data->name = (char*) row[1];
    data->first_name = (char*) row[2];
    data->email = (char*) row[3];
    return (void*) data;
}

void updateListStudents(HWND hwnd) {
    updateList(hwnd, "SELECT id, name, first_name, email FROM tau_users ORDER BY name, first_name", generateStrSt, setDataSt);
}

int addStudent(HWND hwnd) {
    int lenName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_NAME));
    int lenFName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_FNAME));

    if(lenName <=0 || lenFName <=0) {
        MessageBox(NULL, "Il faut renseigner le nom et le prénom.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        char* name = GetItemText(hwnd, IDC_TEXT_NAME);
        char* fName = GetItemText(hwnd, IDC_TEXT_FNAME);
        char* email = GetItemText(hwnd, IDC_TEXT_EMAIL);

        mysql_query(mysql,
            strMultiCat(500, 7, "INSERT INTO tau_users(is_account, name, first_name, email) VALUES(1, '", name, "', '", fName, "', '", email, "')") );

        GlobalFree((HANDLE)name);
        GlobalFree((HANDLE)fName);
        GlobalFree((HANDLE)email);

        SetDlgItemText(hwnd, IDC_TEXT_NAME, "");
        SetDlgItemText(hwnd, IDC_TEXT_FNAME, "");
        SetDlgItemText(hwnd, IDC_TEXT_EMAIL, "");

        updateListStudents(hwnd);
    }
    return 0;
}

int updateStudent(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST);
    int curselList = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if(curselList < 0) {
        MessageBox(NULL, "Aucun élément n'est sélectionné.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        Student* stdnt = (Student*) SendMessage(hList, LB_GETITEMDATA, curselList, 0);
        selectedStudent = stdnt;
        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_USER), hwnd, AboutUpdateUserDlgProc);
        selectedStudent = NULL;
        updateListStudents(hwnd);
    }
    return 0;
}

int deleteStudent(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST);
    int curselList = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if(curselList < 0) {
        MessageBox(NULL, "Aucun élément n'est sélectionné.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        Student* stdnt = (Student*) SendMessage(hList, LB_GETITEMDATA, curselList, 0);
        char* alert = strMultiCat(200, 5, "Vous êtes sur le point de supprimer ", stdnt->first_name, " ", stdnt->name, ". Voulez-vous continuer ?");
        int res = MessageBox(NULL, alert, "En cours de suppression", MB_ICONWARNING | MB_YESNO);
        free(alert);
        if(res == IDYES) {
            char* request = strMultiCat(100, 2, "DELETE FROM tau_users WHERE id=", itoa10(stdnt->id, 20));
            mysql_query(mysql, request);

            updateListStudents(hwnd);
        }
    }
    return 0;
}

BOOL CALLBACK AboutUpdateUserDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_INITDIALOG:
        {
            SetDlgItemText(hwnd, IDC_TEXT_NAME, selectedStudent->name);
            SetDlgItemText(hwnd, IDC_TEXT_FNAME, selectedStudent->first_name);
            SetDlgItemText(hwnd, IDC_TEXT_EMAIL, selectedStudent->email);
            return TRUE;
        }
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_UPDATE:
                    int lenName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_NAME));
                    int lenFName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_FNAME));

                    if(lenName <=0 || lenFName <=0) {
                        MessageBox(NULL, "Il faut renseigner le nom et le prénom.", "Erreur", MB_OK | MB_ICONERROR);
                    } else {
                        char* name = GetItemText(hwnd, IDC_TEXT_NAME);
                        char* fName = GetItemText(hwnd, IDC_TEXT_FNAME);
                        char* email = GetItemText(hwnd, IDC_TEXT_EMAIL);

                        mysql_query(mysql,
                                strMultiCat(500, 8, "UPDATE tau_users SET name='", name, "', first_name='", fName, "', email='", email, "' WHERE id=", itoa10(selectedStudent->id, 20)));

                        GlobalFree((HANDLE)name);
                        GlobalFree((HANDLE)fName);
                        GlobalFree((HANDLE)email);

                        EndDialog(hwnd, 0);
                    }
                    break;
            }
            break;
        case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
        default:
            return FALSE;
    }
    return TRUE;
}


// ==============  GROUPS MANAGER  ============== //
BOOL CALLBACK DB_ManagerGroups_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_INITDIALOG:
            updateListGroups(hwnd);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_ADD:
                    addGroup(hwnd);
                    break;
                case ID_DELETE:
                    deleteGroup(hwnd);
                    break;
                case ID_CLOSE:
                    EndDialog(hwnd, 0);
                    break;
            }
            break;
        case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
        default:
            return FALSE;
    }
    return TRUE;
}

char* generateStrGr(const MYSQL_ROW row) {
    return row[1];
}

void* setDataGr(const MYSQL_ROW row) {
    Group* data = (Group*) malloc(sizeof(Group));
    data->id = atoi(row[0]);
    data->name = (char*) row[1];
    return (void*) data;
}

void updateListGroups(HWND hwnd) {
    updateList(hwnd, "SELECT id, name FROM tau_groups WHERE is_deleted=0 ORDER BY name", generateStrGr, setDataGr);
}

int addGroup(HWND hwnd) {
    int lenName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_NAME));

    if(lenName <=0) {
        MessageBox(NULL, "Il faut renseigner le nom du groupe.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        char* name = GetItemText(hwnd, IDC_TEXT_NAME);

        mysql_query(mysql,
            strMultiCat(500, 3, "INSERT INTO tau_groups(name, is_deleted) VALUES('", name, "', 0)") );

        GlobalFree((HANDLE)name);

        SetDlgItemText(hwnd, IDC_TEXT_NAME, "");

        updateListGroups(hwnd);
    }
    return 0;
}

int deleteGroup(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST);
    int curselList = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if(curselList < 0) {
        MessageBox(NULL, "Aucun élément n'est sélectionné.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        Group* grp = (Group*) SendMessage(hList, LB_GETITEMDATA, curselList, 0);
        char* alert = strMultiCat(200, 3, "Vous êtes sur le point de supprimer le groupe '", grp->name, "'. Voulez-vous continuer ?");
        int res = MessageBox(NULL, alert, "En cours de suppression", MB_ICONWARNING | MB_YESNO);
        free(alert);
        if(res == IDYES) {
            char* request = strMultiCat(100, 2, "DELETE FROM tau_groups WHERE id=", itoa10(grp->id, 20));
            mysql_query(mysql, request);

            updateListGroups(hwnd);
        }
    }
    return 0;
}

// ==============  SUBJECTS MANAGER  ============== //
BOOL CALLBACK DB_ManagerSubjects_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_INITDIALOG:
            updateListSubjects(hwnd);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_ADD:
                    addSubject(hwnd);
                    break;
                case ID_DELETE:
                    deleteSubject(hwnd);
                    break;
                case ID_CLOSE:
                    EndDialog(hwnd, 0);
                    break;
            }
            break;
        case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
        default:
            return FALSE;
    }
    return TRUE;
}

char* generateStrSbj(const MYSQL_ROW row) {
    return strMultiCat(200, 5, row[1], " (", row[2], ") : ", row[3]);
}

void* setDataSbj(const MYSQL_ROW row) {
    Subject* data = (Subject*) malloc(sizeof(Subject));
    data->id = atoi(row[0]);
    data->name = (char*) row[1];
    data->shortName = (char*) row[2];
    data->color = (char*) row[3];
    return (void*) data;
}

void updateListSubjects(HWND hwnd) {
    updateList(hwnd, "SELECT id, name, shortName, color FROM tau_subjects ORDER BY name, shortName", generateStrSbj, setDataSbj);
}

int addSubject(HWND hwnd) {
    int lenName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_NAME));
    int lenShName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_SHNAME));
    int lenColor = GetWindowTextLength(GetDlgItem(hwnd, IDC_COLOR));

    if(lenName <=0 || lenShName <=0 || lenColor <=0) {
        MessageBox(NULL, "Il faut renseigner le nom (long et court) ainsi que la couleur.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        char* name = GetItemText(hwnd, IDC_TEXT_NAME);
        char* shName = GetItemText(hwnd, IDC_TEXT_SHNAME);
        char* color = GetItemText(hwnd, IDC_COLOR);

        mysql_query(mysql,
            strMultiCat(500, 7, "INSERT INTO tau_subjects(name, shortName, color) VALUES('", name, "', '", shName, "', '", color, "')") );

        GlobalFree((HANDLE)name);
        GlobalFree((HANDLE)shName);
        GlobalFree((HANDLE)color);

        SetDlgItemText(hwnd, IDC_TEXT_NAME, "");
        SetDlgItemText(hwnd, IDC_TEXT_SHNAME, "");
        SetDlgItemText(hwnd, IDC_COLOR, "");

        updateListSubjects(hwnd);
    }
    return 0;
}

int deleteSubject(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST);
    int curselList = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if(curselList < 0) {
        MessageBox(NULL, "Aucun élément n'est sélectionné.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        Subject* sbj = (Subject*) SendMessage(hList, LB_GETITEMDATA, curselList, 0);
        char* alert = strMultiCat(200, 3, "Vous êtes sur le point de supprimer la matière '", sbj->name, "'. Voulez-vous continuer ?");
        int res = MessageBox(NULL, alert, "En cours de suppression", MB_ICONWARNING | MB_YESNO);
        free(alert);
        if(res == IDYES) {
            char* request = strMultiCat(100, 2, "DELETE FROM tau_subjects WHERE id=", itoa10(sbj->id, 20));
            mysql_query(mysql, request);

            updateListSubjects(hwnd);
        }
    }
    return 0;
}

// ==============  KHOLLEURS MANAGER  ============== //
Teacher* selectedTeacher = NULL;
Kholleur* selectedKholleur = NULL;

BOOL CALLBACK DB_ManagerKholleurs_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_INITDIALOG:
            updateListTeachers(hwnd);
            updateListKholleurs(hwnd);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_ADD_TEACHER:
                    addTeacher(hwnd);
                    break;
                case ID_ADD_KHOLLEUR:
                    addKholleur(hwnd);
                    break;
                case ID_DELETE:
                    //deleteGroup(hwnd);
                    break;
                case ID_CLOSE:
                    //EndDialog(hwnd, 0);
                    break;
                case IDC_LIST_TEACHERS:
                    // It's our listbox, check the notification code
                    switch(HIWORD(wParam)) {
                        case LBN_DBLCLK:
                            updateTeacher(hwnd);
                            break;
                    }
                    break;
                case IDC_LIST_KHOLLEURS:
                    // It's our listbox, check the notification code
                    switch(HIWORD(wParam)) {
                        case LBN_DBLCLK:
                            updateKholleur(hwnd);
                            break;
                    }
                    break;
            }
            break;
        case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
        default:
            return FALSE;
    }
    return TRUE;
}

char* generateStrTch(const MYSQL_ROW row) {
    return strMultiCat(200, 4, row[1], " (", row[2], ")");
}

void* setDataTch(const MYSQL_ROW row) {
    Teacher* data = (Teacher*) malloc(sizeof(Teacher));
    data->id = atoi(row[0]);
    data->name = (char*) row[1];
    data->id_subjects = atoi(row[2]);
    return (void*) data;
}

void updateListTeachers(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST_TEACHERS);
    unsigned int i = 0;

    // Clear the list
    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    // Make the request
    mysql_query(mysql, "SELECT id, name, id_subjects FROM tau_teachers ORDER BY name");
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    result = mysql_store_result(mysql);

    // Treat the results
    for(i=0; (row = mysql_fetch_row(result)); i++) {
        int pos = SendDlgItemMessage(hwnd, IDC_LIST_TEACHERS, LB_ADDSTRING, 0, (LPARAM) generateStrTch(row));
        SendMessage(hList, LB_SETITEMDATA, pos, (LPARAM) setDataTch(row));
        //MessageBox(NULL, ((Kholleur*) SendMessage(hList, LB_GETITEMDATA, pos, 0))->name, "INFO2", MB_OK);
    }
    mysql_free_result(result);
}

int addTeacher(HWND hwnd) {
    int lenName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_NAME_TEACHER));

    if(lenName <=0) {
        MessageBox(NULL, "Il faut renseigner le nom de l'enseignant.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        char* name = GetItemText(hwnd, IDC_TEXT_NAME_TEACHER);

        mysql_query(mysql,
            strMultiCat(500, 3, "INSERT INTO tau_teachers(name, id_subjects) VALUES('", name, "', 0)") );

        GlobalFree((HANDLE)name);

        SetDlgItemText(hwnd, IDC_TEXT_NAME_TEACHER, "");

        updateListTeachers(hwnd);
    }
    return 0;
}

char* generateStrKll(const MYSQL_ROW row) {
    return strMultiCat(200, 9, row[1], " (", row[2], ") : ", row[3], ", ", row[4], ", ", row[5]);
}

void* setDataKll(const MYSQL_ROW row) {
    Kholleur* data = (Kholleur*) malloc(sizeof(Kholleur));
    data->id = atoi(row[0]);
    data->name = (char*) row[1];
    data->id_subjects = atoi(row[2]);
    data->duration = atoi(row[3]);
    data->preparation = atoi(row[4]);
    data->pupils = atoi(row[5]);
    return (void*) data;
}

void updateListKholleurs(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST_KHOLLEURS);
    unsigned int i = 0;

    // Clear the list
    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    // Make the request
    mysql_query(mysql, "SELECT id, name, id_subjects, duration, preparation, pupils FROM tau_kholleurs ORDER BY name");
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    result = mysql_store_result(mysql);

    // Treat the results
    for(i=0; (row = mysql_fetch_row(result)); i++) {
        int pos = SendDlgItemMessage(hwnd, IDC_LIST_KHOLLEURS, LB_ADDSTRING, 0, (LPARAM) generateStrKll(row));
        SendMessage(hList, LB_SETITEMDATA, pos, (LPARAM) setDataKll(row));
        //MessageBox(NULL, ((Teacher*) SendMessage(hList, LB_GETITEMDATA, pos, 0))->name, "INFO1", MB_OK);
    }
    mysql_free_result(result);
}

int addKholleur(HWND hwnd) {
    int lenName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_NAME_KHOLLEUR));

    if(lenName <=0) {
        MessageBox(NULL, "Il faut renseigner le nom du kholleur.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        char* name = GetItemText(hwnd, IDC_TEXT_NAME_KHOLLEUR);

        mysql_query(mysql,
            strMultiCat(500, 3, "INSERT INTO tau_kholleurs(name, id_subjects, duration, preparation, pupils) VALUES('", name, "', 0, 0, 0, 0)") );

        GlobalFree((HANDLE)name);

        SetDlgItemText(hwnd, IDC_TEXT_NAME_KHOLLEUR, "");

        updateListKholleurs(hwnd);
    }
    return 0;
}

int updateTeacher(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST_TEACHERS);
    int curselList = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if(curselList < 0) {
        MessageBox(NULL, "Aucun élément n'est sélectionné.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        Teacher* tcher = (Teacher*) SendMessage(hList, LB_GETITEMDATA, curselList, 0);
        selectedTeacher = tcher;
        MessageBox(NULL, selectedTeacher->name, "INFO3", MB_OK);
        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_TEACHER), hwnd, AboutUpdateTeacherDlgProc);
        selectedTeacher = NULL;
        updateListTeachers(hwnd);
    }
    return 0;
}

BOOL CALLBACK AboutUpdateTeacherDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_INITDIALOG:
        {
            SetDlgItemText(hwnd, IDC_TEXT_NAME, selectedTeacher->name);
            return TRUE;
        }
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_UPDATE:
                    EndDialog(hwnd, 0);
                    break;
                    int lenName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_NAME));

                    if(lenName <=0) {
                        MessageBox(NULL, "Il faut renseigner le nom et le prénom.", "Erreur", MB_OK | MB_ICONERROR);
                    } else {
                        char* name = GetItemText(hwnd, IDC_TEXT_NAME);

                        char request[500] = "";
                        strcat(request, "UPDATE tau_users SET name='");
                        strcat(request, name);
                        strcat(request, "' WHERE id=");
                        char strID[20] = "";
                        _itoa(selectedStudent->id, strID, 10);
                        strcat(request, strID);
                        mysql_query(mysql, request);

                        GlobalFree((HANDLE)name);

                        EndDialog(hwnd, 0);
                    }
                    break;
            }
            break;
        case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
        default:
            return FALSE;
    }
    return TRUE;
}

int updateKholleur(HWND hwnd) {
    HWND hList = GetDlgItem(hwnd, IDC_LIST_KHOLLEURS);
    int curselList = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if(curselList < 0) {
        MessageBox(NULL, "Aucun élément n'est sélectionné.", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    } else {
        Kholleur* kll = (Kholleur*) SendMessage(hList, LB_GETITEMDATA, curselList, 0);
        selectedKholleur = kll;
        MessageBox(NULL, selectedKholleur->name, "INFO4", MB_OK);
        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_KHOLLEUR), hwnd, AboutUpdateKholleurDlgProc);
        selectedKholleur = NULL;
        updateListKholleurs(hwnd);
    }
    return 0;
}

BOOL CALLBACK AboutUpdateKholleurDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_INITDIALOG:
        {
            SetDlgItemText(hwnd, IDC_TEXT_NAME, selectedKholleur->name);
            return TRUE;
        }
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_UPDATE:
                    EndDialog(hwnd, 0);
                    break;
                    int lenName = GetWindowTextLength(GetDlgItem(hwnd, IDC_TEXT_NAME));

                    if(lenName <=0) {
                        MessageBox(NULL, "Il faut renseigner le nom et le prénom.", "Erreur", MB_OK | MB_ICONERROR);
                    } else {
                        char* name = GetItemText(hwnd, IDC_TEXT_NAME);

                        char request[500] = "";
                        strcat(request, "UPDATE tau_users SET name='");
                        strcat(request, name);
                        strcat(request, "' WHERE id=");
                        char strID[20] = "";
                        _itoa(selectedStudent->id, strID, 10);
                        strcat(request, strID);
                        mysql_query(mysql, request);

                        GlobalFree((HANDLE)name);

                        EndDialog(hwnd, 0);
                    }
                    break;
            }
            break;
        case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;
        default:
            return FALSE;
    }
    return TRUE;
}
