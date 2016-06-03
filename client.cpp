#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>

#define LOCALHOST "127.0.0.1"
#define PORT 7200

using namespace std;
using namespace cv;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main()
{
  int sockfd, portno, n, imgSize;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[256];

  portno = PORT;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) error("ERROR opening socket");

  server = gethostbyname(LOCALHOST);

  if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
       (char *)&serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
      error("ERROR connecting");

  Mat frame = imread("image.jpg", CV_LOAD_IMAGE_COLOR);
  if(!frame.data)
  {
      cout <<  "Could not open or find the image" << endl ;
      return -1;
  }

  frame = (frame.reshape(0,1));

  imgSize=frame.total()*frame.elemSize();

  n = send(sockfd, frame.data, imgSize, 0);
  if (n < 0) error("ERROR writing to socket");

  close(sockfd);

  return 0;
}
