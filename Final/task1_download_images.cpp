#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <thread>
#include <math.h>

int counter = 0;
using namespace std;

int progress_func(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded){
    // ensure that the file to be downloaded is not empty
    // because that would cause a division by zero error later on
    if (TotalToDownload <= 0.0) {
        return 0;
    }

    // how wide you want the progress meter to be
    int totaldotz=40;
    double fractiondownloaded = NowDownloaded / TotalToDownload;
    // part of the progressmeter that's already "full"
    int dotz = (int) round(fractiondownloaded * totaldotz);
    
    // create the "meter"
    int ii=0;
    printf("%3.0f%% [",fractiondownloaded*100);
    // part  that's full already
    for ( ; ii < dotz;ii++) {
        printf("=");
    }
    // remaining part (spaces)
    for ( ; ii < totaldotz;ii++) {
        printf(" ");
    }
    // and back to line begin - do not forget the fflush to avoid output buffering problems!
    printf("]\r");
    fflush(stdout);
    // if you don't return 0, the transfer will be aborted - see the documentation
    return 0; 
}

void fnDownloadImage(const char *url){
  CURL *curl;
  CURLcode res;
  FILE *fp;
  
  counter++;
  string file_name = "image_" + to_string(counter) + ".jpg";
  cout << "*** Download Started for " << url << " ***\n";
  curl = curl_easy_init();

  if(curl) {
    fp = fopen(file_name.c_str(),"wb");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    // Internal CURL progressmeter must be disabled if we provide our own callback
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
    // Install the callback function
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func); 
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
    cout << "*** Download Ended for " << url << " ***\n";
  }
}

int main(void)
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
