#include "euble_interface.h"

#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include <fcntl.h> 

#include<arpa/inet.h>
#include<sys/socket.h>
 

#define SERVER "192.168.1.122"
#define BUFLEN 512    //Max length of buffer
#define PORT   4567   //The port on which to send data

int s = -1;
struct sockaddr_in si_other;
int slen=sizeof(si_other);
static unsigned int tid = 200001;


void die(char *s)
{
  perror(s);
  exit(1);
}
  

void euble_init(void* env)
{
  LOG("init x86 udp test env\n");
}
 
int euble_open(const char* device_name)
{
    int i;
    char buf[BUFLEN];
    char message[BUFLEN];

    DBG("call bleOpen %s .\n", device_name); 
    if (s>0) {
        DBG("socket has been opend:ble - %s.\n", device_name);     
        return s;
    }
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    if (-1 == fcntl(s, F_SETFL, O_NONBLOCK))
    {
        die("socket");
    }
  
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    //printf("socket buff-------%d\n", utl_sock_get_rcvbuf(s));

    return s;
}



int euble_close(int fd)
{
    DBG("fd=%d call bleClose\n", fd);
    if (s > 0) {
        close(s);
        s = -1;
    }
    return 0;
}



int euble_reset(int fd)
{
    const char* cmd = "0x0000";
#if 0
    if(euble_write(fd, cmd,strlen(cmd)) < 0){
        DBG("fd=%d call bleReset fail.\n", fd);
    }
#endif
    tid = 200001;   //reset the tid
    euble_write(fd, cmd,strlen(cmd));
    return 0;
}


int euble_write(int fd, const void* mybuf, size_t count)
{
    unsigned char buf[1024] = { 0};
    unsigned int i;
    
    if (s<0) {
        DBG("fd=%d sock not connected", fd);
        return -1;
    }
    sprintf(buf, "%d:%s",tid,mybuf);
    count += 7;

    //for(i=0;i<1;i++)
    {
        //send the message
        si_other.sin_port = htons(PORT);
        if (sendto(s, buf, count , 0 , (struct sockaddr *) &si_other, slen)==-1) {
            DBG("fd=%d send fail %s\n", fd, buf);      
            return -1;
        }else{
            //printf("Write Data:Tid-%d Len-%d:%s\n", tid, count-7, mybuf);
        }
    }
    DBG("**** Write Data:Tid-%d Len-%d : %s\n", tid, count-7, mybuf);
    tid++;

    return 0;
}


int euble_read(int fd, void* buf, size_t count, int blockms)
{    
    struct timeval tv;
    fd_set readfds;
    int sz = 0;
    //static unsigned int lastTid = 1;
    unsigned int curTid =0;
    unsigned char bufTmp[1024];

    if (s<0) {
        DBG("fd=%d sock not connected", fd);
        return -1;
    }

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(s, &readfds);
    memset(buf,'\0', count);
    memset(bufTmp,'\0', 1024);

    if (select(s+1,&readfds,NULL, NULL, &tv) > 0)
    {
        //try to receive some data, this is a blocking call
        sz = recvfrom(s, bufTmp, count, 0, (struct sockaddr *) &si_other, &slen);
        if (sz < 0) {
            //DBG("fd=%d recv fail.\n", fd);
            return 0;
        }else if (sz > 0){
            //printf("**** Read Data: %s\n", bufTmp);
            sscanf(bufTmp, "%d:", &curTid);
            if(curTid != tid-1){
                memset(bufTmp,'\0', count);
                return 0;
            }else{
                sz -=7;
                //lastTid = curTid;
                memcpy(buf, &(bufTmp[7]),sz);
            }
            DBG("**** Read Data:Tid-%d Len-%d : %s\n", curTid, sz, buf);
        }
    }
    return sz;
}

