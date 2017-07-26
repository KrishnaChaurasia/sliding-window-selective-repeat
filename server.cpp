#include<iostream>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
using namespace std;

int GenerateRandom();
bool crc(char *);
#define PORT 5556

int serverSocket, check;
int client1Socket;

void closeAll(){
  close(serverSocket);
  close(client1Socket);
}

int strlength(char *msg){
  int i =0;
  while(msg[i]!='\0')
    i++;
  return i;
}


int main()
{
  
  srand(time(NULL));
  
  cout<<"\n";
  
  int counter = 0;
  struct sockaddr_in server, client1;
  char ack[10] = {'A','C','K',' ',' '};
  char nack[10]= {'N','A','C','K',' '};
  
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(serverSocket<0){
    cout<<"Error creating socket\n\n";
    closeAll();
    return 0;
  }
  cout<<"Socket Created\n";
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);
  
  check = bind(serverSocket, (struct sockaddr *) &server, sizeof(server));
  if(check<0){
    cout<<"Error binding the socket\n\n";
    closeAll();
    return 0;
  }
  cout<<"Binding successful\n";
  
  cout<<"Listening..............\n" <<endl;
  check = listen(serverSocket, 5);
  if(check<0){
    cout<<"Error listening\n\n";
    closeAll();
    return 0;
  }
  
  socklen_t len = sizeof(client1);
  client1Socket = accept(serverSocket, (struct sockaddr *) &client1, &len);
  if(client1Socket<0){
    cout<<"Error accepting the request\n\n";
    closeAll();
    return 0;
  }	
  cout<<"Connection estd\n";
  
  int ctr=0; int k; int n=1;
  char msg[123] = {' '};
  
  char msg1[40]={' '};
  char msg2[40]={' '};
  char msg3[40]={' '};
  
  char id;
  
  int sw=0;
  char id1;
  char id2;
  char id3;
  
  bool rep1=false;
  bool rep2=false;
  bool rep3=false;
  
  while(n){
    for(int i=0; i<sizeof(msg); i++) msg[i]='\0';
    n=recv(client1Socket, msg, 123, 0);
    
    //cout<<"Size : " <<msg[123] <<endl;
    if(strlen(msg)>100)
      msg[123]='\0';
    else
      msg[41]='\0';
    //cout<<"MSG :" <<strlength(msg) <<endl;
    if(strlen(msg)>100){
      msg[123]='\0';
      cout<<msg <<endl;
      int j=0, k=0;
      for(int i=0; i<40; i++) msg1[i]=msg[i]; msg1[40]='\0';
      for(int i=41; i<81; i++) msg2[j++]=msg[i]; msg2[40]='\0';
      for(int i=82; i<122; i++) msg3[k++]=msg[i]; msg3[40]='\0';
      
      id1=msg[40]; id2=msg[81]; id3=msg[122];
      
      //cout<<msg1<<endl;
      //cout<<msg2<<endl;
      //cout<<msg3<<endl;
      
      k=GenerateRandom();
      if(n){
	if(k==1 || k==0){
	  if(crc(msg1) && crc(msg2) && crc(msg3)){
	    sw=3; rep1=true; rep2=true; rep3=true;
	  }
	  else if(crc(msg1) && crc(msg2)){
	    rep1=true; rep2=true; sw=sw+2;
	  }
	  else if(crc(msg1) && crc(msg3)){
	    rep1=true; rep3=true; sw=sw+2;
	  }
	  else if(crc(msg2) && crc(msg3)){
	    rep2=true; rep3=true; sw=sw+2;
	  }
	  else if(crc(msg1)){
	    rep1=true; sw=sw+1;
	  }
	  else if(crc(msg2)){
	    rep2=true; sw=sw+1;
	  }	
	  else if(crc(msg3)){
	    rep3=true; sw=sw+1;						
	  }
	}	
	else{
	  cout<<"\n -> ACK/NACK dropped \n"<<endl;
	}
      }
    }
    else{
      msg[41]='\0';
      cout<<"MSG :" <<msg <<endl;
      char temp[40]={' '}; for(int i=0; i<40; i++) temp[i]=msg[i]; temp[40]='\0';      
      if(crc(temp)){
	sw++;
	if(msg[40]==id1) rep1=true;
	if(msg[40]==id2) rep2=true;
	if(msg[40]==id3) rep3=true;
      }
    }
    if(sw==3){
      if(id3=='0') id='1';
      if(id3=='1') id='2';
      if(id3=='2') id='3';
      if(id3=='3') id='4';
      if(id3=='4') id='5';
      if(id3=='5') id='0';
      ack[3]=id; ack[4]='\0';
      
      //sleep(1.1);
      check = send(client1Socket, ack, sizeof(ack), 0);
      if(check<0){
	cout<<"Error sending the message\n\n";
	closeAll();
	return 0;
      }
      sw=0; rep1=false; rep2=false; rep3=false;
      cout<<"\n -> ACK: "<<id <<" SENT \n"<<endl;
    }
    else if(rep1==false){
      nack[4]=id1; nack[5]='\0';
      check = send(client1Socket, nack, sizeof(nack), 0);
      if(check<0){
	cout<<"Error sending the message\n\n";
	closeAll();
	return 0;
      }
      cout<<"\n -> NACK: "<<id1 <<" SENT \n"<<endl;
    }
    else if(rep2==false){
      nack[4]=id2; nack[5]='\0';
      check = send(client1Socket, nack, sizeof(nack), 0);
      if(check<0){
	cout<<"Error sending the message\n\n";
	closeAll();
	return 0;
      }
      cout<<"\n -> NACK: "<<id2 <<" SENT \n"<<endl;
    }
    else if(rep3==false){
      nack[4]=id3; nack[5]='\0';
      check = send(client1Socket, nack, sizeof(nack), 0);
      if(check<0){
	cout<<"Error sending the message\n\n";
	closeAll();
	return 0;
      }
      cout<<"\n -> NACK: "<<id3 <<" SENT \n"<<endl;
    }	
    sleep(3);
  }
  
  closeAll();
  return 0;
}


int GenerateRandom()
{
  int a;
  a=rand()%2;
  return a;
}

bool crc(char *message){
  char *msg1=message;
  int ctr=0; int i=0;
  char a1, a2, a3, a4, a5, a6, a7, a8, a9;    
  
  // CRC-8 polynomial x^8+x^2+x+1
  char poly[9];
  poly[0]='1'; poly[1]='0'; poly[2]='0'; poly[3]='0'; poly[4]='0'; poly[5]='0'; poly[6]='1'; poly[7]='1'; poly[8]='1';
  
  //    int len=strlen(message);
  int len = 40;
  while(i+8<=len){
    int one=0; ctr=0;
    a1=msg1[i]; a2=msg1[i+1]; a3=msg1[i+2]; a4=msg1[i+3]; a5=msg1[i+4]; a6=msg1[i+5]; a7=msg1[i+6]; a8=msg1[i+7]; a9=msg1[i+8];
    
    if(msg1[i]==poly[0])
      msg1[i]='0';
    else msg1[i]='1';
    
    if(msg1[i+1]==poly[1])
      msg1[i+1]='0';
    else msg1[i+1]='1';
    
    if(msg1[i+2]==poly[2])
      msg1[i+2]='0';
    else msg1[i+2]='1';
    
    if(msg1[i+3]==poly[3])
      msg1[i+3]='0';
    else msg1[i+3]='1';
    
    if(msg1[i+4]==poly[4])
      msg1[i+4]='0';
    else msg1[i+4]='1';
    
    if(msg1[i+5]==poly[5])
      msg1[i+5]='0';
    else msg1[i+5]='1';
    
    if(msg1[i+6]==poly[6])
      msg1[i+6]='0';
    else msg1[i+6]='1';
    
    if(msg1[i+7]==poly[7])
      msg1[i+7]='0';
    else msg1[i+7]='1';
    
    if(msg1[i+8]==poly[8])
      msg1[i+8]='0';
    else msg1[i+8]='1';
    
    if (a1==poly[0]) ctr=1;           else {ctr=0; one=1;}
    if (a2==poly[1] && one==0) ctr=2; else one=1;
    if (a3==poly[2] && one==0) ctr=3; else one=1;
    if (a4==poly[3] && one==0) ctr=4; else one=1;
    if (a5==poly[4] && one==0) ctr=5; else one=1;
    if (a6==poly[5] && one==0) ctr=6; else one=1;
    if (a7==poly[6] && one==0) ctr=7; else one=1;
    if (a8==poly[7] && one==0) ctr=8; else one=1;
    if (a9==poly[8] && one==0) ctr=9; else one=1;
    i=i+ctr;
  }	
  int ct=0;
  for(int i=0; i<40; i++){
    if(msg1[i]=='0'){
      ct++;
    }
  }
  //cout<<"Mesg1 :"<<strlen(msg1)<<endl;
  if(ct==len){
    //cout<<"true";
    return true;
  }
return false;
}
