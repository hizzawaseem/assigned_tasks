#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

using namespace std;

class Server_socket{
    // fstream file;
    FILE *picture;

    int PORT;
    
    int general_socket_descriptor;
    int new_socket_descriptor;

    struct sockaddr_in address;
    int address_length;

    public:
        Server_socket(){
            create_socket();
            PORT = 8050;

            address.sin_family = AF_INET; 
            address.sin_addr.s_addr = INADDR_ANY; 
            address.sin_port = htons( PORT );
            address_length = sizeof(address);

            bind_socket();
            set_listen_set();
            accept_connection();

            // getting picture size
            picture = fopen("image.jpg", "rb");

            if(picture == NULL) {
                cout << "[ERROR] : Error Opening Image File"; 
                exit(EXIT_FAILURE);
            }
            else
                cout<<"[LOG] : File is ready to Transmit.\n";

        }

        void create_socket(){
            if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
                perror("[ERROR] : Socket failed");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket Created Successfully.\n";
        }

        void bind_socket(){
            if (bind(general_socket_descriptor, (struct sockaddr *)&address, sizeof(address))<0) {
                perror("[ERROR] : Bind failed");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Bind Successful.\n";
        }

        void set_listen_set(){
            if (listen(general_socket_descriptor, 3) < 0) {
                perror("[ERROR] : Listen");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Socket in Listen State (Max Connection Queue: 3)\n";
        }

        void accept_connection(){
            if ((new_socket_descriptor = accept(general_socket_descriptor, (struct sockaddr *)&address, (socklen_t*)&address_length))<0) { 
                perror("[ERROR] : Accept");
                exit(EXIT_FAILURE);
            }
            cout<<"[LOG] : Connected to Client.\n";
        }

        void transmit_image(){
           
            int size, read_size, stat, packet_index;
            char send_buffer[27316], read_buffer[256] = {};
            packet_index = 1;

            // getting picture size
            picture = fopen("house.jpeg", "rb");
        
            if(picture == NULL) {
                cout<<"[ERROR] : Error Opening Image File\n"; 
            } 

            fseek(picture, 0, SEEK_END);
            size = ftell(picture);
            fseek(picture, 0, SEEK_SET);
            cout << "[LOG] : Total Picture size: " << size << '\n';

            // Send Picture Size
            cout << "[LOG] : Sending Picture Size\n";
            write(new_socket_descriptor, (void *)&size, sizeof(int));

            // Send Picture as Byte Array
            cout << "[LOG] : Sending Picture as Byte Array\n";

            do { //Read while we get errors that are due to signals.
                stat=read(new_socket_descriptor, &read_buffer , 255);
                cout << "[LOG] : Bytes read: " << stat << '\n';
            } while (stat < 0);

            cout << "[LOG] : Received data in socket\n";

            while(!feof(picture)) {
                //Read from the file into our send buffer
                read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

                //Send data through our socket 
                do{
                    stat = write(new_socket_descriptor, send_buffer, read_size);  
                } while (stat < 0);

                cout << "[LOG] : Packet Number: " << packet_index  << '\n';
                cout << "[LOG] : Packet Size Sent: " << read_size  << '\n';     
                // printf(" \n");
                // printf(" \n");

                packet_index++;  

                //Zero out our send buffer
                bzero(send_buffer, sizeof(send_buffer));
        }
    }
};

int main(){
    Server_socket S;
    S.transmit_image();
    return 0;
}