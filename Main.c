#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

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

//Ambil waktu di file
void getDate(Date *date){
    FILE *fp = fopen("Time.txt","r");
    if (fp == NULL){ //File ada
        time_t rawtime;
        time (&rawtime); //waktu sekarang
        struct tm* current_time = localtime(&rawtime); //Buat misahin date-month-year
        date->day = current_time->tm_mday ;
        date->month = current_time->tm_mon+1;
        date->year = current_time->tm_year + 1900;
        fp = fopen("Time.txt", "w");
        fprintf(fp,"%d/%d/%d", date->day, date->month, date->year);
        fclose(fp);
    }
    else{ //File ga ada, bikin baru
        fscanf(fp,"%d/%d/%d",&date->day, &date->month, &date->year);
        fclose(fp);
    }
}

//Cek perbedaan waktu
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

    double difference = difftime(mktime(&newTime), mktime(&oldTime)); //difftime buat return selisih waktu
    return difference;
}

//Ganti tanggal (modifier hari)
void setDate(Date *date){
    int flag = 0; //Flag > 0 == inputnya salah
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

    if (check_date(*date, newDate) > 0){ //Cek dulu time nya mundur atau ga
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

//Buat nambah waktu (return waktu yg udh ditambah)
Date addTime(Date date, int days){
    int result = date.day + days;
    if ((date.month == 1) || (date.month == 3) || (date.month == 5) || (date.month == 7) || (date.month == 8) || (date.month == 10) || (date.month == 12)){
        if (result > 31){
            date.day = result % 31;
            date.month += 1;
        }
        else{
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
//                                              Login

//bandingin login input & database nya
int validate_account_input(char *email_input, char *password_input, Account *account, int arraySize){
    for(int i = 0; i < arraySize; i++){
        if ((strcmp(email_input, account[i].email) == 0) &&(strcmp(password_input, account[i].password) == 0))
            return i;
    }
    return -1;
}

//Menu login
Account login_page(){
    Account *account, temp;
    account = NULL;
    int current_size = 1;
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
        exit(1); //account belum di malloc, jadi aman
    }

    char email_input[40];
    char password_input[30];

    while(1){
        fflush(stdin);
        printf("=======================================\n");
        printf("             Halaman Log in            \n");
        printf("=======================================\n");
        printf("Masukkan data\n");
        printf("Email: ");
        scanf("%[^\n]",email_input);
        fflush(stdin);
        printf("Password: ");
        scanf("%[^\n]",password_input);
        fflush(stdin);

        int index = validate_account_input(email_input,password_input, account, current_size);
        if (index >= 0){
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

//Ini function buat cek ada yg borrrow atau engga
int check_duplicate_borrow(BorrowList **front_borrow,BorrowList **rear_borrow, char *bookRef);

void showAllBook(Book *head){
    Book *temp;
    temp = head;

    printf("==============================================================================================================================\n");
    printf("| %-25s | %-15s | %-15s | %-25s | %-7s | %-5s | %-12s |\n", "Title", "Author", "Publisher", "Ref-number", "Release", "Stock", "Avaiable");
    printf("==============================================================================================================================\n");
    while(temp != NULL){
        printf("| %-25s | %-15s | %-15s | %-25s | %-7d | %-5d | %-12s |\n", temp->title, temp->author, temp->publisher, temp->ref_number, temp->release, temp->stock, (temp->avaiable >= 1 ? "Avaiable" : "Not Avaiable"));
        temp = temp->next;
    }
    printf("==============================================================================================================================\n");
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
                printf("Book title found!\n");//Found pertama print ini
            }
            printf("=======================================\n");
            printf("Title       : %s\n",node->title);
            printf("Author      : %s\n",node->author);
            printf("Publisher   : %s\n",node->publisher);
            printf("Ref-number  : %s\n",node->ref_number);
            printf("Release     : %d\n",node->release);
            printf("Stock       : %d\n",node->stock);
            printf("Avaiability : %s\n",node->avaiable >= 1 ? "Avaiable" : "Not Avaiable");
        }
        node = node->next;
    }
    if (found == 0){
        printf("Book title not found!\n");
    }
    printf("=======================================\n");
}

void searchBookMenu(Book *head){
    char title_input[40];

    printf("=======================================\n");
    printf("              Search Book              \n");
    printf("=======================================\n");
    fflush(stdin);
    printf("Title: ");
    scanf("%[^\n]",&title_input);
    fflush(stdin);
    searchBook_byTitle(head, title_input);
}

void updateBookFile(Book *head){
    Book *node = head;
    FILE *fp;
    fp = fopen("Book data.txt", "w");

    while(node != NULL){
        fprintf(fp, "%s#%s#%s#%s#%d#%d#%d\n",node->title, node->author,node->publisher,node->ref_number, node->release, node->stock, node->avaiable);
        node = node->next;
    }
    fclose(fp);
}

void addBookMenu(Book **head, Book **tail){
    Book newBook;

    fflush(stdin);
    printf("=======================================\n");
    printf("                Add Book               \n");
    printf("=======================================\n");
    printf("Title       : ");
    scanf("%[^\n]",newBook.title);
    fflush(stdin);
    printf("Author      : ");
    scanf("%[^\n]",newBook.author);
    fflush(stdin);
    printf("Publisher   : ");
    scanf("%[^\n]",newBook.publisher);
    fflush(stdin);
    printf("Ref-number  : ");
    scanf("%[^\n]",newBook.ref_number);
    fflush(stdin);
    printf("Release year: ");
    scanf("%d", &newBook.release);
    fflush(stdin);
    printf("Stock: ");
    scanf("%d",&newBook.stock);
    newBook.avaiable = newBook.stock;
    fflush(stdin);

    addNewBook(newBook.title, newBook.author, newBook.publisher, newBook.ref_number, newBook.release, newBook.stock, newBook.avaiable, head, tail);
    updateBookFile(*head);
    printf("=======================================\n");
}

int isBorrowed(BorrowList **front_borrow,BorrowList **rear_borrow, char *bookRef){
    return check_duplicate_borrow(front_borrow, rear_borrow, bookRef);
}

void removeBook(Book **head, Book **tail, BorrowList **front_borrow, BorrowList **rear_borrow){
    Book *selected = *head;
    char ref_number_input[25];
    printf("=======================================\n");
    printf("              Remove Book              \n");
    printf("=======================================\n");
    showAllBook(*head);
    printf("Ref-number: ");
    fflush(stdin);
    scanf("%[^\n]",ref_number_input);
    fflush(stdin);

    while(selected != NULL){
        if (strcmp(selected->ref_number, ref_number_input) == 0){
            if (isBorrowed(front_borrow, rear_borrow, ref_number_input) > 0){
                printf("This book is being borrowed, please wait for the book to be returned\n");
                return;
            }

            if(selected == *head){
                *head = (*head)->next;
                (*head)->prev = NULL;
                free(selected);
            }
            else if(selected == *tail){
                *tail = selected->prev;
                (*tail)->next = NULL;
                free(selected);
            }
            else{
                selected->prev->next = selected->next;
                selected->next->prev = selected->prev;
                free(selected);
            }
            updateBookFile(*head);
            printf("Book removed\n");
            printf("=======================================\n");
            return;
        }
        selected = selected->next;
    }

    printf("Book not found\n");
    printf("=======================================\n");
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
}

void dequeueBorrowData(BorrowList **front){
    if (isEmpty(*front)){
        printf("Dequeue error: Queue is empty\n");
        return;
    }
    BorrowList *trash = *front;
    *front = (*front)->next;
    free(trash);
}

void fetchBorrowData(BorrowList **front, BorrowList **rear, Account account){
    char name[30];
    char NIM[12];
    char title[40];
    char ref_number[25];
    char status[20];
    char filename[25];
    Date borrow, due;
    FILE *fp;
    strcpy(filename,account.NIM);
    strcat(filename, "-Borrowing data.txt");
    fp = fopen(filename, "r");
    if (fp != NULL){
        while (fscanf(fp,"%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%d/%d/%d#%d/%d/%d\n", name, NIM, title, ref_number, status, &borrow.day, &borrow.month, &borrow.year, &due.day, &due.month, &due.year) != EOF){
            enqueueBorrowData(front, rear, name, NIM, title, ref_number, status, borrow, due);
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

void updateBorrowFile(BorrowList **front_borrow, BorrowList **rear_borrow, Account user){
    FILE *fp;
    char filename[25];
    strcpy(filename, user.NIM);
    strcat(filename,"-Borrowing data.txt");
    fp = fopen(filename, "w");

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
    //Update for overdue
    BorrowList *front_borrow_clone, *rear_borrow_clone;
    BorrowList *front_selector, *rear_selector;

    front_selector = rear_selector = NULL;
    front_borrow_clone = rear_borrow_clone = NULL;
    bool isoverdue = false;
    double time_remaining;

    //Clone queue
    while(!isEmpty(*front_borrow)){
        copy_front_queue(*front_borrow, &front_selector, &rear_selector);
        copy_front_queue(*front_borrow, &front_borrow_clone, &rear_borrow_clone);
        dequeueBorrowData(front_borrow);
    }
    while(!isEmpty(front_selector)){
        time_remaining = check_date(date, front_selector->due);
        //Cek apakah pinjam lebih dari 7 hari / overdue
        if (time_remaining <= 0 && (strcmp(front_borrow_clone->status, "Borrowing") == 0)){
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
    if (isoverdue){
            updateBorrowFile(front_borrow, rear_borrow, user);
    }
}

int check_full_loan(BorrowList **front_borrow, BorrowList **rear_borrow){
    int counter = 0;
    int flag = 0;

    BorrowList *front_selector, *rear_selector;
    front_selector = rear_selector = NULL;

    while(!isEmpty(*front_borrow)){
        copy_front_queue(*front_borrow, &front_selector, &rear_selector);
        dequeueBorrowData(front_borrow);
        counter++;
    }
    while(!isEmpty(front_selector)){ //jumlah data front_selector == front_borrow_clone
        copy_front_queue(front_selector, front_borrow, rear_borrow);
        dequeueBorrowData(&front_selector);
    }
    return counter;
}

void borrowBook(Book **head_book,BorrowList **front_borrow, BorrowList **rear_borrow,Date date,Account user){
    char title_input[40], ref_number_input[25];
    int found_title = 0, found_ref = 0;

    //Cek Loan, maks 4 sesuai peraturan UMN
    if (check_full_loan(front_borrow, rear_borrow) == 4){
        printf("You can only borrow 4 books at a time!\n");
        return;
    }

    showAllBook(*head_book);
    printf("=======================================\n");
    printf("              Borrow Book              \n");
    printf("=======================================\n");
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

            printf("=======================================\n");
            printf("Title       : %s\n",node->title);
            printf("Author      : %s\n",node->author);
            printf("Publisher   : %s\n",node->publisher);
            printf("Ref-number  : %s\n",node->ref_number);
            printf("Release     : %d\n",node->release);
            printf("Stock       : %d\n",node->stock);
            printf("Avaiability : %s\n",node->avaiable >= 1 ? "Avaiable" : "Not Avaiable");
            addNewBook(node->title, node->author, node->publisher, node->ref_number, node->release, node->stock, node->avaiable, &selector_head, &selector_tail);
            selected = node;
        }
        node = node->next;
    }
    printf("=======================================\n");
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
            due = addTime(date, 7);
            enqueueBorrowData(front_borrow, rear_borrow, user.name, user.NIM, selected->title, selected->ref_number, "Borrowing", date, due);

            char filename[25];
            strcpy(filename, user.NIM);
            strcat(filename, "-Borrowing data.txt");

            FILE *fp = fopen(filename, "a");
            fprintf(fp,"%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n",(*rear_borrow)->name, (*rear_borrow)->NIM, (*rear_borrow)->title, (*rear_borrow)->ref_number, (*rear_borrow)->status, (*rear_borrow)->borrow.day, (*rear_borrow)->borrow.month, (*rear_borrow)->borrow.year, (*rear_borrow)->due.day, (*rear_borrow)->due.month, (*rear_borrow)->due.year);
            fclose(fp);

            fp = fopen("Book data.txt", "w");

            node = *head_book;
            while(node != NULL){
                fprintf(fp,"%s#%s#%s#%s#%d#%d#%d\n",node->title,node->author,node->publisher,node->ref_number,node->release,node->stock,node->avaiable);
                node = node->next;
            }
            fclose(fp);

            printf("Book borrowed!\n");
        }
        else{
            printf("You can't borrow a same book twice\n");
            return;
        }

    }
    printf("=======================================\n");
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
        printf("Borrower    : %s\n",front_selector->name);
        printf("NIM         : %s\n",front_selector->NIM);
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
    FILE *fp;
    if (strcmp(user.userType, "ADMIN") == 0){
        fp = fopen("History.txt", "r");
    }
    else if (strcmp(user.userType, "STUDENT") == 0){
        char filename[25];
        strcpy(filename, user.NIM);
        strcat(filename, "-History.txt");
        fp = fopen(filename, "r");
    }
    BorrowList history;
    printf("=======================================\n");
    printf("            Borrow History             \n");
    printf("=======================================\n");
    if (fp != NULL){
        while(fscanf(fp, "%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%d/%d/%d#%d/%d/%d\n", history.name, history.NIM, history.title, history.ref_number, history.status, &history.borrow.day, &history.borrow.month, &history.borrow.year, &history.due.day, &history.due.month, &history.due.year) != EOF){
            printf("Borrower       : %s\n", history.name);
            printf("Borrower's NIM : %s\n", history.NIM);
            printf("Title          : %s\n", history.title);
            printf("Ref number     : %s\n", history.ref_number);
            printf("Status         : %s\n", history.status);
            printf("Borrow date    : %d/%d/%d\n", history.borrow.day, history.borrow.month, history.borrow.year);
            printf("Due date       : %d/%d/%d\n\n", history.due.day, history.due.month, history.due.year);
            printf("=======================================\n");
        }
        fclose(fp);
    }
    else{
        printf("History is empty!");
    }
}

void returnBookMenu(Book **head_book, BorrowList **front_borrow, BorrowList **rear_borrow, Date date){
    int loan_number, counter = 1;
    char ref_number[25];
    Account user;

    BorrowList *front_selector, *rear_selector;
    front_selector = rear_selector = NULL;

    if (isEmpty(*front_borrow)){
        printf("List is empty");
        return;
    }

    displayBorrowData(front_borrow, rear_borrow);
    printf("Loan Number: ");
    fflush(stdin);
    scanf("%d", &loan_number);
    fflush(stdin);

    while(!isEmpty(*front_borrow)){
        copy_front_queue(*front_borrow, &front_selector, &rear_selector);
        dequeueBorrowData(front_borrow);
    }

    bool find = false;
    while(!isEmpty(front_selector)){
        if (counter == loan_number){
            find = true;
            strcpy(ref_number, front_selector->ref_number);
            if (strcmp(front_selector->status, "Overdue") == 0){
                strcpy(front_selector->status, "Returned, Overdue");
            }
            else{
                strcpy(front_selector->status, "Returned");
            }
            strcpy(user.NIM, front_selector->NIM);
            char userFileName[25];
            strcpy(userFileName, front_selector->NIM);
            strcat(userFileName, "-History.txt");
            FILE *fpUser = fopen(userFileName, "a");
            FILE *fpAdmin = fopen("History.txt", "a");
            fprintf(fpUser, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", front_selector->name, front_selector->NIM, front_selector->title, front_selector->ref_number, front_selector->status, front_selector->borrow.day, front_selector->borrow.month, front_selector->borrow.year, front_selector->due.day, front_selector->due.month, front_selector->due.year);
            fprintf(fpAdmin, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", front_selector->name, front_selector->NIM, front_selector->title, front_selector->ref_number, front_selector->status, front_selector->borrow.day, front_selector->borrow.month, front_selector->borrow.year, front_selector->due.day, front_selector->due.month, front_selector->due.year);
            fclose(fpUser);
            fclose(fpAdmin);
        }
        else{
            copy_front_queue(front_selector, front_borrow, rear_borrow);
        }
        dequeueBorrowData(&front_selector);
        counter++;
    }

    if (find == true){

        Book *node = *head_book;
        while (node != NULL){
            if (strcmp(node->ref_number, ref_number) == 0){
                node->avaiable += 1;
                break;
            }
            node = node->next;
        }
        updateBorrowFile(front_borrow, rear_borrow, user);
        updateBookFile(*head_book);
        printf("Book returned!\n");
    }
    else{
        printf("Book not found!\n");
    }
    printf("=======================================\n");
}

//========================================================================================================

void freeMemory(Book **head_book, Book **tail_book, BorrowList **front_borrow, BorrowList **rear_borrow){
    //Free memory book
    while(*head_book != NULL){
        Book *trash = *head_book;
        *head_book = (*head_book)->next;
        free(trash);
    }
    *tail_book = *head_book = NULL;

    //Free memory borrow list
    while(*front_borrow != NULL){
        dequeueBorrowData(front_borrow);
    }
    *front_borrow = *rear_borrow = NULL;

}

void adminPage(Account user){
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

    FILE *fp = fopen("Account.txt", "r");
    Account temp_account;
    while(fscanf(fp,"%[^#]#%[^#]#%[^#]#%[^#]#%[^\n]\n",temp_account.userType,temp_account.email,temp_account.password,temp_account.name,temp_account.NIM) != EOF){
        if (strcmp(temp_account.userType, "STUDENT") == 0){
            fetchBorrowData(&front_borrow, &rear_borrow, temp_account);
        }
    }
    fclose(fp);

    while(1){

        FILE *fp = fopen("Account.txt", "r");
        while(fscanf(fp,"%[^#]#%[^#]#%[^#]#%[^#]#%[^\n]\n",temp_account.userType,temp_account.email,temp_account.password,temp_account.name,temp_account.NIM) != EOF){
            if (strcmp(temp_account.userType, "STUDENT") == 0){
                updateBorrowData(&head_book, &front_borrow, &rear_borrow, temp_account, date);
            }
        }
        fclose(fp);

        fflush(stdin);
        printf("=======================================\n");
        printf("               Main Page               \n");
        printf("=======================================\n");
        printf("Welcome, %s\n",user.name);
        printf("Current time: %d/%d/%d\n",date.day, date.month, date.year);
        printf("[1] Show books list\n");
        printf("[2] Search book title\n");
        printf("[3] Add new book\n");
        printf("[4] Remove a book\n");
        printf("[5] Show loan list\n");
        printf("[6] Return loan book\n");
        printf("[7] Show borrow history\n");
        printf("[8] Log out\n");
        printf("[9] Exit\n");
        printf("[0] Change current time (for debugging)\n");
        printf("\nOption: ");
        scanf("%d",&option);
        printf("\n");

        if (option == 0){
            setDate(&date);
        }
        else if (option == 1){
            showAllBook(head_book);
        }
        else if (option == 2){
            searchBookMenu(head_book);
        }
        else if (option == 3){
            addBookMenu(&head_book, &tail_book);
        }
        else if (option == 4){
            removeBook(&head_book, &tail_book, &front_borrow, &rear_borrow);
        }
        else if (option == 5){
            displayBorrowData(&front_borrow, &rear_borrow);
        }
        else if (option == 6){
            returnBookMenu(&head_book, &front_borrow, &rear_borrow, date);
        }
        else if (option == 7){
            displayBorrowHistory(user);
        }
        else if (option == 8){
            freeMemory(&head_book, &tail_book, &front_borrow, &rear_borrow);
            return;
        }
        else if (option == 9){
            freeMemory(&head_book, &tail_book, &front_borrow, &rear_borrow);
            exit(0);
        }
        else{
            printf("Invalid input\n");
        }
        printf("\n");
    }
}

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
        option = -1;
        fflush(stdin);
        printf("=======================================\n");
        printf("               Main Page               \n");
        printf("=======================================\n");
        printf("Welcome, %s\n",user.name);
        printf("Current time: %d/%d/%d\n",date.day, date.month, date.year);
        printf("[1] Show books list\n");
        printf("[2] Search book title\n");
        printf("[3] Borrow a book\n");
        printf("[4] Show loan list\n");
        printf("[5] Show loan history\n");
        printf("[6] Log out\n");
        printf("[7] Exit\n");
        printf("[0] Change current time (for debugging)\n");
        printf("\nOption: ");
        scanf("%d",&option);
        printf("\n");

        if (option == 0){
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
            displayBorrowData(&front_borrow, &rear_borrow);
        }
        else if (option == 5){
            displayBorrowHistory(user);
        }
        else if (option == 6){
            freeMemory(&head_book, &tail_book, &front_borrow, &rear_borrow);
            break;
        }
        else if (option == 7){
            freeMemory(&head_book, &tail_book, &front_borrow, &rear_borrow);
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
        printf("=======================================\n");
        printf("              UMN Library              \n");
        printf("         Book-borrowing System         \n");
        printf("=======================================\n");
        printf("[1] Log in\n");
        printf("[2] Exit\n\n");
        printf("Option: ");
        scanf("%d",&option);
        if (option == 1){
            account = login_page();
            if (strcmp(account.userType,"ADMIN") == 0){
                adminPage(account);
            }
            else if (strcmp(account.userType, "STUDENT") == 0){
                studentPage(account);
            }
            else{
                printf("Invalid login, try again\n");
            }
        }
        else if (option == 2){
            break;
        }
        else{
            printf("Invalid input\n");
        }
        printf("\n");
    }
    return 0;
}
