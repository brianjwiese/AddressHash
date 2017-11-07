//
//  main.c
//  ChefSteps1
//
//  Created by Brian Wiese on 10/31/17.
//  Copyright © 2017 Brian Wiese. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>

#define BUFF 256                                  // read input buffer - max 256 chars for email addr
#define TABLE_BITS 5
#define TABLE_SIZE 140000                                  // range of hash table
#define SHIFT_AMT 5*sizeof(unsigned int) - TABLE_BITS
#define HASH_MULT 2640025017u                               //Fibonacci unsigned hash multiplier
#define EMPTY_LIST NULL

struct addr_list_item                       //address list item data structure
{
    char                    *addr;
    struct addr_list_item   *next;
};

typedef struct addr_list_item Addr_list_item;
typedef Addr_list_item *Addr_list;

static Addr_list addr_ht[TABLE_SIZE];         // Address Hash Table (inits to NULL)
FILE *inFp;                              // input file ptr
FILE *outFp;                              // output file ptr

// ** debug only **
int WordCount = 0;
//int DupCount = 0;

//function prototypes
unsigned int str_to_int (const char *str);      // convert string to integer
unsigned hash(const char *name);                // generate hash value via converted Int from String
void add_to_HTable(const char *iStr);           // check and add to HashTable if NOT duplicate in HashTable

unsigned int str_to_int (const char *str)
{
    unsigned value = 0u, tmp = 0u;
    int size = sizeof(int)/sizeof(char);
    int len = (int) strlen(str);
    
    while (len >= size)                 // size bytes each time
    {
        value ^= *(unsigned *)str;      // xor with explicit cast
        str += size;
        len -= size;
    }
    if (len > 0)                        // up to size-1 bytes left
    {
        strcpy( (char *)&tmp, str);
        value ^= tmp;
    }
    return (value);
}

unsigned hash(const char *name)
{
    unsigned int val;
    val = str_to_int(name);                         //convert string to int value
    return ( (val*HASH_MULT) >> (SHIFT_AMT));
}

void add_to_HTable(const char *iStr)
{
    extern Addr_list addr_ht[TABLE_SIZE];
    
    unsigned hashvalue = hash(iStr);
    
    //set temp Addr_list at hash value of input Str address
    Addr_list tmp = addr_ht[hashvalue];
    
    //search for Str address w/in linked list at HashTable index
    for (; tmp != EMPTY_LIST; tmp = tmp->next)
        if (strcmp (iStr, tmp->addr) == 0)   // if found, it's a DUPLICATE
        {
            // ** debug only
            //DupCount++;
            //printf ("du: %i : h:%u : %s\n", DupCount, hashvalue, tmp->addr);
            
            return;  //return - don't add to the HashTable!
        }
    //else  // ** debug only
    //printf ("compare: %s : %s\n", iStr, tmp->addr);
    
    // allocate unique address item to linked list
    Addr_list newAddr = (Addr_list) malloc (sizeof(Addr_list_item));
    
    int n = (int) strlen(iStr) + 1;
    char * newStr = (char *) malloc (n * sizeof(char));  //allocate new addr char *
    
    strncpy (newStr, iStr, n);
    
    newAddr->next = addr_ht[hashvalue];         // point newAddr.next to
    newAddr->addr = newStr;                     // add new address string to newAddr.addr
    addr_ht[hashvalue] = newAddr;               // add newAddr to HashTable
    
    // ** debug only **
    WordCount++;
    printf ("wc: %i : ", WordCount);
    
    // show hash value
    printf ("h:%u : ", hashvalue);
    
    //show word added to HTable
    printf ("%s\n",  newStr);
    
    fprintf (outFp, "%s ", newStr);      // output "addr string"+SPACE of non-dup addresses
    
    return;
}

int main(int argc, const char * argv[])
{
    clock_t start, end;
    char input [BUFF];
    
    //get start time
    start = clock();
    
    if (argc != 3)
    {
        fprintf (stderr, "%s: expects both input and output file arguments, but %d was provided.\n", argv[0], argc-1);
        fprintf (stderr, "Usage: %s inputfilepath outputfilepath\n", argv[0]);
        exit (1);
    }
    
    if (( inFp = fopen (argv[1], "r")) == EMPTY_LIST )  // open file for reading input data
        
    {
        fprintf (stderr, "%s: Cannot open %s for reading.\n", argv[0], argv[1]);
        exit (1);
    }
    
    if (( outFp = fopen (argv[2], "w")) == EMPTY_LIST )  //open file for writing (overwrite existing)
    {
        fprintf (stderr, "%s: Cannot open %s for writing.\n", argv[0], argv[2]);
        exit (1);
    }
    
    while (fscanf (inFp, "%256s", input) != EOF)   //read addr string while not EOF
    {
        add_to_HTable (input);              // add addr string to Hashtable, if applicable
    }
    
    fclose (inFp);                // close input file ptr
    fclose (outFp);               // close output file ptr
    
    //get end time
    end = clock();
    
    // output runtime duration
    printf ("Duration: %2.5f seconds\n", (double) (end - start) / CLOCKS_PER_SEC);
    
    return (0);
}

