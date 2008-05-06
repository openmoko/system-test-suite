/* dm2_Network.c
 *
 * (C) 2007 by OpenMoko, Inc.
 * Written by Nod Huang <nod_huang@openmoko.com>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdarg.h>

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <unistd.h>

#include <string.h>

#include <time.h>

#include <dirent.h>

#include <resolv.h>

#include <arpa/inet.h>

#include <signal.h>

#include <getopt.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <sys/param.h>

#include <sys/ioctl.h>

#include <sys/socket.h>

#include <net/if.h>

#include <netinet/in.h>

#include <net/if_arp.h>

#include "dm2_Network.h"

/*
 * define Global Value.
 */

FILE *logfp;
FILE *ClientFP;

char buffer[MAXBUF + 1];

char *host = 0;

char *port = 0;

char *back = 0;

char *dirroot = 0;

char *logdir = 0;

unsigned char daemon_y_n =0;

int temp;

#define DEVICE_NAME "usb0"

#define prterrmsg(msg)  { perror(msg); abort(); }
#define wrterrmsg(msg)  { fputs(msg, logfp); fputs(strerror(errno), logfp); fflush(logfp); abort(); }
#define prtinfomsg(msg) { fputs(msg, stdout);  }
#define wrtinfomsg(msg) {  fputs(msg, logfp); fflush(logfp);}


/*Purpose: allocate memory to copy the content of d */

static void AllocateMemory(char **s, int l, char *d)
{

    *s = malloc(l + 1);

    memset(*s, 0, l + 1);

    memcpy(*s, d, l);

}

#if 0
/*Purpose: Find Test Item by keyword*/

static int Find_Test_Item(char *buf)
{ 
  int item;
  int find=-1;
 
 // printf("Item:%s\n",buf); 
  
   for(item=0;item<Test_Item;item++)
   {    
    if(!strcmp(Test_Item_Table[item].item.frame_text,buf))
     {
    //  printf("Match Item:%s\n",Test_Item_Table[item].item.frame_text);   
      find=item;
      return find;
      break;
     }
   }
 return find;
}
#endif
/*Purpose: Find out the path of the last directory .*/

static char *dir_up(char *dirpath)
{

    static char Path[MAXPATH];

    int len;

    strcpy(Path, dirpath);

    len = strlen(Path);

    if (len > 1 && Path[len - 1] == '/')

        len--;

    while (Path[len - 1] != '/' && len > 1)

        len--;

    Path[len] = 0;

    return Path;
}

/*Purpose: According to the content of the path,send it back to the client sock.
 *        If Path was an folder,list it; Else download it.
*/

static void GiveResponse(FILE * client_sock, char *Path)

{
    struct dirent *dirent;

    struct stat info;

    char Filename[MAXPATH];

    DIR *dir;

    int fd, len, ret;

    char *p, *realPath, *realFilename, *nport;

    /* get current directory and content */

    len = strlen(dirroot) + strlen(Path) + 1;

    realPath = malloc(len + 1);

    memset(realPath, 0 ,len + 1);

    sprintf(realPath, "%s/%s", dirroot, Path);

    /* get current port */

    len = strlen(port) + 1;

    nport = malloc(len + 1);

    memset(nport, 0, len + 1);

    sprintf(nport, ":%s", port);

    /* get current content data and check separate between file and folder   */

    if (stat(realPath, &info)) {

        fprintf(client_sock,

                "HTTP/1.1 200 OK\r\nServer: DAS \r\nConnection: close\r\n\r\n<html><head><title>%d - %s</title></head>"

                "<body><font size=+4>TestServer</font><br><hr width=\"100%%\"><br><center>"

                "<table border cols=3 width=\"100%%\">", errno,

                strerror(errno));

        fprintf(client_sock,

                "</table><font color=\"CC0000\" size=+2>Error：\n%s %s</font></body></html>",

                Path, strerror(errno));

        goto out;

    }

    /* request download file */

    if (S_ISREG(info.st_mode)) {

        fd = open(realPath, O_RDONLY);

        len = lseek(fd, 0, SEEK_END);

        p = (char *) malloc(len + 1);

        memset(p,0, len + 1);

        lseek(fd, 0, SEEK_SET);

        ret = read(fd, p, len);

        close(fd);

        fprintf(client_sock,

                "HTTP/1.1 200 OK\r\nServer: DAS\r\nConnection: keep-alive\r\nContent-type: application/*\r\nContent-Length:%d\r\n\r\n",

                len);

        fwrite(p, len, 1, client_sock);

        free(p);

    } else if (S_ISDIR(info.st_mode)) {

        /* request brose folder */

        dir = opendir(realPath);

        fprintf(client_sock,

                "HTTP/1.1 200 OK\r\nServer: DAS by ZhouLifa\r\nConnection: close\r\n\r\n<html><head><title>%s</title></head>"

                "<body><font size=+4>Test Server</font><br><hr width=\"100%%\"><br><center>"

                "<table border cols=3 width=\"100%%\">", Path);

        fprintf(client_sock,

                "<caption><font size=+3>Folder %s</font></caption>\n",

                Path);

        fprintf(client_sock,

                "<tr><td>Name</td><td>Size</td><td>Time</td></tr>\n");

        if (dir == 0) {

            fprintf(client_sock,

                    "</table><font color=\"CC0000\" size=+2>%s</font></body></html>",

                    strerror(errno));

            return;
        }
        /* scan the folder */

        while ((dirent = readdir(dir)) != 0) {

            if (strcmp(Path, "/") == 0)

                sprintf(Filename, "/%s", dirent->d_name);

            else

                sprintf(Filename, "%s/%s", Path, dirent->d_name);

            fprintf(client_sock, "<tr>");

            len = strlen(dirroot) + strlen(Filename) + 1;

            realFilename = malloc(len + 1);

            memset(realFilename, 0,len + 1);

            sprintf(realFilename, "%s/%s", dirroot, Filename);

            if (stat(realFilename, &info) == 0) {

                if (strcmp(dirent->d_name, "..") == 0)

                    fprintf(client_sock,

                            "<td><a href=\"http://%s%s%s\">(parent)</a></td>",

                            host, atoi(port) == 80 ? "" : nport,

                            dir_up(Path));

                else

                    fprintf(client_sock,

                            "<td><a href=\"http://%s%s%s\">%s</a></td>",

                            host, atoi(port) == 80 ? "" : nport, Filename,

                            dirent->d_name);

                if (S_ISDIR(info.st_mode))

                    fprintf(client_sock, "<td>Folder</td>");

                else if (S_ISREG(info.st_mode))

                    fprintf(client_sock, "<td>%d</td>", (int) info.st_size);

                else if (S_ISLNK(info.st_mode))

                    fprintf(client_sock, "<td>Link</td>");

                else if (S_ISCHR(info.st_mode))

                    fprintf(client_sock, "<td>Character</td>");

                else if (S_ISBLK(info.st_mode))

                    fprintf(client_sock, "<td>Block</td>");

                else if (S_ISFIFO(info.st_mode))

                    fprintf(client_sock, "<td>FIFO</td>");

                else if (S_ISSOCK(info.st_mode))

                    fprintf(client_sock, "<td>Socket</td>");

                else

                    fprintf(client_sock, "<td>(Unknow)</td>");

                fprintf(client_sock, "<td>%s</td>", ctime(&info.st_ctime));

            }

            fprintf(client_sock, "</tr>\n");

            free(realFilename);

        }

        fprintf(client_sock, "</table></center></body></html>");

    } else {

        /* not folder and file */

        fprintf(client_sock,

                "HTTP/1.1 200 OK\r\nServer: DAS\r\nConnection: close\r\n\r\n<html><head><title>permission denied</title></head>"

                "<body><font size=+4>Test Server</font><br><hr width=\"100%%\"><br><center>"

                "<table border cols=3 width=\"100%%\">");

        fprintf(client_sock,

                "</table><font color=\"CC0000\" size=+2>Resources'%s'blocked，Ask administrator！</font></body></html>",

                Path);
    }

  out:

    free(realPath);

    free(nport);

}

static int excute_request(int new_socket)
{
  int len;

//  printf("Socket_id: %d\n",new_socket); 

        if (!fork()) {

            memset(buffer,0, MAXBUF + 1);

            if ((len = recv(new_socket, buffer, MAXBUF, 0)) > 0) {

                FILE *ClientFP = fdopen(new_socket, "w");

                if (ClientFP == NULL) {

                    if (!daemon_y_n) {

                        prterrmsg("fdopen()");

                    } else {

                        prterrmsg("fdopen()");

               					     }

            } else {
#if 0                   			 
                 //parser recived index then start testing
                 temp = Find_Test_Item(buffer);

		   //remote control
		   if(temp>=0)
		   {	
		    Test_index=temp;

                   result[temp]=Undefine;
                  // printf("Testing Item: %d\n",i);                    	
 	     	     g_timeout_add(100,(GSourceFunc) update_window_status,(gpointer)temp);	 	
                   g_timeout_add(100,(GSourceFunc) update_window_frame,(gpointer)temp);		       		
	           
		     //test content
       	     if(Test_Item_Table[temp].item.test_func!=NULL){             

      		        g_timeout_add(Segment_Time,(GSourceFunc) Test_Item_Table	 			
									[temp].item.test_func,NULL);        
		        //   void (*fp) (void) =  Test_Item_Table[i].item.test_func;		
		        //	  fp();		
	               }               
       	
		   //send result back	 
	   	   if(temp!=Test_Item-1){
		        do{
			//     g_print("Result[%d]:%d\n",temp,result[temp]);
	    	        }while(result[temp]==Undefine);    
   
                    fprintf(ClientFP,"Test Item:%d result:%s\n",temp,result[temp]?"Pass":"Fail");    
               				      }

                  else{  	

    		        gchar buf[MAX_Buffer];

                       memset(buf,0,sizeof(buf));
                       read_log(Log_Path,buf);
                       fprintf(ClientFP,"%s\n",buf);    
		     				 }
		  }
		  //web browser		
		  else{
#endif

                    char Req[MAXPATH + 1] = "";

                    sscanf(buffer, "GET %s HTTP", Req);

                    memset(buffer,0, MAXBUF + 1);

                    sprintf(buffer, "Request: \"%s\"\n", Req);
						

                    if (!daemon_y_n) {

                        prtinfomsg(buffer);

                   } else {

                        wrtinfomsg(buffer);

         				             }

                    /* Give response to Client side */

                    GiveResponse(ClientFP, Req);                    

                  // }

		fclose(ClientFP);
	      }
            }

            exit(0);

        }

        close(new_socket);   

  return 0;
}

static void get_ip(void)
{

   int addrlen;    
 
   int sock;

   struct sockaddr_in sin;

   struct ifreq ifr;
	
   sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock == -1){

	   if (!daemon_y_n) {

            prterrmsg("socket()");

           } 
	   else {

            wrterrmsg("socket()");

            }	

	}	

	strncpy(ifr.ifr_name, DEVICE_NAME, IFNAMSIZ);

	ifr.ifr_name[IFNAMSIZ - 1] = 0;	

	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0){

	   if (!daemon_y_n) {

            prterrmsg("ioctl()");

           } 
	   else {

            wrterrmsg("ioctl()");

           }
	}

	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
       
	//fprintf(stdout, "%s: %s\n",DEVICE_NAME, inet_ntoa(sin.sin_addr));

        addrlen = strlen(inet_ntoa(sin.sin_addr));

        if ( addrlen >0){                

          AllocateMemory(&host, addrlen, inet_ntoa(sin.sin_addr)); 	
                                       
          }
         else {
               
           addrlen = strlen(DEFAULTIP);

           AllocateMemory(&host, addrlen, DEFAULTIP);
 			
          }	
}

int Start_Server(void)
{

    struct sockaddr_in addr;

    int sock_fd, addrlen;   

    /* get IP 、Port Number、Back Number */

    if (!host) {
	
	get_ip(); 
     }

    if (!port) {

        addrlen = strlen(DEFAULTPORT);

        AllocateMemory(&port, addrlen, DEFAULTPORT);

    }

    if (!dirroot) {

        addrlen = strlen(DEFAULTDIR);

        AllocateMemory(&dirroot, addrlen, DEFAULTDIR);

    }

    if (!back) {

        addrlen = strlen(DEFAULTBACK);

        AllocateMemory(&back, addrlen, DEFAULTBACK);

    }

    printf

        ("host=%s port=%s back=%s dirroot=%s This is %s background mode(ProcessID: %d)\n",

         host, port, back,dirroot, daemon_y_n?"":"Not", getpid());



    /* fork() twice to background mode */

    if (daemon_y_n) {

        if (fork())

            exit(0);

        if (fork())

            exit(0);

        close(0), close(1), close(2);

      //  logfp = fopen(logdir, "a+");

      //  if (!logfp)

        //    exit(0);

    }

    /* avoid deadlock */

    signal(SIGCHLD, SIG_IGN);

    /* create a socket */

    if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {

        if (!daemon_y_n) {

            prterrmsg("socket()");

        } else {

            wrterrmsg("socket()");

        }

    }

    /* set socket configuration */

    addrlen = 1;

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &addrlen,

               sizeof(addrlen));



    addr.sin_family = AF_INET;

    addr.sin_port = htons(atoi(port));

    addr.sin_addr.s_addr = inet_addr(host);

    addrlen = sizeof(struct sockaddr_in);

    /* bind IP and Port */

    if (bind(sock_fd, (struct sockaddr *) &addr, addrlen) < 0) {

        if (!daemon_y_n) {

            prterrmsg("bind()");

        } else {

            wrterrmsg("bind()");

        }
	return TRUE;
    }

    /* start listen */

    if (listen(sock_fd, atoi(back)) < 0) {

        if (!daemon_y_n) {

            prterrmsg("listen()");

        } else {

            wrterrmsg("listen()");

        }

    }
  
    while (1) 
    {        
        int new_fd;

        addrlen = sizeof(struct sockaddr_in);

        /* accept request */

        new_fd = accept(sock_fd, (struct sockaddr *) &addr,
			(socklen_t *) &addrlen);

        if (new_fd < 0) {

            if (!daemon_y_n) {

                prterrmsg("accept()");

            } else {

                wrterrmsg("accept()");

            }

            break;

        }

        memset(buffer,0, MAXBUF + 1);

        sprintf(buffer, "Connect From: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

         if (!daemon_y_n) {

            prtinfomsg(buffer);

         } else {

            wrtinfomsg(buffer);

          }

	// gtk_widget_set_sensitive(status_view, TRUE);

        /*process the new client */
        excute_request(new_fd);

	// gtk_widget_set_sensitive(status_view, !Sequence_Test);

      //  g_thread_create((GThreadFunc)process_request,(gpointer) new_fd,FALSE,NULL); 

    }
    close(sock_fd);

    return FALSE;

}
