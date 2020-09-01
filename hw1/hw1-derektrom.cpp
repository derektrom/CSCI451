//Derek Trom
#include <string>
#include <stdio.h>
#include <unistd.h>

int findWebAddresses()
{
    system("wget -q https://www.senate.gov/general/contact_information/senators_cfm.cfm -O output.txt");
    int success = system("grep -Eoi 'Contact:\n<a [^>]+>' output.txt | grep -Eo 'href=\"[^\\\"]+\"' | grep -Eo '(http|https)://[^/\"]+' | sort -u");
    system("unlink output.txt");
    if (success == 0)
    {
        printf("Success!!!!");
        return 1;
    }
    else
    {
        printf("Unsuccessful");
        return 0;
    }
}
int main()
{
    int worked = findWebAddresses();
    if (worked == 1)
    {
        printf("Success good bye!");
        exit(0);
    }
}
