#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>

int counter = 0;
using namespace std;

void fnDownloadImage(const char *url){
    counter++;
    string file_name = "image_" + to_string(counter) + ".jpg";
    cout << '\n';
    cout << "*** Download Started for " << url << " ***\n";
    string command = "curl " + std::string(url) + " --output " + file_name;
    system(command.c_str());
}

int main()
{
    std::thread t1(fnDownloadImage,"https://i0.wp.com/www.escapenormal.com/wp-content/uploads/2017/05/Santorini-Greece.jpg"); 
    std::thread t2(fnDownloadImage,"https://i0.wp.com/www.escapenormal.com/wp-content/uploads/2017/05/Venice_Italy.jpg");
    std::thread t3(fnDownloadImage,"https://i2.wp.com/www.escapenormal.com/wp-content/uploads/2017/05/Great-Wall-of-China.jpg");

    // synchronize threads:
    t1.join();         
    t2.join();        
    t3.join();
    return 0;
}