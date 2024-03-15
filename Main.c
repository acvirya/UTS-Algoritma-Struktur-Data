#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ADMINUSER "ADMIN"
#define STUDENTUSER "STUDENT"

typedef struct Account{
    char userType[8];
    char email[40];
    char password[30];

    char name[30];
    char NIM[12];
}Account;

//========================================================================================================
//                                             For Login Purpose

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
            printf("%s %s\n",account[current_size-1].email, account[current_size-1].password);
            current_size++;
        }
        current_size--;

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
        printf("Masukkan data\n");
        printf("Email: ");
        scanf("%[^\n]",email_input);
        fflush(stdin);
        printf("Password: ");
        scanf("%[^\n]",password_input);
        fflush(stdin);

        int index = validate_account_input(email_input,password_input, account, current_size);
        if (index >= 0){
            printf("login successfully, navigating to ");
            return account[index];
        }
        else{
            printf("Wrong email/password, try again\n");
        }

    }

}

//========================================================================================================
//                                         For Debugging Purpose



//========================================================================================================
//                                          Shared function



//========================================================================================================
//                                             For Admin

void adminPage(Account user){
}

//========================================================================================================
//                                             For Student

void studentPage(Account user){
    printf("       ");

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
