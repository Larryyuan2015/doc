
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <iostream>


#define SERV_PORT   3001
#define GPIO_NUM 2
int digitalNum[GPIO_NUM];
int status[GPIO_NUM];
int lastStatus[GPIO_NUM];
void init(){
	digitalNum[0] = 7;
	digitalNum[1] = 11;
    wiringPiSetup() ;
	for(int i = 0 ; i < GPIO_NUM ; ++i){
		pinMode (digitalNum[i], OUTPUT) ;
	}
}

int main()
{
	init();
	
	/* sock_fd --- socketæä»¶æè¿°ç¬¦ åå»ºudpå¥æ¥å­*/
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_fd < 0)
	{
		perror("socket");
		exit(1);
	}

	/* å°å¥æ¥å­åIPãç«¯å£ç»å® */
	struct sockaddr_in addr_serv;
	int len;
	memset(&addr_serv, 0, sizeof(struct sockaddr_in)); //æ¯ä¸ªå­èé½ç¨0å¡«å
	addr_serv.sin_family = AF_INET; //ä½¿ç¨IPV4å°å
	addr_serv.sin_port = htons(SERV_PORT); //ç«¯å£
	/* INADDR_ANYè¡¨ç¤ºä¸ç®¡æ¯åªä¸ªç½å¡æ¥æ¶å°æ°æ®ï¼åªè¦ç®çç«¯å£æ¯SERV_PORTï¼å°±ä¼è¢«è¯¥åºç¨ç¨åºæ¥æ¶å° */
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);  //èªå¨è·åIPå°å
	len = sizeof(addr_serv);

	/* ç»å®socket */
	if(bind(sock_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0)
	{
		perror("bind error:");
		exit(1);
	}
  
	//init camera
	if(access("/dev/video0",0)){
		printf("Please check your camera connection,then try again.\n");
		exit(0);
	}

	char *i2c = "i2cset -y 1 0x70 0x00 0x01";
	system(i2c);
	digitalWrite(7,0);
	digitalWrite(11,0);

	char *mv1 = "mv file";
	char *mv2 = "mv file";
	char *cmd = "command";
		
	int recv_num;
	int send_num;
	char send_buf[20] = "i am server!";
	char recv_buf[20];
	struct sockaddr_in addr_client;

	while(1)
	{
		printf("server wait:\n");

		recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);

		if(recv_num < 0)
		{
			perror("recvfrom error:");
			exit(1);
		}

		recv_buf[recv_num] = '\0';
		printf("server receive %d bytes: %s\n", recv_num, recv_buf);
		
		if(strcmp(recv_buf,"1") == 0)
		{//rear camera A
			char *i2c = "i2cset -y 1 0x70 0x00 0x01";
			system(i2c);
			digitalWrite(11,0);
			digitalWrite(7,0);
			
			memset(send_buf,0,20*sizeof(char));
			strcpy(send_buf,"CA");		
			
		}else if( strcmp(recv_buf,"0") == 0 )
		{//front camera B
			i2c = "i2cset -y 1 0x70 0x00 0x02";
			system(i2c);
			digitalWrite(11,0);
			digitalWrite(7,1);
			
			memset(send_buf,0,20*sizeof(char));
			strcpy(send_buf,"CB");	
		}else if( strcmp(recv_buf,"3") == 0 )
		{//start mjpg streamer
			cmd = "bash /home/pi/gt400/start_mjpg_streamer.sh";
			system(cmd);
			
			memset(send_buf,0,20*sizeof(char));
			strcpy(send_buf,"ST");	
		}else if( strcmp(recv_buf,"4") == 0 )
		{//stop mjpg streamer
			cmd = "ps -ef | grep mjpg_streamer | grep -v grep | awk '{print $2}' | xargs kill -9";
			system(cmd);
			
			memset(send_buf,0,20*sizeof(char));
			strcpy(send_buf,"CL");	
		}else if( strcmp(recv_buf,"2") == 0 )
		{//set wifi 2.4G band
			if(access("/home/pi/gt400/5g_ap.conf",F_OK) != 0)
			{ //5g_ap.conf file doesnot exist, that means 5G conf is current running.
				mv1 = "sudo mv /etc/create_ap.conf /home/pi/gt400/5g_ap.conf";
				system(mv1);
				
				mv2= "sudo mv /home/pi/gt400/2g_ap.conf /etc/create_ap.conf";
				system(mv2);
				
				sleep(1);
				memset(send_buf,0,20*sizeof(char));
				strcpy(send_buf,"2.4G");	
				sleep(2);
				system("sudo reboot");
			}else{
				memset(send_buf,0,20*sizeof(char));
				strcpy(send_buf,"2GE");
			}
			
		}else if( strcmp(recv_buf,"5") == 0 )
		{//set wifi 5G band
			if(access("/home/pi/gt400/5g_ap.conf",F_OK) == 0)
			{ //5g_ap.conf file exists, that means 2.4G conf is current running.
				mv1 = "sudo mv /etc/create_ap.conf /home/pi/gt400/2g_ap.conf";
				system(mv1);
				
				mv2= "sudo mv /home/pi/gt400/5g_ap.conf /etc/create_ap.conf";
				system(mv2);

				sleep(1);
				memset(send_buf,0,20*sizeof(char));
				strcpy(send_buf,"5G");
				sleep(2);
				system("sudo reboot");
			}else{
				memset(send_buf,0,20*sizeof(char));
				strcpy(send_buf,"5GE");
			}
		}
		
		send_num = sendto(sock_fd, send_buf, 18, 0, (struct sockaddr *)&addr_client, len);

		if(send_num < 0)
		{
		  perror("sendto error:");
		  exit(1);
		}
		
		sleep(1);
	}

  close(sock_fd);

  return 0;
}
