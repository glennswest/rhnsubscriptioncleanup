// Based On: https://github.com/jobbler/cdn_reports

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "cJSON.h"


void process_key(char *);
void get_list(char *);
char *readEntireFile( char *thefilename);
void parse_object(cJSON *item);

char *owner_id;
char *user_id;
char *password_id;

char *json;
cJSON * root;

int main()
{
char thebuf[512];
char *theline;
FILE *thelist;
char *the_char;
char *the_key;
char *the_end;

        owner_id = getenv("ownerid");
        if (owner_id == NULL){
           printf("Please set the ownerid, can be retrieved using subscription manager\n");
           printf("export ownerid=999999\n");
           return(0);
           }
        user_id = getenv("user");
        if (user_id == NULL){
           printf("Please set the user - export user=xyz@redhat.com\n");
           return(0);
           }
        password_id = getenv("password");
        if (password_id == NULL){
           printf("Please set the rhn password variable\n");
           printf("export password=nojoke\n");
           exit(0);
           }
        printf("Ownerid: %s\n",owner_id);
        printf("Userid: %s\n", user_id);
        get_list(owner_id);
        json = readEntireFile("/tmp/cleanup.out");
        root = cJSON_Parse(json);
        parse_object(root);
        return(0);
        thelist = fopen("/tmp/remove_list","r");
	while(!feof(thelist)){
            theline = fgets(thebuf,500,thelist);
            if (theline == NULL){
               return(0);
               }
            the_key = strchr(thebuf,'|');
            if (the_key != NULL){
               the_key++; // SKip space
               the_end = strchr(++the_key,' ');
               *the_end = 0;
               printf("%s\n",the_key);
               process_key(the_key);
               }
	    }


}

void parse_object(cJSON *item)
{
int i;
char *theid;
char *thedate;
struct tm checkin;
time_t now;
struct tm thetime;
double eseconds;
int days;

    
    time(&now);
    thetime = *localtime(&now);
    for (i = 0; i < cJSON_GetArraySize(item); i++){
        cJSON *subitem = cJSON_GetArrayItem(item, i);
        theid = cJSON_GetObjectItem(subitem, "uuid")->valuestring;
        thedate = cJSON_GetObjectItem(subitem, "lastCheckin")->valuestring;
        // 2017-03-28T00:51:25.000+0000
        if (strptime(thedate,"%Y-%m-%dT%H:%M:%S",&checkin) == NULL){
           printf("%s - %s\n",theid,thedate);
           printf("Date Time Format Error\n");
           exit(-1);
           }
       eseconds = difftime(now, mktime(&checkin));
       days = (int)eseconds / 60 / 60 / 24;
       if (days > 5){
          printf("Delete %s - %s - %d\n",theid,thedate,days);
          process_key(theid);
          }
    }
}

void get_list(char *owner_id)
{
char cmd[512];

    strcpy(cmd,"curl --silent -X GET -u ");
    strcat(cmd, user_id);
    strcat(cmd, ":");
    strcat(cmd, password_id);
    strcat(cmd," -k \"https://subscription.rhn.redhat.com/subscription/owners/");
    strcat(cmd,owner_id);
    strcat(cmd,"/consumers?include=id&include=uuid&include=name&include=lastCheckin&include=created\" > /tmp/cleanup.out");
    system(cmd);
}

void process_key(char *the_key)
{
char cmd[512];

    strcpy(cmd,"curl --silent -X DELETE -u ");
    strcat(cmd, user_id);
    strcat(cmd, ":");
    strcat(cmd, password_id);
    strcat(cmd, " -k \"https://subscription.rhn.redhat.com/subscription/consumers/\"");
    strcat(cmd, the_key);
    system(cmd);
}

char *readEntireFile( char *thefilename)
{
int fd;
FILE *thefile;
struct stat sb;
    
    fd = open(thefilename, O_RDONLY);

    int rc = fstat( fd, &sb );
    if ( rc < 0 )
    {
        return( NULL );
    }

    char *data = calloc( 1, sb.st_size + 1 );
    if ( data == NULL )
    {
        return( NULL );
    }

    ssize_t bytes_read = pread( fd, data, sb.st_size, ( off_t ) 0 );

    return( data );
}

