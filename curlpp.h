#ifndef CURLPP_H
#define CURLPP_H

#include <string>
#include <mutex>
#include <curl/curl.h>
#include <json/json.h>
#define NUM_OF_CURL_ERR 1000
//#define DONT_CURL

#ifdef DONT_CURL
	#warning *********** CURL is not activated!! ***********
#endif //DONT_CURL

using namespace std;

struct CurlStruct {
	char *memory = nullptr;
	size_t size = 0;
};

/**
 * @brief The CURLpp class
 *
 *
 * libCURL wrapper
 *
 * uses the builder design pattern
 *
 * EXAMPLE:
 *
 *	CURLpp test = CURLpp::Builder()
 *			.set_connect_timeout(2000)
 *			.set_follow_location(1)
 *			.set_url("ciao.php")
 *			.add_http_header("Content-Type: application/json")
 *			.build();						//build() MUST BE THE LAST!!
 *
 *
 * TO ADD A NEW VALUE:
 * 1 - add the new variable in builder with a default value
 * 2 - add the corresponding parameter in CURLpp constructor
 * 3 - add a Builder::set_new_variable with the same style as the given ones (remember return *this; at the end)
 * 4 - add the variable in the return inside Builder::build (the same order as CURLpp ctor)
 * 5 - add the corresponding curl setopt command in the CURLpp ctor
 * 6 - watch out for char pointers, you may want to make a persistent copy as member of CURLpp (e.g. post_fields_)
 *
 */
class CURLpp
{
	typedef vector<map<string, string>> vector_of_errors;
public:
	class Builder;
	std::string perform();
	Json::Value performJson();

	~CURLpp();

	std::string getLastError() const;

	static vector<map<string, string>> getListOfErrors();

private:
	Builder* copia; //avoid to go out of scope of certain thing
	CURLpp(const CURLpp::Builder& opt);

	template<typename T>
	void log(const char *name, T value);
	std::string getErrorFromCode(int code);
	static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

	CURL * marx;
	std::string lastError = "noerr";

	static mutex error_mutex;
	static vector_of_errors listOfErrors;
	static int error_counter;
};

class CURLpp::Builder{

public:

	int connect_timeout = 600;
	int timeout = 1200;
	int post = 0;
	int put = 0;
	int del = 0;
	int patch = 0;
	int get = 0;
	int follow_location = 0;
	std::string useragent = "";
	std::string url = "";
	curl_slist* http_header = NULL;
	std::string post_fields = "";
	std::string put_fields = "";
	std::string del_fields = "";
	std::string patch_fields = "";
	std::string get_fields = "";
	int verbose = 0;
	int auth = 0;
	std::string authCode = "";


	Builder& set_connect_timeout	( const int i )		{ this->connect_timeout = i; return *this; }
	Builder& set_timeout		( const int i )		{ this->timeout = i; return *this; }
	Builder& set_post		( const int i )		{ this->post = i; return *this; }
	Builder& set_follow_location	( const int i )		{ this->follow_location = i; return *this; }
	Builder& add_http_header	( const char * i )	{ http_header = curl_slist_append(http_header, i); return *this;}
	Builder& set_verbose		( const int i )		{ this->verbose = i; return *this; }
	Builder& set_delete_request	( const std::string i )	{ this->del = 1; this->del_fields = i; return *this; }
	Builder& set_auth		( const std::string i )	{ this->auth = 1; this->authCode = i; return *this; }
	Builder& set_put_fields		( const std::string i )	{ this->put = 1; put_fields = i; return *this; }
	Builder& set_post_fields	( const std::string i )	{ this->post = 1; this->post_fields = i; return *this; }
	Builder& set_patch_fields	( const std::string i )	{ this->patch = 1; this->patch_fields = i; return *this; }
	Builder& set_get_fields		( const std::string i )	{ this->get = 1; this->get_fields = i; return *this; }
	Builder& set_useragent		( const std::string i )	{ this->useragent = i; return *this; }
	Builder& set_url		( const std::string i )	{ this->url = i; return *this; }

	CURLpp build();
};





/** In order to launch RTB Won Notification Url the code at rtbhub.h/cpp is adapted to recicle the internal OBJ connection ecc
 *  See: thread_local C_D_Handler Lenin;
 *
 * Is not needed to have "multiple" curl handle for the various host because
 * https://curl.haxx.se/docs/faq.html#What_about_Keep_Alive_or_persist
 * When you use the easy interface the connection cache is kept within the easy handle.
 * If you instead use the multi interface, the connection cache will be kept within the multi handle and
 * will be shared among all the easy handles that are used within the same multi handle.
 *
 */
typedef unsigned long long quint64;
//GG all forward declaration of template, no one can stop us!
class QByteArray;
template<typename QByteArray>
class QList;


class CurlHandlerWrapper {
public:

	CurlHandlerWrapper(long timeout_in_milliseconds = 350L);

	~CurlHandlerWrapper() { curl_easy_cleanup(handle); }

	bool sendNotification(const quint64& banner_id, const QList<QByteArray>& UrlList);
private:
	CURL* handle {nullptr};
};


#endif // CURLPP_H
