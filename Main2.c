#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define ADMINUSER "ADMIN"
#define STUDENTUSER "STUDENT"

/*================================================ STRUCT ===============================================*/
/*===================== Login Struct ===================*/
typedef struct{
    char userType[8];
    char email[40];
    char password[30];

    char name[30];
    char NIM[12];
}Account;

/*===================== Book Struct ===================*/
typedef struct Book{
    char title[40];
    char author[30];
    char publisher[30];
    char ref_number[25];
    int release;

    int stock;
    int avaiable;
}Book;

/*===================== Date Struct ===================*/
typedef struct{
    int day;
    int month;
    int year;
}Date;

/*===================== Borrow Struct ===================*/
typedef struct BorrowList{
    char name[30];
    char NIM[12];

    char title[40];
    char ref_number[25];

    char status[20];
    Date borrow, due;
}BorrowList;

/*===================== Queue Struct ===================*/
typedef struct Queue{
    int maxSize;
    BorrowList* borrowQueue;
    int front;
    int rear;
    int itemCount;
}Queue;

/*===================== Initialize Queue ===================*/
void initializeQueue(Queue* q, int size) {
    q->maxSize = size;
    q->borrowQueue = (BorrowList*) malloc(sizeof(BorrowList) * size);
    q->front = 0;
    q->rear = -1;
    q->itemCount = 0;
}

/*================================================ DATE ===============================================*/
/*===================== Edit Date (Debuggin due date) ===================*/
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

/*===================== Check Date ===================*/
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

/*===================== Set Date ===================*/
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

/*================================================ LOGIN ===============================================*/
/*===================== Validate Account ===================*/
int validate_account_input(char *email_input, char *password_input, Account *account, int arraySize){
    for(int i = 0; i < arraySize; i++){
        if ((strcmp(email_input, account[i].email) == 0) &&(strcmp(password_input, account[i].password) == 0))
            return i;
    }
    return -1;
}
/*===================== Login Page ===================*/
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

    Queue q;
    initializeQueue(&q, current_size);

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

int check_duplicate_borrow(Queue *borrowQueue, char *bookRef);
/*===================== Show Book ===================*/
void showAllBook(Book *books, int bookCount){
    printf("=======================================\n");
    for(int i = 0; i < bookCount; i++){
        printf("Title       : %s\n",books[i].title);
        printf("Author      : %s\n",books[i].author);
        printf("Publisher   : %s\n",books[i].publisher);
        printf("Ref-number  : %s\n",books[i].ref_number);
        printf("Release     : %d\n",books[i].release);
        printf("Stock       : %d\n",books[i].stock);
        printf("Avaiability : %s\n\n",books[i].avaiable >= 1 ? "Avaiable" : "Not Avaiable");
    }
}

/*===================== Add Book ===================*/
void addNewBook(Book **books, int *bookCount, char *title, char *author, char *publisher, char *ref_number, int release, int stock, int avaiable) {
    *books = realloc(*books, (*bookCount + 1) * sizeof(Book));

    strcpy((*books)[*bookCount].title, title);
    strcpy((*books)[*bookCount].author, author);
    strcpy((*books)[*bookCount].publisher, publisher);
    strcpy((*books)[*bookCount].ref_number, ref_number);
    (*books)[*bookCount].release = release;
    (*books)[*bookCount].stock = stock;
    (*books)[*bookCount].avaiable = avaiable;

    (*bookCount)++;
}


/*===================== Fetch Book ===================*/
void fetchBookData(Book **books, int *bookCount){
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
        addNewBook(books, bookCount, title, author, publisher, ref_number, release, stock, avaiable);
        }
        fclose(fp);
    }
    else{
        return;
    }
}


/*===================== Search Title ===================*/
void searchBook_byTitle(Book *books, int bookCount, char *title_input){
    int found = 0;

    for(int i = 0; i < bookCount; i++){
        if (strcmp(books[i].title, title_input) == 0){
            found++;
            if (found == 1){
                printf("Book title found!\n");
            }
            printf("Title       : %s\n",books[i].title);
            printf("Author      : %s\n",books[i].author);
            printf("Publisher   : %s\n",books[i].publisher);
            printf("Ref-number  : %s\n",books[i].ref_number);
            printf("Release     : %d\n",books[i].release);
            printf("Stock       : %d\n",books[i].stock);
            printf("Avaiability : %s\n\n",books[i].avaiable >= 1 ? "Avaiable" : "Not Avaiable");
        }
    }
    if (found == 0){
        printf("Book title not found!\n");
    }
}

/*===================== Search Menu ===================*/
void searchBookMenu(Book *books, int bookCount){
    char title_input[40];
    printf("     Search Book     \n");
    printf("=====================\n");
    fflush(stdin);
    printf("Title: ");
    scanf("%[^\n]", title_input);
    fflush(stdin);
    searchBook_byTitle(books, bookCount, title_input);
}

/*===================== Update Book File ===================*/
void updateBookFile(Book *books, int bookCount){
    FILE *fp;
    fp = fopen("Book data.txt", "w");

    for(int i = 0; i < bookCount; i++){
        fprintf(fp, "%s#%s#%s#%s#%d#%d#%d\n",books[i].title, books[i].author,books[i].publisher,books[i].ref_number, books[i].release, books[i].stock, books[i].avaiable);
    }
    fclose(fp);
}

/*===================== Add Book Menu ===================*/
void addBookMenu(Book **books, int *bookCount){
    Book newBook;

    fflush(stdin);
    printf("       Add Book       \n");
    printf("======================\n");
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

    addNewBook(books, bookCount, newBook.title, newBook.author, newBook.publisher, newBook.ref_number, newBook.release, newBook.stock, newBook.avaiable);
    updateBookFile(*books, *bookCount);
}


int isBorrowed(Queue *borrowQueue, char *bookRef){
    return check_duplicate_borrow(borrowQueue, bookRef);
}

void removeBook(Book *books, int *bookCount, Queue *borrowQueue){
    char ref_number_input[25];
    printf("     Remove Book     \n");
    printf("=====================\n");
    showAllBook(books, *bookCount);
    printf("=====================\n");
    printf("Ref-number: ");
    fflush(stdin);
    scanf("%[^\n]",ref_number_input);
    fflush(stdin);

    for(int i = 0; i < *bookCount; i++){
        if (strcmp(books[i].ref_number, ref_number_input) == 0){
            if (isBorrowed(borrowQueue, ref_number_input) > 0){
                printf("This book is being borrowed, please wait for the book to be returned\n");
                return;
            }

            // Shift all elements to the left
            for(int j = i; j < *bookCount - 1; j++){
                books[j] = books[j + 1];
            }
            (*bookCount)--;

            printf("Book removed\n");
            return;
        }
    }

    printf("Book not found\n");
}

int isEmpty(Queue *q){
    return q->itemCount == 0;
}

void enqueueBorrowData(Queue *q, char *name, char* NIM, char* title, char* ref_number, char* status, Date borrow, Date due){
    if(q->itemCount < q->maxSize){
        BorrowList data;
        strcpy(data.name, name);
        strcpy(data.NIM, NIM);
        strcpy(data.title, title);
        strcpy(data.ref_number, ref_number);
        strcpy(data.status, status);

        data.borrow.day = borrow.day;
        data.borrow.month = borrow.month;
        data.borrow.year = borrow.year;

        data.due.day = due.day;
        data.due.month = due.month;
        data.due.year = due.year;

        q->rear = (q->rear + 1) % q->maxSize;
        q->borrowQueue[q->rear] = data;
        q->itemCount++;
    } else {
        printf("Queue is full\n");
    }
}

void dequeueBorrowData(Queue *q){
    if (isEmpty(q)){
        printf("Dequeue error: Queue is empty\n");
        return;
    }
    q->front = (q->front + 1) % q->maxSize;
    q->itemCount--;
}

void fetchBorrowData(Queue *q, Account account){
    char name[30];
    char NIM[12];

    char title[40];
    char ref_number[25];

    char status[20];
    Date borrow, due;

    FILE *fp;
    char filename[25];
    strcpy(filename,account.NIM);
    strcat(filename, "-Borrowing data.txt");
    fp = fopen(filename, "r");
    if (fp != NULL){
        while (fscanf(fp,"%[^#]# %[^#]# %[^#]# %[^#]# %[^#]# %d/%d/%d# %d/%d/%d\n", name, NIM, title, ref_number, status, &borrow.day, &borrow.month, &borrow.year, &due.day, &due.month, &due.year) != EOF){
            enqueueBorrowData(q, name, NIM, title, ref_number, status, borrow, due);
        }
        fclose(fp);
    }
}


void copy_front_queue(Queue *origin, Queue *destination){
    BorrowList data = origin->borrowQueue[origin->front];
    enqueueBorrowData(destination, data.name, data.NIM, data.title, data.ref_number, data.status, data.borrow, data.due);
}

int check_duplicate_borrow(Queue *borrowQueue, char *bookRef){

    int flag = 0;
    int current = borrowQueue->front;

    if(borrowQueue->rear == -1){
        //printf("Borrow queue is still empty\n\n");
        return flag;
    }

    else{
        while(current != borrowQueue->rear){
            if (strcmp(borrowQueue->borrowQueue[current].ref_number, bookRef) == 0){
                flag++;
            }
            current = (current + 1) % borrowQueue->maxSize;
        }
    }

    return flag;
}

/*===================== Return Book ===================*/
void returnBook(Book *books, int bookCount, char* ref_number){
    int found = 0;

    FILE *fp = fopen("Book data.txt", "w");

    for(int i = 0; i < bookCount; i++){
        if (strcmp(books[i].ref_number, ref_number) == 0){
            books[i].avaiable += 1;
        }
        fprintf(fp,"%s#%s#%s#%s#%d#%d#%d\n",books[i].title,books[i].author,books[i].publisher,books[i].ref_number,books[i].release,books[i].stock,books[i].avaiable);
    }
    fclose(fp);
}
/*===================== Update Borrow File ===================*/
void updateBorrowFile(Queue *borrowQueue, Account user){
    FILE *fp;
    char filename[25];
    strcpy(filename, user.NIM);
    strcat(filename,"-Borrowing data.txt");
    fp = fopen(filename, "w");

    int current = borrowQueue->front;
    printf("%d",borrowQueue->front);
    while(current != borrowQueue->rear){
        fprintf(fp, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", borrowQueue->borrowQueue[current].name, borrowQueue->borrowQueue[current].NIM, borrowQueue->borrowQueue[current].title, borrowQueue->borrowQueue[current].ref_number, borrowQueue->borrowQueue[current].status, borrowQueue->borrowQueue[current].borrow.day, borrowQueue->borrowQueue[current].borrow.month, borrowQueue->borrowQueue[current].borrow.year, borrowQueue->borrowQueue[current].due.day, borrowQueue->borrowQueue[current].due.month, borrowQueue->borrowQueue[current].due.year);
        current = (current + 1) % borrowQueue->maxSize;
        if (current == borrowQueue->front) {
            break;
        }

    }
    fclose(fp);
}

/*===================== Update Borrow Data ===================*/
void updateBorrowData(Queue *q, Account user, Date date){
    //Update for overdue
    Queue borrowQueueClone;
    initializeQueue(&borrowQueueClone, q->maxSize);
    bool isoverdue = false;
    double time_remaining;

    //Clone queue
    while(!isEmpty(q)){
        enqueueBorrowData(&borrowQueueClone, q->borrowQueue[q->front].name, q->borrowQueue[q->front].NIM, q->borrowQueue[q->front].title, q->borrowQueue[q->front].ref_number, q->borrowQueue[q->front].status, q->borrowQueue[q->front].borrow, q->borrowQueue[q->front].due);
        dequeueBorrowData(q);
    }
    while(!isEmpty(&borrowQueueClone)){
        time_remaining = check_date(date, borrowQueueClone.borrowQueue[borrowQueueClone.front].due);
        //Cek apakah pinjam lebih dari 7 hari / overdue
        if (time_remaining <= 0 && (strcmp(borrowQueueClone.borrowQueue[borrowQueueClone.front].status, "Borrowing") == 0)){
            isoverdue = true;
            strcpy(borrowQueueClone.borrowQueue[borrowQueueClone.front].status, "Overdue");
            enqueueBorrowData(q, borrowQueueClone.borrowQueue[borrowQueueClone.front].name, borrowQueueClone.borrowQueue[borrowQueueClone.front].NIM, borrowQueueClone.borrowQueue[borrowQueueClone.front].title, borrowQueueClone.borrowQueue[borrowQueueClone.front].ref_number, borrowQueueClone.borrowQueue[borrowQueueClone.front].status, borrowQueueClone.borrowQueue[borrowQueueClone.front].borrow, borrowQueueClone.borrowQueue[borrowQueueClone.front].due);
            dequeueBorrowData(&borrowQueueClone);
        }
        else{
            enqueueBorrowData(q, borrowQueueClone.borrowQueue[borrowQueueClone.front].name, borrowQueueClone.borrowQueue[borrowQueueClone.front].NIM, borrowQueueClone.borrowQueue[borrowQueueClone.front].title, borrowQueueClone.borrowQueue[borrowQueueClone.front].ref_number, borrowQueueClone.borrowQueue[borrowQueueClone.front].status, borrowQueueClone.borrowQueue[borrowQueueClone.front].borrow, borrowQueueClone.borrowQueue[borrowQueueClone.front].due);
            dequeueBorrowData(&borrowQueueClone);
        }
    }
    if (isoverdue){
            updateBorrowFile(q, user);
    }
}

int check_full_loan(Queue *q){
    return q->itemCount;
}

/*===================== Borrow Data ===================*/
void borrowBook(Book *books, int bookCount, Queue *borrowQueue, Date date, Account user){
    char title_input[40], ref_number_input[25];
    int found_title = 0, found_ref = 0;

    if (check_full_loan(borrowQueue) == 4){
        printf("You can only borrow 4 books at a time!\n");
        return;
    }

    showAllBook(books, bookCount);
    printf("=====================\n");
    printf("     Borrow Book     \n");
    printf("=====================\n");
    fflush(stdin);
    printf("Title: ");
    scanf("%[^\n]", title_input);
    fflush(stdin);

    // Check input
    Book selected;
    for(int i = 0; i < bookCount; i++){
        if (strcmp(books[i].title, title_input) == 0){
            found_title++;
            if (found_title == 1){
                printf("Book title found!\n");
            }

            printf("========================================\n");
            printf("Title       : %s\n", books[i].title);
            printf("Author      : %s\n", books[i].author);
            printf("Publisher   : %s\n", books[i].publisher);
            printf("Ref-number  : %s\n", books[i].ref_number);
            printf("Release     : %d\n", books[i].release);
            printf("Stock       : %d\n", books[i].stock);
            printf("Avaiability : %s\n\n", books[i].avaiable >= 1 ? "Avaiable" : "Not Avaiable");
            selected = books[i];
        }
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

        for(int i = 0; i < bookCount; i++){
            if (strcmp(books[i].ref_number, ref_number_input) == 0){
                found_ref = 1;
                selected = books[i];
                break;
            }
        }
        if (found_ref == 0){
            printf("Ref-number not found!");
            return;
        }
    }

    if (found_ref == 1 || found_title == 1){
        if (found_title == 1){
            strcpy(ref_number_input, selected.ref_number);
        }

        if (check_duplicate_borrow(borrowQueue, ref_number_input) == 0){
            if (selected.avaiable == 0){
                printf("Book is not avaiable for now\n");
                return;
            }
            for(int i = 0; i < bookCount; i++){
                if (strcmp(books[i].ref_number, selected.ref_number) == 0){
                    books[i].avaiable -= 1;
                }
            }

            Date due;
            due = addTime(date, 7);
            enqueueBorrowData(borrowQueue, user.name, user.NIM, selected.title, selected.ref_number, "Borrowing", date, due);

            char filename[25];
            strcpy(filename, user.NIM);
            strcat(filename, "-Borrowing data.txt");

            FILE *fp = fopen(filename, "a");
            fprintf(fp, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", borrowQueue->borrowQueue[borrowQueue->rear].name, borrowQueue->borrowQueue[borrowQueue->rear].NIM, borrowQueue->borrowQueue[borrowQueue->rear].title, borrowQueue->borrowQueue[borrowQueue->rear].ref_number, borrowQueue->borrowQueue[borrowQueue->rear].status, borrowQueue->borrowQueue[borrowQueue->rear].borrow.day, borrowQueue->borrowQueue[borrowQueue->rear].borrow.month, borrowQueue->borrowQueue[borrowQueue->rear].borrow.year, borrowQueue->borrowQueue[borrowQueue->rear].due.day, borrowQueue->borrowQueue[borrowQueue->rear].due.month, borrowQueue->borrowQueue[borrowQueue->rear].due.year);
            fclose(fp);

            fp = fopen("Book data.txt", "w");

            for(int i = 0; i < bookCount; i++){
                fprintf(fp,"%s#%s#%s#%s#%d#%d#%d\n", books[i].title, books[i].author, books[i].publisher, books[i].ref_number, books[i].release, books[i].stock, books[i].avaiable);
            }
            fclose(fp);

            printf("Book reserved, please pick it in the library in 2 days\n");
            selected.avaiable -= 1;
        }
        else{
            printf("You can't borrow a same book twice\n");
            return;
        }
    }
}

void displayBorrowData(Queue *q){
    Queue borrowQueueClone;
    initializeQueue(&borrowQueueClone, q->maxSize);

    Queue tempQueue;
    initializeQueue(&tempQueue, q->maxSize);

    while(!isEmpty(q)){
        copy_front_queue(q, &borrowQueueClone);
        copy_front_queue(q, &tempQueue);
        dequeueBorrowData(q);
    }

    while(!isEmpty(&tempQueue)){
        copy_front_queue(&tempQueue, q);
        dequeueBorrowData(&tempQueue);
    }

    if (isEmpty(&borrowQueueClone)){
        printf("List is empty\n\n");
        return;
    }
    else {
        printf("======================================\n");
        printf("             Borrow List              \n");
        printf("======================================\n\n");
    }

    int i = 1;
    while(!isEmpty(&borrowQueueClone)){
        if (strcmp(borrowQueueClone.borrowQueue[borrowQueueClone.front].status, "Borrowing") == 0) {
            printf("Loan #%d\n", i);
            i++;
            printf("Title       : %s\n", borrowQueueClone.borrowQueue[borrowQueueClone.front].title);
            printf("Ref number  : %s\n", borrowQueueClone.borrowQueue[borrowQueueClone.front].ref_number);
            printf("Status      : %s\n", borrowQueueClone.borrowQueue[borrowQueueClone.front].status);
            printf("Borrow date : %d/%d/%d\n", borrowQueueClone.borrowQueue[borrowQueueClone.front].borrow.day, borrowQueueClone.borrowQueue[borrowQueueClone.front].borrow.month, borrowQueueClone.borrowQueue[borrowQueueClone.front].borrow.year);
            printf("Due date    : %d/%d/%d\n\n", borrowQueueClone.borrowQueue[borrowQueueClone.front].due.day, borrowQueueClone.borrowQueue[borrowQueueClone.front].due.month, borrowQueueClone.borrowQueue[borrowQueueClone.front].due.year);
            printf("======================================\n");
        }
        dequeueBorrowData(&borrowQueueClone);
    }
}

/*===================== Borrow History ===================*/
void displayBorrowHistory(Account user){
    FILE *fp;
    if (strcmp(user.userType, ADMINUSER) == 0){
        fp = fopen("History.txt", "r");
    }
    else if (strcmp(user.userType, STUDENTUSER) == 0){
        char filename[25];
        strcpy(filename, user.NIM);
        strcat(filename, "-History.txt");
        fp = fopen(filename, "r");
    }
    BorrowList history;
    printf("======================================\n");
    printf("            Borrow History            \n");
    printf("======================================\n");
    if (fp != NULL){
        while(fscanf(fp, "%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%d/%d/%d#%d/%d/%d\n", history.name, history.NIM, history.title, history.ref_number, history.status, &history.borrow.day, &history.borrow.month, &history.borrow.year, &history.due.day, &history.due.month, &history.due.year) != EOF){
            printf("Borrower       : %s\n", history.name);
            printf("Borrower's NIM : %s\n", history.NIM);
            printf("Title          : %s\n", history.title);
            printf("Ref number     : %s\n", history.ref_number);
            printf("Status         : %s\n", history.status);
            printf("Borrow date    : %d/%d/%d\n", history.borrow.day, history.borrow.month, history.borrow.year);
            printf("Due date       : %d/%d/%d\n\n", history.due.day, history.due.month, history.due.year);
            printf("======================================\n");
        }
        fclose(fp);
    }
    else{
        printf("History is empty!");
    }
}

/*===================== Return Book ===================*/
void returnBookMenu(Book *book, int booksCount, Queue *q, Date date){
    int choose, flag = 0;
    int counter = 0, current = 1;
    Account user;

    char ref_number[25];

    Queue selectorQueue;
    initializeQueue(&selectorQueue, q->maxSize);

    if (isEmpty(q)){
        printf("List is empty");
        return;
    }

    displayBorrowData(q);

    printf("Choose: ");
    scanf("%d",&choose);

    //cek choose
    if ((choose < 1) || (choose > 4)){
        flag++;
    }
    else{
        while(!isEmpty(q)){
            copy_front_queue(q, &selectorQueue);
            dequeueBorrowData(q);
            counter++;
        }
        if (counter < choose){
            printf("Invalid input");
        }
        else{
            int find = 0;
            while(!isEmpty(&selectorQueue)){
                if (current == choose){
                    find = 1;
                    strcpy(ref_number, selectorQueue.borrowQueue[selectorQueue.front].ref_number);
                    if(strcmp(selectorQueue.borrowQueue[selectorQueue.front].status, "Overdue") == 0) {
                        strcpy(selectorQueue.borrowQueue[selectorQueue.front].status, "Returned, Overdue");
                    }
                    if(strcmp(selectorQueue.borrowQueue[selectorQueue.front].status, "Borrowing") == 0) {
                        strcpy(selectorQueue.borrowQueue[selectorQueue.front].status, "Returned");
                    }

                    strcpy(user.NIM, selectorQueue.borrowQueue[selectorQueue.front].NIM);
                    char userFileName[25];
                    strcpy(userFileName, selectorQueue.borrowQueue[selectorQueue.front].NIM);
                    strcat(userFileName, "-History.txt");
                    FILE *fpUser = fopen(userFileName, "a");
                    FILE *fpAdmin = fopen("History.txt", "a");
                    fprintf(fpAdmin, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", selectorQueue.borrowQueue[selectorQueue.front].name, selectorQueue.borrowQueue[selectorQueue.front].NIM, selectorQueue.borrowQueue[selectorQueue.front].title, selectorQueue.borrowQueue[selectorQueue.front].ref_number, selectorQueue.borrowQueue[selectorQueue.front].status, selectorQueue.borrowQueue[selectorQueue.front].borrow.day, selectorQueue.borrowQueue[selectorQueue.front].borrow.month, selectorQueue.borrowQueue[selectorQueue.front].borrow.year, selectorQueue.borrowQueue[selectorQueue.front].due.day, selectorQueue.borrowQueue[selectorQueue.front].due.month, selectorQueue.borrowQueue[selectorQueue.front].due.year);
                    fprintf(fpUser, "%s#%s#%s#%s#%s#%d/%d/%d#%d/%d/%d\n", selectorQueue.borrowQueue[selectorQueue.front].name, selectorQueue.borrowQueue[selectorQueue.front].NIM, selectorQueue.borrowQueue[selectorQueue.front].title, selectorQueue.borrowQueue[selectorQueue.front].ref_number, selectorQueue.borrowQueue[selectorQueue.front].status, selectorQueue.borrowQueue[selectorQueue.front].borrow.day, selectorQueue.borrowQueue[selectorQueue.front].borrow.month, selectorQueue.borrowQueue[selectorQueue.front].borrow.year, selectorQueue.borrowQueue[selectorQueue.front].due.day, selectorQueue.borrowQueue[selectorQueue.front].due.month, selectorQueue.borrowQueue[selectorQueue.front].due.year);
                    fclose(fpUser);
                    fclose(fpAdmin);
                }
                else{
                    copy_front_queue(&selectorQueue, q);
                }
                dequeueBorrowData(&selectorQueue);
                current++;
            }
            if (find == 1){
                for(int i = 0; i < booksCount; i++){
                    if (strcmp(book[i].ref_number, ref_number) == 0){
                        book[i].avaiable += 1;
                        break;
                    }
                }
                updateBorrowFile(q,user);
            }
        }
    }
}

void freeMemory(Book **books, int *bookCount, Queue *q){
    if (*books != NULL){
        free(*books);
        *books = NULL;
        *bookCount = 0;
    }

    //Free memory borrow list
    if (q->borrowQueue != NULL){
        free(q->borrowQueue);
        q->borrowQueue = NULL;
        q->front = 0;
        q->rear = -1;
        q->itemCount = 0;
    }
}


/*================================================ PAGE ===============================================*/
/*===================== Admin Page ===================*/
void adminPage(Account user){
    int queueSize = 100;
    int bookCount = 0;
    Book *books = NULL;

    Queue borrowQueue;
    initializeQueue(&borrowQueue, queueSize);

    int option;

    //Book
    fetchBookData(&books, &bookCount);

    //Time
    Date date;
    getDate(&date);

    FILE *fp = fopen("Account.txt", "r");
    Account temp_account;
    while(fscanf(fp, "%[^#]#%[^#]#%[^#]#%[^#]#%[^\n]\n", temp_account.userType,temp_account.email,temp_account.password,temp_account.name,temp_account.NIM) != EOF){
        fetchBorrowData(&borrowQueue, temp_account);
    }
    fclose(fp);

    while(1){

        FILE *fp = fopen("Account.txt", "r");
        while(fscanf(fp,"%[^#]#%[^#]#%[^#]#%[^#]#%[^\n]\n",temp_account.userType,temp_account.email,temp_account.password,temp_account.name,temp_account.NIM) != EOF){
            updateBorrowData(&borrowQueue, temp_account, date);
        }
        fclose(fp);

        fflush(stdin);
        printf("=======================================\n");
        printf("               Main Page               \n");
        printf("=======================================\n");
        printf("Welcome, %s\n",user.name);
        printf("Current time: %d/%d/%d\n",date.day, date.month, date.year);
        printf("[1] Show Books list\n");
        printf("[2] Search book\n");
        printf("[3] Add new book\n");
        printf("[4] Remove a book\n");
        printf("[5] Show borrowed book list\n");
        printf("[6] Edit borrower status\n");
        printf("[7] Show borrow history\n");
        printf("[8] Log out\n");
        printf("[9] Exit\n");
        printf("[0] Change current time (for debugging)\n");
        printf("\nOption: ");
        scanf("%d", &option);
        printf("\n");

        if (option == 0){
            setDate(&date);
        }
        else if (option == 1){
            showAllBook(books, bookCount);
        }
        else if (option == 2){
            searchBookMenu(books, bookCount);
        }
        else if (option == 3){
            addBookMenu(&books, &bookCount);
        }
        else if (option == 4){
            removeBook(books, &bookCount, &borrowQueue);
        }
        else if (option == 5){
            displayBorrowData(&borrowQueue);
        }
        else if (option == 6){
            returnBookMenu(books, bookCount, &borrowQueue, date);
        }
        else if (option == 7){
            displayBorrowHistory(user);
        }
        else if (option == 8){
            freeMemory(&books, &bookCount, &borrowQueue);
            return;
        }
        else if (option == 9){
            freeMemory(&books, &bookCount, &borrowQueue);
            exit(0);
        }
        else{
            printf("Invalid input\n");
        }
        printf("\n");
    }
}



/*===================== Student Page ===================*/
void studentPage(Account user){
    int queueSize = 4;
    int bookCount = 0;
    Book *books = NULL;

    Queue borrowQueue;
    initializeQueue(&borrowQueue, queueSize);

    int option;

    //Book
    fetchBookData(&books, &bookCount);

    //Time
    Date date;
    getDate(&date);

    //Borrow
    fetchBorrowData(&borrowQueue, user);

    while(1){

        updateBorrowData(&borrowQueue, user, date);

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
        printf("[6] Show borrow history\n");
        printf("[7] Log out\n");
        printf("[8] Exit\n");
        printf("[0] Change current time (for debugging)\n");
        printf("\nOption: ");
        scanf("%d",&option);
        printf("\n");

        if (option == 0){
            setDate(&date);
        }
        else if (option == 1){
            showAllBook(books, bookCount);
        }
        else if (option == 2){
            searchBookMenu(books, bookCount);
        }
        else if (option == 3){
            borrowBook(books, bookCount, &borrowQueue, date, user);
        }
        else if (option == 4){
            //returnBookMenu(&borrowQueue, date, user);
        }
        else if (option == 5){
            displayBorrowData(&borrowQueue);
        }
        else if (option == 6){
            displayBorrowHistory(user);
        }
        else if (option == 7){
            freeMemory(&books, &bookCount, &borrowQueue);
            return;
        }
        else if (option == 8){
            freeMemory(&books, &bookCount, &borrowQueue);
            exit(0);
        }
        else{
            printf("Invalid input\n");
        }
        printf("\n");
    }
}


/*===================== Sign In ===================*/
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
                adminPage(account);
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
