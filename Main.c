#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ADMINUSER "ADMIN"
#define STUDENTUSER "STUDENT"

//Struct biasa
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
    int release;

    int stock;
    int avaiable;

    struct Book *prev, *next;
}Book;

//BorrowList date
typedef struct{
    int day;
    int month;
    int year;
}Date;

//Queue
typedef struct BorrowList{
    char name[30];
    char NIM[12];
    char email[40];
    Date borrow, due;
    struct BorrowList *next;
}BorrowList;

//========================================================================================================
//                                         For Debugging Purpose

//Buat edit tanggal sekarang (debugging due date);

//========================================================================================================
//                                       For Login Purpose (beres)
//
int validate_account_input(char *email_input, char *password_input, Account *account, int arraySize){
    for(int i = 0; i < arraySize; i++){
        if ((strcmp(email_input, account[i].email) == 0) &&(strcmp(password_input, account[i].password) == 0))
            return i;
    }
    return -1;
}



//Use Array of Struct
Account login_page(){
    Account *account = (Account*)malloc(sizeof(Account));
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
            //printf("%s %s\n",account[current_size-1].email, account[current_size-1].password); for debugging
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
            return account[index];
        }
        else{
            printf("Wrong email/password, try again\n");
        }

    }

}

//========================================================================================================
//                                          Shared function

void *createNewNode(char *title, char *author, int release, int stock, int avaiable, Book **head, Book **tail){
    Book *newNode = (Book*) malloc(sizeof(Book));
    strcpy(newNode->title, title);
    strcpy(newNode->author, author);
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
    int release;

    int stock;
    int avaiable;

    if (fp != NULL){
        while(fscanf(fp,"%[^#]#%[^#]#%d#%d#%d\n",title,author,&release,&stock,&avaiable) != EOF){
            createNewNode(title,author,release,stock,avaiable,head,tail);
        }
        fclose(fp);
    }
    else{
        return;
    }
}

//function lihat daftar buku

//function cari buku (bisa cari judul, author, & release year)

//========================================================================================================
//                                             For Admin

//function tambah judul buku baru

//function hapus buku

//function edit rincian buku (yang bisa di edit hanya stock)
//JANGAN MENGURANGI BUKU YANG SEDANG DIPINJAM
//CONTOH Jika sebuah buku stocknya 3, trs dipinjam 2, maka input harus dicek (di kasus ini, input wajib >= 2)

//function lihat daftar peminjam buku

//function edit status peminjam

//function lihat histori peminjam buku

void adminPage(Account user){
    Book *head, *tail;
    head = tail = NULL;
    fetchBookData(&head, &tail);
}

//========================================================================================================
//                                             For Student

//function pinjam buku

//function kembalikan buku

//function lihat histori peminjaman buku

void studentPage(Account user){
    Book *head, *tail;
    head = tail = NULL;
    fetchBookData(&head, &tail);
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
            if (strcmp(account.userType,ADMINUSER) == 0)
                adminPage(account);
            else if (strcmp(account.userType, STUDENTUSER) == 0)
                studentPage(account);
            else
                printf("Invalid login, try again\n");
        }

        else if (option == 2)
            break;
        else
            printf("Invalid input\n");

        printf("\n");
    }
    return 0;
}
