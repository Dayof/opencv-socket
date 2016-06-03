#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>

#include<sys/socket.h>	//socket
#include<sys/types.h>
#include<netinet/in.h>

using namespace std;
using namespace cv;

#define PORT 7200

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main()
{
  int sockfd, newsockfd, portno, n, imgSize, bytes=0;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  Mat img;

  sockfd=socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0) error("ERROR opening socket");

  bzero((char*)&serv_addr, sizeof(serv_addr));
  portno = PORT;

  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=INADDR_ANY;
  serv_addr.sin_port=htons(portno);

  if(bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof(serv_addr))<0) error("ERROR on binding");

  listen(sockfd,5);
  clilen=sizeof(cli_addr);

  newsockfd=accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if(newsockfd<0) error("ERROR on accept");

  // bzero(buffer,1024);
  // n = read(newsockfd, buffer, 1023);
  // if(n<0) error("ERROR reading from socket");
  //printf("Here is the message: %s\n", buffer);

  // n=write(newsockfd, "I got your message", 18);
  // if(n<0) error("ERROR writing to socket");

  img = Mat::zeros(768, 769, CV_8UC3);

  imgSize = img.total()*img.elemSize();
  uchar sockData[imgSize];

  for(int i=0;i<imgSize;i+=bytes)
    if ((bytes=recv(newsockfd, sockData+i, imgSize-i,0))==-1) error("recv failed");

  int ptr=0;

  for(int i=0;i<img.rows;++i)
    for(int j=0;j<img.cols;++j)
    {
      img.at<Vec3b>(i,j) = Vec3b(sockData[ptr+0],sockData[ptr+1],sockData[ptr+2]);
      ptr=ptr+3;
    }

  imwrite("test.jpg", img);
  namedWindow( "Server", CV_WINDOW_AUTOSIZE );// Create a window for display.
  imshow( "Server", img );
  waitKey(0);

  close(newsockfd);
  close(sockfd);

  return 0;
}
