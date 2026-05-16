// Secure Bank Account Management System
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100

// structure definition
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// function prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void clearInputBuffer(void);

int main(int argc, char *argv[])
{
    FILE *cfPtr;
    unsigned int choice;

    // open file
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        // create file if it doesn't exist
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be opened.\n");
            return 1;
        }

        // initialize blank records
        struct clientData blankClient = {0};

        for (int i = 0; i < MAX_ACCOUNTS; i++)
        {
            fwrite(&blankClient,
                   sizeof(struct clientData),
                   1,
                   cfPtr);
        }

        rewind(cfPtr);
    }

    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        default:
            printf("Invalid choice.\n");
            break;
        }
    }

    fclose(cfPtr);

    printf("Program ended.\n");

    return 0;
}

// menu function
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("\n===== BANK MENU =====\n");
    printf("1 - Store text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Exit\n");
    printf("Enter your choice: ");

    if (scanf("%u", &menuChoice) != 1)
    {
        clearInputBuffer();
        return 0;
    }

    return menuChoice;
}

// create text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;

    struct clientData client = {0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        printf("Text file could not be created.\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr,
            "%-6s%-16s%-11s%10s\n",
            "Acct",
            "Last Name",
            "First Name",
            "Balance");

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);

    printf("accounts.txt created successfully.\n");
}

// update record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;

    struct clientData client = {0};

    printf("Enter account number (1-100): ");

    if (scanf("%u", &account) != 1 ||
        account < 1 ||
        account > MAX_ACCOUNTS)
    {
        printf("Invalid account number.\n");
        clearInputBuffer();
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    if (fread(&client,
              sizeof(struct clientData),
              1,
              fPtr) != 1)
    {
        printf("File read error.\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    printf("\nAccount Details:\n");
    printf("%-6u %-15s %-10s %.2f\n",
           client.acctNum,
           client.lastName,
           client.firstName,
           client.balance);

    printf("Enter transaction (+ deposit / - withdraw): ");

    if (scanf("%lf", &transaction) != 1)
    {
        printf("Invalid amount.\n");
        clearInputBuffer();
        return;
    }

    if (client.balance + transaction < 0)
    {
        printf("Insufficient balance.\n");
        return;
    }

    client.balance += transaction;

    fseek(fPtr,
          -sizeof(struct clientData),
          SEEK_CUR);

    if (fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr) != 1)
    {
        printf("File write error.\n");
        return;
    }

    printf("Account updated successfully.\n");
}

// add new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0};

    unsigned int accountNum;

    printf("Enter new account number (1-100): ");

    if (scanf("%u", &accountNum) != 1 ||
        accountNum < 1 ||
        accountNum > MAX_ACCOUNTS)
    {
        printf("Invalid account number.\n");
        clearInputBuffer();
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    if (fread(&client,
              sizeof(struct clientData),
              1,
              fPtr) != 1)
    {
        printf("File read error.\n");
        return;
    }

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter lastname firstname balance:\n");

    if (scanf("%14s %9s %lf",
              client.lastName,
              client.firstName,
              &client.balance) != 3)
    {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }

    if (client.balance < 0)
    {
        printf("Balance cannot be negative.\n");
        return;
    }

    client.acctNum = accountNum;

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    if (fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr) != 1)
    {
        printf("File write error.\n");
        return;
    }

    printf("Account created successfully.\n");
}

// delete record
void deleteRecord(FILE *fPtr)
{
    struct clientData client = {0};
    struct clientData blankClient = {0};

    unsigned int accountNum;

    printf("Enter account number to delete (1-100): ");

    if (scanf("%u", &accountNum) != 1 ||
        accountNum < 1 ||
        accountNum > MAX_ACCOUNTS)
    {
        printf("Invalid account number.\n");
        clearInputBuffer();
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    if (fread(&client,
              sizeof(struct clientData),
              1,
              fPtr) != 1)
    {
        printf("File read error.\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    if (fwrite(&blankClient,
               sizeof(struct clientData),
               1,
               fPtr) != 1)
    {
        printf("File write error.\n");
        return;
    }

    printf("Account deleted successfully.\n");
}

// clear invalid input buffer
void clearInputBuffer(void)
{
    while (getchar() != '\n');
}