#include "http_server.hh"

#include <vector>

#include <sys/stat.h>

#include <fstream>
#include <string>
#include <numeric>
#include <iterator>

#include <sstream>
#include <iostream>

using namespace std;

/****/
/****/



vector<string> split(const string &s, char delim) {
  vector<string> elems;

  stringstream ss(s);
  string item;

  while (getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }

  return elems;
}

HTTP_Request::HTTP_Request(string request) {
  vector<string> lines = split(request, '\n');
  vector<string> split_request = split(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request

  /*k
   TODO : extract the request method and URL from first_line here
  */

  this->method=split_request[0];
  this->url=split_request[1];

  // cout<<"\n-------------HTTP_Request\n this->method: "<<this->method<<" this->url: "<<this->url<<endl;

  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }


}

HTTP_Response *handle_request(string req) {
  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response = new HTTP_Response();

  string url = string("html_files") + request->url;

  response->HTTP_version = "1.0";

  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

   

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
      url = url + "/index.html";

    }

    /*
    TODO : open the file and read its contents
    */

  //  cout<<"---handle request-------url: "<<url<<endl;

  
    ifstream readFile( url );
    string copyFile = accumulate( istream_iterator<char>{ readFile >> noskipws }, {}, string{} );
    readFile.close();

  
    /*
    TODO : set the remaining fields of response appropriately
    */

    response->body = copyFile;
    response->content_length = to_string(response->body.length());

  }

  else {
    response->status_code = "404";

    /*
    TODO : set the remaining fields of response appropriately
    */

   response->content_type = "text/html";
   response->status_text = "Not Found";
   url = string("html_files")+"/not_found.html";

   ifstream readFile( url );
   string copyFile = accumulate( istream_iterator<char>{ readFile >> noskipws }, {}, string{} );  
   
   response->body = copyFile;
   response->content_length = to_string(response->body.length());

   readFile.close();

  }

  delete request;

  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
 string my_response;

 my_response = "HTTP/" + HTTP_version + " " + status_code + " " + status_text + "\n";
 my_response = my_response + "Content-Type:" + content_type + "\n";
 my_response = my_response + "Content-Length:" + content_length + "\n" + body;

 return my_response;

}