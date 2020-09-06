/***********************
*Derek Trom
*derek.trom@und.edu
*CSCI 451 HW1
*7 September 2020
*************************/
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
int findWebAddresses()
{
    //use WGET to download webpage and save to output.txt
    system("wget -q https://www.senate.gov/general/contact_information/senators_cfm.cfm -O output.txt");
    //grep and use regex from output and pipe two times to narrow down 
    int success = system("grep -Eo '<a [^>]+>' output.txt | grep -Eo 'href=\"[^\\\"]+\"' | grep -Eo '(http|https)://[a-zA-Z0-9./?=_%:-]*' | sort");
    system("unlink output.txt");
    //0 means successful system calls 
    if (success == 0)
    {
        return 1;
    }
    //failure 
    else
    {
        return 0;
    }
}
int main()
{
    //call function and if 1 returned it was successful
    int worked = findWebAddresses();
    if (worked == 1)
    {
        printf("Success good bye!\n");
        exit(0);
    }
}
