#include<iostream>
#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/types.h>
#include<netdb.h>
#include<stdlib.h>
using namespace std;

char msg[40]={' '};

char msg1[40]={' '}; 
char msg2[40]={' '};
char msg3[40]={' '};

int GenerateRandom();
void computeCrcBits(char []);
void corruptMessage();
int findCtr(char, char);

char frames[123]={' '};

#define PORT 5556

int main(int argc, char *argv[])
{

  srand(time(NULL));
  
  int clientSocket, check;
  struct sockaddr_in server;
  struct hostent *hp;
  //char msg[32]={' '};
  char msgRecv[10]={' '};
  
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(clientSocket<0){
    cout<<"Error creating the socket\n";
    return 0;
  }
  
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  
  hp = gethostbyname(argv[1]);
  if(hp<0){
    cout<<"Server address/ip missing\n\n";
    return 0;
  }
  
  bcopy ((char *)hp->h_addr, (char *)&server.sin_addr.s_addr, hp->h_length);
  
  check = connect(clientSocket, (struct sockaddr *) &server, sizeof(server));
  if(check<0){
    cout<<"Error connecting\n\n";
    return 0;
  }
  cout<<"Connection estd\n\n";
  
  int from=open("input.txt", O_RDONLY);
  if(from<0){
    cout<<"Error opening the file\n\n";
    return 0;
  }
  
  int nr=1; int ctr=0; int k;
  
  //nr=read(from, msg, 33);
  //for(int i=0; i<32; i++) cout<<msg[i]; cout<<endl;
  //computeCrcBits();
  //corruptMessage();
  
  
  ctr=3;
  char lastIndex='0';
  char tmsg[41]={' '};
  char id='0';
  char rid;
  
  frames[40]='0'; frames[81]='1'; frames[122]='2';
  
  while(nr!=0){
    if(ctr>0){
      if(rid=='0') { frames[40]='0'; frames[81]='1'; frames[122]='2'; id='0'; }
      if(rid=='1') { frames[40]='1'; frames[81]='2'; frames[122]='3'; id='1'; }
      if(rid=='2') { frames[40]='2'; frames[81]='3'; frames[122]='4'; id='2'; }	
      if(rid=='3') { frames[40]='3'; frames[81]='4'; frames[122]='5'; id='3'; }
      if(rid=='4') { frames[40]='4'; frames[81]='5'; frames[122]='0'; id='4'; }
      if(rid=='5') { frames[40]='5'; frames[81]='0'; frames[122]='1'; id='5'; }
      
      if(ctr==3){
	for(int i=0; i<sizeof(msg); i++){ msg1[i]=' '; msg2[i]=' '; msg3[i]=' '; }
	for(int i=0; i<sizeof(msgRecv); i++) msgRecv[i]=' '; msgRecv[sizeof(msgRecv)]='\0';
	nr=read(from, msg1, 33); 
	nr=read(from, msg2, 33);
	nr=read(from, msg3, 33);
	
	computeCrcBits(msg1);
	for(int i=0; i<40; i++) msg1[i]=msg[i];	    
	
	computeCrcBits(msg2);
	for(int i=0; i<40; i++) msg2[i]=msg[i];
	
	computeCrcBits(msg3);
	for(int i=0; i<40; i++) msg3[i]=msg[i];
  	
	int j=0,k=0;
	for(int i=0; i<40; i++) frames[i]=msg1[i]; 
	for(int i=41; i<81; i++) frames[i]=msg2[j++]; 
	for(int i=82; i<122; i++) frames[i]=msg3[k++]; 
	corruptMessage();
	ctr=0;
      }
      else if(ctr==2){
	for(int i=0; i<sizeof(msg); i++){ msg1[i]=' '; msg2[i]=' '; }
	for(int i=0; i<sizeof(msgRecv); i++) msgRecv[i]=' '; msgRecv[sizeof(msgRecv)]='\0';
	for(int i=0; i<40; i++) msg1[i]=msg3[i];	  		 
	nr=read(from, msg2, 33);
	nr=read(from, msg3, 33);
	
	computeCrcBits(msg2);
	for(int i=0; i<40; i++) msg2[i]=msg[i];
	
	computeCrcBits(msg3);
	for(int i=0; i<40; i++) msg3[i]=msg[i];
  	
	int j=0,k=0;
	for(int i=0; i<40; i++) frames[i]=msg1[i]; 
	for(int i=41; i<81; i++) frames[i]=msg2[j++];
	for(int i=82; i<122; i++) frames[i]=msg3[k++];
	corruptMessage();
	ctr=0;
      }
      else if(ctr==1){
	for(int i=0; i<sizeof(msg); i++){ msg1[i]=' ';}
	for(int i=0; i<sizeof(msgRecv); i++) msgRecv[i]=' '; msgRecv[sizeof(msgRecv)]='\0';
	for(int i=0; i<40; i++) msg1[i]=msg2[i];
	for(int i=0; i<40; i++) msg2[i]=msg3[i];
	nr=read(from, msg3, 33);
	
	computeCrcBits(msg3);
	for(int i=0; i<40; i++) msg3[i]=msg[i];
  	
	int j=0,k=0;
	for(int i=0; i<40; i++) frames[i]=msg1[i]; 
	for(int i=41; i<81; i++) frames[i]=msg2[j++]; 
	for(int i=82; i<122; i++) frames[i]=msg3[k++]; 
	corruptMessage();
	ctr=0;
      }
    }
    
   
    k=GenerateRandom();
    if(k==2){
      cout<<"\n -> Frame dropped\n\n";
    }
    else if(k==1 || k==0){
      if(nr!=0){
	check = send(clientSocket, frames, sizeof(frames), 0);
	cout<<frames<<endl;
	//cout<<"M:"<<msg <<"\n";
	//cout<<"sent\n";
	
	while(1){
	  
	  for(int i=0; i<sizeof(msgRecv); i++) msgRecv[i]=' '; 
	  msgRecv[sizeof(msgRecv)]='\0';
	  
	  fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	  sleep(1);
	  check = recv(clientSocket, msgRecv, sizeof(msgRecv), MSG_DONTWAIT);
	  
	  //rid=msgRecv[4];
	  if(msgRecv[0]=='A'){
	    rid=msgRecv[3];
	    ctr = 3;
	    //cout<<"\n -> ACK: " <<rid <<" recvd ";
	    break;
	  }
	  else if(msgRecv[0]=='N'){
	    rid=msgRecv[4];	
	    if(frames[40]==rid){
	      for(int i=0; i<40; i++) tmsg[i]=msg1[i]; 
	    }
	    else if(frames[81]==rid){
	      for(int i=0; i<40; i++) tmsg[i]=msg2[i]; 	
	    }
	    else if(frames[122]==rid){
	      for(int i=0; i<40; i++) tmsg[i]=msg3[i]; 
	    }
	    tmsg[40]=rid;
	    tmsg[41]='\0';
	    for(int i=0; i<=40; i++) frames[i]=tmsg[i]; frames[41]='\0';
	    //cout<<"\n -> NACK: "<<rid <<" recvd ";
	    break;
	  }
	  else{
	    cout<<"\n -> Dropped \n\n";
	    break;
	  }
	}
	cout<<"\n\n";
	cout<<"\n -> Reply: "<<msgRecv <<"\n";
      }
      else break;
      
    }
    sleep(3);
  }
  return 0;
}

int findCtr(char a, char b){
  if(int(a)-int(b)>0)
    return int(a)-int(b);
  else return int(b)-int(a);
}

void corruptMessage()
{
  int n=GenerateRandom();
  if(n==0){
    int check[123]={0}; check[40]=1; check[81]=1; check[122]=1;
    for(int i=0; i<40; i++) check[i]=1;
    for(int i=73; i<=80; i++) check[i]=1;
    for(int i=114; i<=121; i++) check[i]=1;
    int noOfBitsToCorrupt = 4;//rand()%(30-6+1)  + 6;
    cout<<"\tCorrupted: " <<noOfBitsToCorrupt <<"\n";
    while(noOfBitsToCorrupt>0){
      int corruptThisBit = rand()%123; 
      if(check[corruptThisBit]==0){
	check[corruptThisBit]=1;
	if(frames[corruptThisBit]=='1')
	  frames[corruptThisBit]='0';
	else frames[corruptThisBit]='1';
	noOfBitsToCorrupt--;
      }
    }
  }    
  else cout<<"Not Corrupted \n";
}

int GenerateRandom()
{
  int a;
  a=rand()%2;
  return a;
}


void computeCrcBits(char message[]){
  for(int i=0; i<40; i++) msg[i]=message[i];
  // CRC-8 polynomial x^8+x^2+x+1
  char poly[9]={' '};
  poly[0]='1'; poly[1]='0'; poly[2]='0'; poly[3]='0'; poly[4]='0'; poly[5]='0'; poly[6]='1'; poly[7]='1'; poly[8]='1';
  
  char msgLocal[40]={' '}; // 32+8
  
  for(int i=0; i<=31; i++){
    msgLocal[i] = msg[i];
  }
  
  for(int i=32; i<=39; i++)
    msgLocal[i]='0';
  
  int ctr=0, ct=0, i=0, found=0;
  
  while( ctr <= (40-9) ){
    i=0; ct=ctr; found=0; 
    while(i!=9){
      if(msgLocal[ct]==poly[i]){
	msgLocal[ct]='0';
	i++; ct++;
	
	if(found==0 && i==9)
	  ctr=ctr+i;
      }	 
      else{
	msgLocal[ct]='1';
	if(found==0){
	  found++;
	  ctr=ctr+i; //cout<<"CTR : " <<ctr <<endl;
	}
	i++; ct++;
      }
    }
  }
  for(int i=32; i<=39; i++)
    msg[i]=msgLocal[i];
  //cout<<msg <<endl;
}
