#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>

#include "D8MCapture.h"
#include "hps_0.h"
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
using namespace cv;
using namespace std;

#ifndef CAPTURE_RAM_DEVICE
#define CAPTURE_RAM_DEVICE "/dev/f2h-dma-memory"
#define PORT 8080
#endif /* ifndef CAPTURE_RAM_DEVICE */


string removeSpaces(string input);

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

int main()
{

  int server_fd, new_socket, valread;

  	struct sockaddr_in address;
  	int opt = 1;
  	int addrlen = sizeof(address);
  	//char buffer[1024] = {0};


  	// Creating socket file descriptor
  	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  	{
  		perror("socket failed");
  		exit(EXIT_FAILURE);
  	}

  	// Forcefully attaching socket to the port 8080
  	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
  												&opt, sizeof(opt)))
  	{
  		perror("setsockopt");
  		exit(EXIT_FAILURE);
  	}
  	address.sin_family = AF_INET;
   inet_aton("192.168.1.247", &address.sin_addr);
  	address.sin_port = htons( PORT );

  	// Forcefully attaching socket to the port 8080
  	if (bind(server_fd, (struct sockaddr *)&address,
  								sizeof(address))<0)
  	{
  		perror("bind failed");
  		exit(EXIT_FAILURE);
  	}
  	if (listen(server_fd, 3) < 0)
  	{
  		perror("listen");
  		exit(EXIT_FAILURE);
  	}
  	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
  					(socklen_t*)&addrlen))<0)
  	{
  		perror("accept");
  		exit(EXIT_FAILURE);
  	}


    // Close server_fdsocket
      close(server_fd);
  	int bufsize = 2000000;
  	int sizebufsize= 50;
  	int codebufsize = 40;

      double brightness=1;
      double contrast =1;
      // While loop: accept and echo message back to client
      char buf[bufsize];
      char sizebuf[sizebufsize];
      char codebuf[codebufsize];
  	string code;

      Mat src, overlay;
      D8MCapture *cap = new D8MCapture(TV_DECODER_TERASIC_STREAM_CAPTURE_BASE, CAPTURE_RAM_DEVICE);
      if (!cap->isOpened()) {
          return -1;
      }

      char str[100];
      static struct timeval last_time;
      struct timeval current_time;
      static float last_fps;
      float t;
      float fps;


      namedWindow("camera");
      while (1) {
          fd_set rfds;
          struct timeval tv;
          tv.tv_sec=0;
          tv.tv_usec=500;

          FD_ZERO(&rfds);
          FD_SET(new_socket, &rfds);


          memset(buf, 0, bufsize);
          memset(sizebuf, 0, sizebufsize);
  		memset(codebuf,0, codebufsize);

          int status = select(new_socket+1,&rfds, NULL,NULL, &tv);
          if(status == -1){
              break;
          }
          if(status >0){
              // Wait for client to send data
  		int codebytesrec = recv(new_socket, codebuf, codebufsize, 0);
  		int butesrecsizebuf ;
  		int bytesReceived;
  		code = string(codebuf,0 ,codebufsize);
  		code = removeSpaces(code);
  		if(codebytesrec == -1)
  		{
  			cerr << "Error in recv(). Quitting" << endl;
              break;
  		}
  		if (codebytesrec == 0)
          {
              cout << "Client disconnected " << endl;
              break;
  		}
  		
  		if(code == "image" ){
              recv(new_socket, sizebuf, sizebufsize, 0);
  			bytesReceived = recv(new_socket, buf, atoi(sizebuf), 0);
  			int s = atoi(sizebuf);
              if(s>0){
                  //cout<< "size  "<< s << " revbytes"<< bytesReceived<<endl;
  			ofstream myfile;
  			if(bytesReceived>0){
  				myfile.open("overlay.bmp", ios::binary | ios::out);
  				myfile.write(buf, bytesReceived);
  			}
  			s = s-bytesReceived;
  			if(s<=0){
  				cout<< "image saved"<<endl;
  				myfile.close();
                  overlay = imread("overlay.bmp");
                  //break;
  			}
  			while (s>0){
  				bytesReceived = recv(new_socket, buf, atoi(sizebuf), 0);
  				myfile.write(buf, bytesReceived);
  				s = s-bytesReceived;

  				send(new_socket, buf, bytesReceived + 1, 0);
  				if(s<=0){
  					myfile.close();
  					cout<< "image saved"<<endl;
                      overlay = imread("overlay.bmp");
                      //break;
  				}
  			}
              }

  		}else if(code =="b/c"){
              bytesReceived = recv(new_socket, buf, 7, 0);
              brightness = atof(buf);
              //cout<<"first recv" <<buf<< endl;
              bytesReceived = recv(new_socket, buf, 7, 0);
              //cout<<"second recv" <<buf<< endl;
              contrast = atof(buf);
              //cout<< "B: " << brightness << "C: "<< contrast<<endl;
  			//cout<< "msg :  "<<string(buf,0 ,bytesReceived)<<endl;
  		}

          }

           if (!cap->read(src))
              return -1;

          //imwrite("source.jpg", src);

          gettimeofday(&current_time, NULL);
          t = (current_time.tv_sec - last_time.tv_sec)
                  + (current_time.tv_usec - last_time.tv_usec) / 1000000.;
          fps = 1. / t;
          fps = last_fps * 0.8 + fps * 0.2;
          last_fps = fps;
          last_time = current_time;

          Size size = src.size()/4;

          src.convertTo(src,-1,contrast,brightness);
          //cout<< overlay.size().height<< " "<<overlay.size().width<< endl;
          if(overlay.empty()!= true){
              if( type2str(overlay.type()) !=type2str(src.type()))        {
              overlay.convertTo(overlay, src.type() );
              cvtColor( overlay,overlay, CV_BGR2BGRA );
              }
              resize(overlay,overlay,size);
              overlay.copyTo(src.rowRange(0, overlay.size().height).colRange(0, overlay.size().width));
          }

          //cout<< type2str(overlay.type())<< "\n\n"<< )<<"\n\n\n\n" <<endl;
          imshow("camera", src);
          //imshow("overlay", overlay);
          //-- bail out if escape was pressed
          int c = waitKey(10);
          if ((char) c == 27) {
              break;
          }


      }

  //    overlay = imread("overlay.jpg");


      close(new_socket);
      delete cap;
      destroyAllWindows();

      return 0;

}

string removeSpaces(string input)
{
  input.erase(std::remove(input.begin(),input.end(),' '),input.end());
  return input;
}
