#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define ADMINUSER "ADMIN"
#define STUDENTUSER "STUDENT"

typedef struct{
    char userType[8];
    char email[40];
    char password[30];

    char name[30];
    char NIM[12];
}Account;

//Double Linked List
typedef struct Book{
    char title[40];
    char author[30];
    char publisher[30];
    char ref_number[25];
    int release;

    int stock;
    int avaiable;

    struct Book *prev, *next;
}Book;

//date
typedef struct{
    int day;
    int month;
    int year;
}Date;

//Queue
typedef struct BorrowList{
    char name[30];
    char NIM[12];

    char title[40];
    char ref_number[25];

    char status[20];
    Date borrow, due;
    struct BorrowList *next;
}BorrowList;

//========================================================================================================
//                                          Time Management

//Buat edit tanggal sekarang (debugging due date);
void getDate(Date *date){
    FILE *fp = fopen("Time.txt","r");
    if (fp == NULL){
        time_t rawtime;
        time (&rawtime);
        struct tm* current_time = localtime(&rawtime);
        date->day = current_time->tm_mday ;
        date->month = current_time->tm_mon+1;
        date->year = current_time->tm_year + 1900;
        fp = fopen("Time.txt", "w");
        fprintf(fp,"%d/%d/%d", date->day, date->month, date->year);
        fclose(fp);
    }
    else{
        fscanf(fp,"%d/%d/%d",&date->day, &date->month, &date->year);
        fclose(fp);
    }
}

double check_date(Date date, Date newDate){
    struct tm oldTime, newTime;
    oldTime.tm_mday = date.day;
    oldTime.tm_mon = date.month-1;
    oldTime.tm_year = date.year - 1900;
    oldTime.tm_hour = 0;
    oldTime.tm_min = 0;
    oldTime.tm_sec = 1;


    newTime.tm_mday = newDate.day;
    newTime.tm_mon = newDate.month-1;
    newTime.tm_year = newDate.year - 1900;
    newTime.tm_hour = 0;
    newTime.tm_min = 0;
    newTime.tm_sec = 1;

    double difference = difftime(mktime(&newTime), mktime(&oldTime));
    //printf("%.2f",difference);
    return difference;
}

void setDate(Date *date){
    int flag = 0;
    Date newDate;
    printf("Date now : %d/%d/%d\n",date->day,date->month,date->year);
    printf("Change to: ");
    scanf("%d/%d/%d",&newDate.day,&newDate.month,&newDate.year);

    if ((newDate.day <= 0) || (newDate.month <= 0)){
        flag++;
    }

    if (newDate.month > 12){
        flag++;
    }
    //Cek tgl yg 31
    else if ((newDate.month == 1) || (newDate.month == 3) || (newDate.month == 5) || (newDate.month == 7) || (newDate.month == 8) || (newDate.month == 10) || (newDate.month == 12)){
        if (newDate.day > 31)
            flag++;
    }

    //Cek tgl yg 28-30
    else{
        //Kabisat = 29
        if (((newDate.year % 4) == 0) && (newDate.month == 2) && (newDate.day > 29)){
            flag++;
        }

        //Bukan kabisat
        else if (((newDate.year % 4) != 0) && (newDate.day > 28)){
            flag++;
        }
        else if (newDate.day > 30){
            flag++;
        }
    }

    if (flag > 0){
        printf("Invalid time!\n");
        return;
    }

    if (check_date(*date, newDate) > 0){

        date->day = newDate.day;
        date->month = newDate.month;
        date->year = newDate.year;

        FILE *fp = fopen("Time.txt", "w");
        fprintf(fp,"%d/%d/%d", date->day, date->month, date->year);
        fclose(fp);
        printf("Time changed!");
    }
    else{
        printf("You can't turn back time!\n");
    }
}

Date addTime(Date date, int days){
    int result = date.day + days;
    if ((date.month == 1) || (date.month == 3) || (date.month == 5) || (date.month == 7) || (date.month == 8) || (date.month == 10) || (date.month == 12)){
        if (result > 31){
            date.day = result % 31;
            date.month += 1;
        }
        else{
            ;
            date.day = result;
        }
    }
    else{
        //Kabisat = 29
        if (((date.year % 4) == 0) && (date.month == 2) && (date.day > 29)){
            date.day = result % 29;
            date.month += 1;
        }

        //Bukan kabisat
        else if (((date.year % 4) != 0) && (date.day > 28)){
            date.day = result % 28;
            date.month += 1;
        }
        else if (date.day > 30){
            date.day = result % 30;
            date.month += 1;
        }
        else{
            date.day = result;
        }
    }

    if (date.month > 12){
        date.month = date.month % 12;
        date.year++;
    }

    return date;

}

//========================================================================================================

//========================================================================================================
//                                           Login (beres)

int validate_account_input(char *email_input, char *password_input, Account *account, int arraySize){
    for(int i = 0; i < arraySize; i++){
        if ((strcmp(email_input, account[i].email) == 0) &&(strcmp(password_input, account[i].password) == 0))
            return i;
    }
    return -1;
}

//Use Array of Struct
Account login_page(){
    Account *account = (Account*)malloc(sizeof(Account)), temp;
    int current_size = 1;

    //Fetch account data
    FILE *fp;

    char userType[8];
    char email[40];
    char password[30];

    char name[30];
    char NIM[12];


    if ((fp = fopen("Account.txt","r")) != NULL){

        while(fscanf(fp,"%[^#]#%[^#]#%[^#]#%[^#]#%[^\n]\n",userType,email,password,name,NIM) != EOF){
            account  = (Account*)realloc(account, (current_size * sizeof(Account)));
            strcpy(account[current_size-1].userType,userType);
            strcpy(account[current_size-1].email,email);
            strcpy(account[current_size-1].password,password);
            strcpy(account[current_size-1].name,name);
            strcpy(account[current_size-1].NIM,NIM);
            printf("%s %s\n",account[current_size-1].email, account[current_size-1].password);
            current_size++;
        }
        current_size--;
        fclose(fp);
    }
    else{
        printf("Error: Account data file not found\n");
        exit(1);
    }

    char email_input[40];
    char password_input[30];

    while(1){
        fflush(stdin);
        printf("     Halaman Sign in     \n");
        printf("=========================\n");
        printf("Masukkan data\n");
        printf("Email: ");
        scanf("%[^\n]",email_input);
        fflush(stdin);
        printf("Password: ");
        scanf("%[^\n]",password_input);
        fflush(stdin);

        int index = validate_account_input(email_input,password_input, account, current_size);
        if (index >= 0){
            printf("login successfully, navigating to %s main page\n",(strcmp(account[index].userType, ADMINUSER) == 0)? ADMINUSER : STUDENTUSER);
            temp = account[index];
            free(account);
            return temp;
        }
        else{
            printf("Wrong email/password, try again\n");
        }

    }

}

//========================================================================================================

//========================================================================================================
//                                          Book Management

void showAllBook(Book *head){
    Book *temp;
    temp = head;

    printf("=======================================\n");
    while(temp != NULL){
        printf("Title       : %s\n",temp->title);
        printf("Author      : %s\n",temp->author);
        printf("Publisher   : %s\n",temp->publisher);
        printf("Ref-number  : %s\n",temp->ref_number);
        printf("Release     : %d\n",temp->release);
        printf("Stock       : %d\n",temp->stock);
        printf("Avaiability : %s\n\n",temp->avaiable >= 1 ? "Avaiable" : "Not Avaiable");
        temp = temp->next;
    }
}

void *addNewBook(char *title, char *author, char *publisher, char *ref_number, int release, int stock, int avaiable, Book **head, Book **tail){
    Book *newNode = (Book*) malloc(sizeof(Book));
    strcpy(newNode->title, title);
    strcpy(newNode->author, author);
    strcpy(newNode->publisher, publisher);
    strcpy(newNode->ref_number, ref_number);
    newNode->release = release;
    newNode->stock = stock;
    newNode->avaiable = avaiable;
    if (*head == NULL){
        *head = newNode;
        (*head)->prev = NULL;
        *tail = *head;
    }
    else{
        (*tail)->next = newNode;
        newNode->prev = *tail;
        *tail = (*tail)->next;
    }
    (*tail)->next = NULL;
}

void fetchBookData(Book **head, Book **tail){
    FILE *fp = fopen("Book data.txt", "r");

    char title[40];
    char author[30];
    char publisher[30];
    char ref_number[25];
    int release;

    int stock;
    int avaiable;

    if (fp != NULL){
        while(fscanf(fp,"%[^#]#%[^#]#%[^#]#%[^#]#%d#%d#%d\n",title,author,publisher,ref_number,&release,&stock,&avaiable) != EOF){
            addNewBook(title,author,publisher,ref_number,release,stock,avaiable,head,tail);
        }
        fclose(fp);
    }
    else{
        return;
    }
}

void searchBook_byTitle(Book *head, char *title_input){
    int found = 0;
    Book *node = head;

    while(node != NULL){
        if (strcmp(node->title,title_input) == 0){
            found++;
            if (found == 1){
                printf("Book title found!\n");
            }
            printf("Title       : %s\n",node->title);
            printf("Author      : %s\n",node->author);
            printf("Publisher   : %s\n",node->publisher);
            printf("Ref-number  : %s\n",node->ref_number);
            printf("Release     : %d\n",node->release);
            printf("Stock       : %d\n",node->stock);
            printf("Avaiability : %s\n\n",node->avaiable >= 1 ? "Avaiable" : "Not Avaiable");
        }
        node = node->next;
    }
    if (found == 0){
        printf("Book title not found!\n");
    }
}

void searchBookMenu(Book *head){
    char title_input[40];
    printf("     Search Book     \n");
    printf("=====================\n");
    fflush(stdin);
    printf("Title: ");
    scanf("%[^\n]",&title_input);
    fflush(stdin);
    searchBook_byTitle(head, title_input);
}

//========================================================================================================

//========================================================================================================
//                                      Book-Borrowing Management

int isEmpty(BorrowList *Queue){
    if (Queue == NULL) return 1;
    return 0;
}

void enqueueBorrowData(BorrowList **front, BorrowList **rear, char *name, char* NIM, char* title, char* ref_number, char* status, Date borrow, Date due){
    BorrowList *data = (BorrowList*) malloc(sizeof(BorrowList));
    strcpy(data->name, name);
    strcpy(data->NIM, NIM);
    strcpy(data->title, title);
    strcpy(data->ref_number, ref_number);
    strcpy(data->status, status);

    data->borrow.day = borrow.day;
    data->borrow.month = borrow.month;
    data->borrow.year = borrow.year;

    data->due.day = due.day;
    data->due.month = due.month;
    data->due.year = due.year;

    data->next = NULL;
    if (isEmpty(*front)) *front = data;
    else (*rear)->next = data;
    *rear = data;
    //printf("Adding %s to queue\n", data->ref_number);

}

void dequeueBorrowData(BorrowList **front){
    if (isEmpty(*front)){
        printf("Dequeue error: Queue is empty\n");
        return;
    }
    BorrowList *trash = *front;
    *front = (*front)->next;
    //printf("Removing %s in queue\n", trash->ref_number);
    free(trash);
}

void fetchBorrowData(BorrowList **front, BorrowList **rear, Account account){
    char name[30];
    char NIM[12];

    char title[40];
    char ref_number[25];

    char status[20];
    Date borrow, due;

    FILE *fp;
    if (strcmp(account.userType, ADMINUSER) == 0){
        fp = fopen("Borrowing data.txt", "r");
    }
    else{
        char filename[25];
        strcpy(filename, account.NIM);
        strcat(filename, "-History.txt");
        fp = fopen(filename, "r");
    }
    if (fp != NULL){
        while (fscanf(fp,"%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%d/%d/%d#%d/%d/%d\n", name, NIM, title, ref_number, status, &borrow.day, &borrow.month, &borrow.year, &due.day, &due.month, &due.year) != EOF){
            if (strcmp(account.userType, ADMINUSER) == 0){//Admin
                enqueueBorrowData(front, rear, name, NIM, title, ref_number, status, borrow, due);
            }
            else{//Student
                if (strcmp(account.name, name) == 0){
                    enqueueBorrowData(front, rear, name, NIM, title, ref_number, status, borrow, due);
                }
            }
        }
        fclose(fp);
    }
}

void copy_front_queue(BorrowList *origin, BorrowList **frontDestination, BorrowList **rearDestination){
    enqueueBorrowData(frontDestination, rearDestination, origin->name, origin->NIM, origin->title, origin->ref_number, origin->status, origin->borrow, origin->due);
}

int check_duplicate_borrow(BorrowList **front_borrow,BorrowList **rear_borrow, char *bookRef){

    //Buat 2 Clone queue, 1 untuk selector, 1 lagi untuk balikin queue ke semula lagi

    BorrowList *front_borrow_clone, *rear_borrow_clone;
    BorrowList *front_selector, *rear_selector;
    front_selector = rear_selector = NULL;
    front_borrow_clone = rear_borrow_clone = NULL;
    int flag = 0;

    //Clone queue
    while(!isEmpty(*front_borrow)){
        copy_front_queue(*front_borrow, &front_selector, &rear_selector);
        copy_front_queue(*front_borrow, &front_borrow_clone, &rear_borrow_clone);
        dequeueBorrowData(front_borrow);

    }
    while(!isEmpty(front_selector)){ //jumlah data front_selector == front_borrow_clone
        if (strcmp(front_selector->ref_number, bookRef) == 0){
            flag++;
        }
        dequeueBorrowData(&front_selector);

        //Balikin queue ke semula
        copy_front_queue(front_borrow_clone, front_borrow, rear_borrow);
        dequeueBorrowData(&front_borrow_clone);
    }

    return flag;
}

void returnBook(Book **head, char* ref_number){
    int found = 0;
    Book *node = *head;

    FILE *fp = fopen("Book data.txt", "w");

    while(node != NULL){
        if (strcmp(node->ref_number, ref_number) == 0){
            node->avaiable += 1;
        }
        fprintf(fp,"%s#%s#%s#%s#%d#%d#%d\n",node->title,node->author,node->publisher,node->ref_number,node->release,node->stock,node->avaiable);
        node = node->next;
    }
    fclose(fp);
}

void updateBorrowFile(BorrowList **front_borrow, BorrowList **rear_borrow){
    FILE *fp;
    fp = fopen("Borrowing data.txt", "w");
    BorrowList *front_selector, *rear_selector;

    front_selector = rear_selector = NULL;

    while(!isEmpty(*front_borrow)){
        copy_front_queue(*front_borrow, &front_selector, &rear_selector);
        dequeueBorrowData(front_borrow);
    }

    while(!isEmpty(front_selector)){
        fprintf(fp, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", front_selector->name, front_selector->NIM, front_selector->title, front_selector->ref_number, front_selector->status, front_selector->borrow.day, front_selector->borrow.month, front_selector->borrow.year, front_selector->due.day, front_selector->due.month, front_selector->due.year);
        copy_front_queue(front_selector, front_borrow, rear_borrow);
        dequeueBorrowData(&front_selector);
        //Balikin queue ke semula
    }
    fclose(fp);
}

void updateBorrowData(Book **head_book, BorrowList **front_borrow, BorrowList **rear_borrow, Account user, Date date){
    //Update for overdue & reservation cancelling
    BorrowList *front_borrow_clone, *rear_borrow_clone;
    BorrowList *front_selector, *rear_selector;

    front_selector = rear_selector = NULL;
    front_borrow_clone = rear_borrow_clone = NULL;
    bool isoverdue = false, iscancelled = false;
    double time_remaining;

    char filename[25];
    if (strcmp(user.userType, STUDENTUSER) == 0){
        strcpy(filename, user.NIM);
        strcat(filename, "-History.txt");
    }
    else{
        strcpy(filename, "History.txt");
    }

    FILE *fp, *fp2;
    fp = fopen(filename, "a");
    if (strcmp(user.userType, STUDENTUSER) == 0){
        fp2 = fopen("History.txt", "a");
    }

    //Clone queue
    while(!isEmpty(*front_borrow)){
        copy_front_queue(*front_borrow, &front_selector, &rear_selector);
        copy_front_queue(*front_borrow, &front_borrow_clone, &rear_borrow_clone);
        dequeueBorrowData(front_borrow);
    }
    while(!isEmpty(front_selector)){
        time_remaining = check_date(date, front_selector->due);
        //Cek apakah pesanan lebih dari 2 hari / overdue
        if (time_remaining <= 0){
            if (strcmp(front_selector->status, "Reserved") == 0){
                iscancelled = true;
                strcpy(front_selector->status, "Cancelled");
                returnBook(head_book, front_selector->ref_number);

                fprintf(fp, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", front_selector->name, front_selector->NIM, front_selector->title, front_selector->ref_number, front_selector->status, front_selector->borrow.day, front_selector->borrow.month, front_selector->borrow.year, front_selector->due.day, front_selector->due.month, front_selector->due.year);
                if (strcmp(user.userType, STUDENTUSER) == 0){
                    fprintf(fp2, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", front_selector->name, front_selector->NIM, front_selector->title, front_selector->ref_number, front_selector->status, front_selector->borrow.day, front_selector->borrow.month, front_selector->borrow.year, front_selector->due.day, front_selector->due.month, front_selector->due.year);
                }
                dequeueBorrowData(&front_selector);
                dequeueBorrowData(&front_borrow_clone);

            }
            else if (strcmp(front_selector->status, "Borrowing") == 0){
                isoverdue = true;
                strcpy(front_borrow_clone->status, "Overdue");
                copy_front_queue(front_borrow_clone, front_borrow, rear_borrow);
                dequeueBorrowData(&front_selector);
                dequeueBorrowData(&front_borrow_clone);
            }
            else{
            copy_front_queue(front_borrow_clone, front_borrow, rear_borrow);
            dequeueBorrowData(&front_selector);
            dequeueBorrowData(&front_borrow_clone);
            }
        }
        else{
            copy_front_queue(front_borrow_clone, front_borrow, rear_borrow);
            dequeueBorrowData(&front_selector);
            dequeueBorrowData(&front_borrow_clone);
        }
        fclose(fp);
        if (strcmp(user.userType, STUDENTUSER) == 0){
            fclose(fp2);
        }
        //Cek apakah pesanan overdue
        if (isoverdue || iscancelled){
            updateBorrowFile(front_borrow, rear_borrow);
        }
    }
}

void borrowBook(Book **head_book,BorrowList **front_borrow, BorrowList **rear_borrow,Date date,Account user){
    char title_input[40], ref_number_input[25];
    int found_title = 0, found_ref = 0;

    showAllBook(*head_book);
    printf("=====================\n");
    printf("     Borrow Book     \n");
    printf("=====================\n");
    fflush(stdin);
    printf("Title: ");
    scanf("%[^\n]",&title_input);
    fflush(stdin);

    //Cek input

    Book *node = *head_book;
    Book *selector_head, *selector_tail, *selected;
    selector_head = selector_tail = selected = NULL;

    while(node != NULL){
        if (strcmp(node->title,title_input) == 0){
            found_title++;
            if (found_title == 1){
                printf("Book title found!\n");
            }

            printf("========================================\n");
            printf("Title       : %s\n",node->title);
            printf("Author      : %s\n",node->author);
            printf("Publisher   : %s\n",node->publisher);
            printf("Ref-number  : %s\n",node->ref_number);
            printf("Release     : %d\n",node->release);
            printf("Stock       : %d\n",node->stock);
            printf("Avaiability : %s\n\n",node->avaiable >= 1 ? "Avaiable" : "Not Avaiable");
            addNewBook(node->title, node->author, node->publisher, node->ref_number, node->release, node->stock, node->avaiable, &selector_head, &selector_tail);
            selected = node;
        }
        node = node->next;
    }
    printf("========================================\n");
    if (found_title == 0){
        printf("Book title not found!\n");
        return;
    }

    else if (found_title > 1){
        printf("More than one book have found, please select one of the ref-number above\n");
        printf("Ref-number: ");
        scanf("%[^\n]", ref_number_input);

        node = selector_head;
        while(node != NULL){
            if (strcmp(node->ref_number,ref_number_input) == 0){
                found_ref = 1;

                Book *temp = *head_book;
                while(temp != NULL){
                    if (strcmp(temp->ref_number, ref_number_input) == 0){
                        selected = temp;
                        break;
                    }

                    temp = temp->next;
                }
                break;
            }
            node = node->next;
        }

        if (found_ref == 0){
            printf("Ref-number not found!");
            return;
        }
    }

    if (found_ref = 1 || found_title == 1){
        if (found_title == 1){
            strcpy(ref_number_input, selected->ref_number);
        }
        if (check_duplicate_borrow(front_borrow, rear_borrow, ref_number_input) == 0){
            //printf("not duplicate\n");
            if (selected->avaiable == 0){
                printf("Book is not avaiable for now\n");
                return;
            }

            selected->avaiable -= 1;
            Date due;
            due = addTime(date, 2);
            enqueueBorrowData(front_borrow, rear_borrow, user.name, user.NIM, selected->title, selected->ref_number, "Reserved", date, due);

            FILE *fp = fopen("Borrowing data.txt", "a");
            fprintf(fp,"%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n",(*rear_borrow)->name, (*rear_borrow)->NIM, (*rear_borrow)->title, (*rear_borrow)->ref_number, (*rear_borrow)->status, (*rear_borrow)->borrow.day, (*rear_borrow)->borrow.month, (*rear_borrow)->borrow.year, (*rear_borrow)->due.day, (*rear_borrow)->due.month, (*rear_borrow)->due.year);
            fclose(fp);

            fp = fopen("Book data.txt", "w");

            node = *head_book;
            while(node != NULL){
                fprintf(fp,"%s#%s#%s#%s#%d#%d#%d\n",node->title,node->author,node->publisher,node->ref_number,node->release,node->stock,node->avaiable);
                node = node->next;
            }
            fclose(fp);

            printf("Book reserved, please pick it in the library in 2 days\n");
        }
        else{
            printf("You can't borrow a same book twice\n");
            return;
        }

    }

}

void displayBorrowData(BorrowList **front_borrow, BorrowList **rear_borrow){

    BorrowList *front_borrow_clone, *rear_borrow_clone;
    BorrowList *front_selector, *rear_selector;
    front_selector = rear_selector = NULL;
    front_borrow_clone = rear_borrow_clone = NULL;
    while(!isEmpty(*front_borrow)){
        copy_front_queue(*front_borrow, &front_selector, &rear_selector);
        copy_front_queue(*front_borrow, &front_borrow_clone, &rear_borrow_clone);
        dequeueBorrowData(front_borrow);
    }

    printf("======================================\n");
    printf("             Borrow List              \n");
    printf("======================================\n\n");
    if (isEmpty(front_selector)){
        printf("List empty\n\n");
        printf("======================================\n");
        return;
    }

    int i = 1;
    while(!isEmpty(front_selector)){
        printf("Loan #%d\n", i);
        i++;
        printf("Title       : %s\n", front_selector->title);
        printf("Ref number  : %s\n", front_selector->ref_number);
        printf("Status      : %s\n", front_selector->status);
        printf("Borrow date : %d/%d/%d\n", front_selector->borrow.day, front_selector->borrow.month, front_selector->borrow.year);
        printf("Due date    : %d/%d/%d\n\n", front_selector->due.day, front_selector->due.month, front_selector->due.year);
        printf("======================================\n");
        dequeueBorrowData(&front_selector);

        copy_front_queue(front_borrow_clone, front_borrow, rear_borrow);
        dequeueBorrowData(&front_borrow_clone);
    }
}

void displayBorrowHistory(Account user){
    FILE *fp = fopen("History.txt", "r");
    BorrowList history;
    printf("======================================\n");
    printf("            Borrow History            \n");
    printf("======================================\n");
    if (fp != NULL){
        while(fscanf(fp, "%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%d/%d/%d#%d/%d/%d\n", history.name, history.NIM, history.title, history.ref_number, history.status, &history.borrow.day, &history.borrow.month, &history.borrow.year, &history.due.day, &history.due.month, &history.due.year) != EOF){
            if ((strcmp(user.userType, ADMINUSER) == 0) || (strcmp(user.NIM, history.NIM) == 0)){
                printf("Borrower       : %s\n", history.name);
                printf("Borrower's NIM : %s\n", history.NIM);
                printf("Title          : %s\n", history.title);
                printf("Ref number     : %s\n", history.ref_number);
                printf("Status         : %s\n", history.status);
                printf("Borrow date    : %d/%d/%d\n", history.borrow.day, history.borrow.month, history.borrow.year);
                printf("Due date       : %d/%d/%d\n\n", history.due.day, history.due.month, history.due.year);
                printf("======================================\n");
            }
        }
        fclose(fp);
    }
    else{
        printf("History is empty!");
    }
}

void returnBookMenu(Book **head_book, BorrowList **front_borrow, BorrowList **rear_borrow, Date date, Account user){
    int choose, flag = 0;
    int counter = 0, current = 1;

    char ref_number[25];

    //BorrowList *front_borrow_clone, *rear_borrow_clone;
    BorrowList *front_selector, *rear_selector, selected;

    front_selector = rear_selector = NULL;
    //front_borrow_clone = rear_borrow_clone = NULL;

    if (!isEmpty(*front_borrow)){
        printf("List is empty");
    }

    displayBorrowData(front_borrow, rear_borrow);

    printf("Choose: ");
    scanf("%d",&choose);
    if ((choose < 1) || (choose > 4)){
        flag++;
    }
    else{
        while(!isEmpty(*front_borrow)){
            copy_front_queue(*front_borrow, &front_selector, &rear_selector);
            dequeueBorrowData(front_borrow);
            counter++;
        }
        if (counter < choose){
            printf("Invalid input");
        }
        else{
            while(!isEmpty(front_selector)){
                if (current == choose){
                    strcpy(ref_number, front_selector->ref_number);
                    strcpy(front_selector->status, "Returning");
                }

                copy_front_queue(front_selector, front_borrow, rear_borrow);
                dequeueBorrowData(&front_selector);
            }
            updateBorrowFile(front_borrow, rear_borrow);
        }
    }
}

//========================================================================================================

void studentPage(Account user){

    //Book
    Book *head_book, *tail_book;
    head_book = tail_book = NULL;

    BorrowList *front_borrow, *rear_borrow;
    front_borrow = rear_borrow = NULL;

    int option;

    //Book
    fetchBookData(&head_book, &tail_book);

    //Time
    Date date;
    getDate(&date);

    //Borrow
    fetchBorrowData(&front_borrow, &rear_borrow, user);

    while(1){

        updateBorrowData(&head_book, &front_borrow, &rear_borrow, user, date);

        fflush(stdin);
        printf("=======================================\n");
        printf("               Main Page               \n");
        printf("=======================================\n");
        printf("Welcome, %s\n",user.name);
        printf("Current time: %d/%d/%d\n",date.day, date.month, date.year);
        printf("[1] Show Books list\n");
        printf("[2] Search book\n");
        printf("[3] Borrow a book\n");
        printf("[4] Return/cancel a book reservation\n");
        printf("[5] Show reservation list\n");
        printf("[6] Show reservation history\n");
        printf("[7] Log out\n");
        printf("[8] Exit\n");
        printf("[0] Change current time (for debugging)\n");
        printf("\nOption: ");
        scanf("%d",&option);
        printf("\n");

        if (option == 0){
            //Function ubah tanggal
            setDate(&date);
        }
        else if (option == 1){
            showAllBook(head_book);
        }
        else if (option == 2){
            searchBookMenu(head_book);
        }
        else if (option == 3){
            borrowBook(&head_book, &front_borrow, &rear_borrow, date, user);
        }
        else if (option == 4){
            returnBookMenu(&head_book, &front_borrow, &rear_borrow, date, user);
        }
        else if (option == 5){
            displayBorrowData(&front_borrow, &rear_borrow);
        }
        else if (option == 6){
            displayBorrowHistory(user);
        }
        else if (option == 7){
            break;
        }
        else if (option == 8){
            exit(0);
        }
        else{
            printf("Invalid input\n");
        }
        printf("\n");
    }
}


int main(){
    int option;
    Account account;

    while(1){
        fflush(stdin);
        printf("      UMN Library      \n");
        printf(" Book-borrowing System \n");
        printf("=======================\n");
        printf("[1] Sign in\n");
        printf("[2] Exit\n\n");
        printf("Option: ");
        scanf("%d",&option);
        if (option == 1){
            account = login_page();
            if (strcmp(account.userType,ADMINUSER) == 0){
                //adminPage(account);
            }
            else if (strcmp(account.userType, STUDENTUSER) == 0){
                studentPage(account);
            }
            else{
                printf("Invalid login, try again\n");
            }
        }

        else if (option == 2)
            break;
        else
            printf("Invalid input\n");

        printf("\n");
    }
    return 0;
}
