#include<iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class Client_socket{
    // fstream file;
    FILE *image;

    int PORT;
    
    int general_socket_descriptor;

    struct sockaddr_in address;
    int address_length;

    public:
        Client_socket(){
            create_socket();
            PORT = 8050;

            address.sin_family = AF_INET;
            address.sin_port = htons( PORT );
            address_length = sizeof(address);
            if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)<=0) { 
                cout<<"[ERROR] : Invalid address\n";
            }

            create_connection();
            
            image = fopen("recieved_image.jpeg", "wb");
            if( image == NULL) {
                cout<<"[ERROR] : Error has occurred. Image file could not be opened\n";
                exit(EXIT_FAILURE);
            }
            else
                cout<<"[LOG] : Image file created.\n";

            /*file.open("client_file.txt", ios::out | ios::trunc | ios::binary);
            if(file.is_open()){
                cout<<"[LOG] : File Creted.\n";
            }
            else{
                cout<<"[ERROR] : File creation failed, Exititng.\n";
                exit(EXIT_FAILURE);
            }*/
        }

        void create_socket(){
            if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
                perror("[ERROR] : Socket failed.\n");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket Created Successfully.\n";
        }

        void create_connection(){
            if (connect(general_socket_descriptor, (struct sockaddr *)&address, sizeof(address)) < 0) { 
                perror("[ERROR] : connection attempt failed.\n");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Connection Successfull.\n";
        }

        void receive_image(){

            int recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
            char imagearray[27316];

            //Find the size of the image
            do{
                stat = read(general_socket_descriptor, &size, sizeof(int));
            } while(stat<0);

            
            cout << "[LOG] : Packet received.\n";
            cout << "[LOG] : Packet size: " << stat << '\n';
            cout << "[LOG] : Image size: " << size << '\n';
            
            char buffer[27316];

            //Send our verification signal
            do {
                stat = write(general_socket_descriptor, &buffer, sizeof(int));
            } while(stat<0);

            cout << "[LOG] : Reply sent\n";

            image = fopen("recieved_image.jpeg", "wb");

            if( image == NULL) {
                cout<<"[ERROR] : Image file could not be opened\n";
                exit(EXIT_FAILURE); 
            }

            //Loop while we have not received the entire file yet
            int need_exit = 0;
            struct timeval timeout = {10,0};

            fd_set fds;
            int buffer_fd, buffer_out;

            while(recv_size < size) {
            //while(packet_index < 2){

                FD_ZERO(&fds);
                FD_SET(general_socket_descriptor,&fds);

                buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

                if (buffer_fd < 0)
                    cout << "[ERROR] : Bad file descriptor set.\n";

                if (buffer_fd == 0)
                    cout<<"[ERROR] : Buffer read timeout expired.\n";

                if (buffer_fd > 0){
                    do{
                        read_size = read(general_socket_descriptor,imagearray, 27316);
                    } while(read_size <0);

                    cout << "[LOG] : Packet number received: " << packet_index << '\n';
                    cout << "[LOG] : Packet size: " << read_size  << '\n';
                    
                    //Write the currently read data into our image file
                    write_size = fwrite(imagearray,1,read_size, image); 
                    cout << "[LOG] : Written image size: " << write_size  << '\n';  

                    if(read_size !=write_size) {
                        cout<<"[ERROR] :error in read write\n";    
                    }

                    //Increment the total number of bytes read
                    recv_size += read_size;
                    packet_index++;
                    cout << "[LOG] : Total received image size: " << recv_size  << '\n';
                }
            }

            fclose(image);
            cout << "[LOG] : Image successfully Received!\n";
        }
};

int main(){
    Client_socket C;
    C.receive_image();
    return 0;
}