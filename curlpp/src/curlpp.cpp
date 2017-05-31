#include "curlpp.h"
#include "iostream"
#include <cstring>
#include <ctime>

vector<map<string, string>> CURLpp::listOfErrors(NUM_OF_CURL_ERR);
int CURLpp::error_counter = 0;
mutex CURLpp::error_mutex;

/**
 * @brief CURLpp::perform
 *
 * performs the curl code with the parameters given from the builder
 * @return a string containing "error:" if something went wrong, the curl response otherwise
 */
std::string CURLpp::perform()
{
	long http_code = 0;
	CURLcode res;
	std::string response = "";
	CurlStruct chunk;
	chunk.memory = (char*) malloc(1);  // will be grown as needed by the realloc
	chunk.size = 0;    // no data at this point

	curl_easy_setopt(marx, CURLOPT_WRITEFUNCTION, CURLpp::writeMemoryCallback);
	curl_easy_setopt(marx, CURLOPT_WRITEDATA, (void *)&chunk);

#ifdef DONT_CURL
	response = "{\"response\":{\"id\":42},\"id\":43,\"campaign\":{\"id\":44},\"success\":1667}";
	log("response", response);
#else
	res = curl_easy_perform(marx);
	if(res == CURLE_OK){
		curl_easy_getinfo (marx, CURLINFO_RESPONSE_CODE, &http_code);
		if(http_code == 200){
			if(chunk.size > 0){
				response.append(chunk.memory,chunk.size);
				log("response", chunk.memory);
			}
		}else{
			if(chunk.size > 0){
				response.append(chunk.memory,chunk.size);
				log("httpcode", http_code);
				log("response", chunk.memory);
				lastError = "error: http code: " + std::to_string(http_code) + ", " + response + ", url:" + copia->url;
			}else{
				lastError = "error: http code: " + std::to_string(http_code) + ", no payload, url:" + copia->url;
			}
		}
	}else{
		log("curl errorcode", res);
		log("curl error", getErrorFromCode(res));
		lastError = "error: curl error: " + getErrorFromCode(res) + ", url:" + copia->url + ", timeout:" + to_string(copia->timeout);
	}
#endif

	if(lastError != "noerr"){
		lock_guard<mutex> el_gringo(error_mutex);
		map<string, string> error;
		std::time_t  c_ts = std::time(nullptr);
		error.insert(pair<string, string>("time",std::asctime(std::localtime(&c_ts))));
		error.insert(pair<string, string>("error",lastError));
		listOfErrors[error_counter++] = error;
		error_counter = error_counter % NUM_OF_CURL_ERR;
	}

	// free results
	free(chunk.memory);

	return response;
}

Json::Value CURLpp::performJson()
{
	std::string response = perform();

	Json::Value jsonResponse;
	Json::Reader reader;

	reader.parse(response.c_str(), jsonResponse);

	Json::StyledWriter styledWriter;
	std::string fullString = styledWriter.write(jsonResponse);
	log("json response", fullString);

	return jsonResponse;
}

CURLpp::~CURLpp(){
	// clean CURL handler
	curl_easy_cleanup(marx);

	// free the custom headers
	curl_slist_free_all(copia->http_header);
	delete copia;
}

size_t CURLpp::writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	CurlStruct *mem = (CurlStruct *)userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

CURLpp::CURLpp(const Builder& opt) :marx(curl_easy_init()) {
	copia = new Builder(opt);
	curl_easy_setopt(marx, CURLOPT_HTTPHEADER, copia->http_header);
	curl_easy_setopt(marx, CURLOPT_CONNECTTIMEOUT_MS, copia->connect_timeout);
	curl_easy_setopt(marx, CURLOPT_TIMEOUT_MS,copia->timeout);
	curl_easy_setopt(marx, CURLOPT_URL, copia->url.c_str());
	curl_easy_setopt(marx, CURLOPT_USERAGENT, copia->useragent.c_str());
	curl_easy_setopt(marx, CURLOPT_FOLLOWLOCATION, copia->follow_location);
	curl_easy_setopt(marx, CURLOPT_VERBOSE, copia->verbose);

	if(copia->post > 0){
		curl_easy_setopt(marx, CURLOPT_POST, copia->post);
		curl_easy_setopt(marx, CURLOPT_POSTFIELDS, copia->post_fields.c_str());
		log("post_fields", copia->post_fields);
	}

	if(copia->del){
		curl_easy_setopt(marx, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(marx, CURLOPT_POSTFIELDS, copia->del_fields.c_str());
		log("DELETE REQUEST", copia->del_fields);
	}

	if(copia->put > 0){
		curl_easy_setopt(marx, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(marx, CURLOPT_POSTFIELDS, copia->put_fields.c_str());
		log("put_fields", copia->put_fields);
	}

	if(copia->patch > 0){
		curl_easy_setopt(marx, CURLOPT_CUSTOMREQUEST, "PATCH");
		curl_easy_setopt(marx, CURLOPT_POSTFIELDS, copia->patch_fields.c_str());
		log("patch_fields", copia->patch_fields);
	}

	if(copia->get > 0){
		curl_easy_setopt(marx, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(marx, CURLOPT_POSTFIELDS, copia->get_fields.c_str());
		log("get_fields", copia->get_fields);
	}

	if(copia->auth){
		curl_easy_setopt(marx, CURLOPT_USERPWD, copia->authCode.c_str());
	}
	log("url", copia->url);
}

/**
 * why do not use curl_easy_strerror ?
 */
std::string CURLpp::getErrorFromCode(int code)
{
	static std::map<int, std::string> errorMap = {
		{0,  "CURLE_OK "},
		{1,  "CURLE_UNSUPPORTED_PROTOCOL "},
		{2,  "CURLE_FAILED_INIT "},
		{3,  "CURLE_URL_MALFORMAT "},
		{4,  "CURLE_NOT_BUILT_IN "},
		{5,  "CURLE_COULDNT_RESOLVE_PROXY "},
		{6,  "CURLE_COULDNT_RESOLVE_HOST "},
		{7,  "CURLE_COULDNT_CONNECT "},
		{8,  "CURLE_FTP_WEIRD_SERVER_REPLY "},
		{9,  "CURLE_REMOTE_ACCESS_DENIED "},
		{10, "CURLE_FTP_ACCEPT_FAILED "},
		{11, "CURLE_FTP_WEIRD_PASS_REPLY "},
		{12, "CURLE_FTP_ACCEPT_TIMEOUT "},
		{13, "CURLE_FTP_WEIRD_PASV_REPLY "},
		{14, "CURLE_FTP_WEIRD_227_FORMAT "},
		{15, "CURLE_FTP_CANT_GET_HOST "},
		{16, "CURLE_HTTP2 "},
		{17, "CURLE_FTP_COULDNT_SET_TYPE "},
		{18, "CURLE_PARTIAL_FILE "},
		{19, "CURLE_FTP_COULDNT_RETR_FILE "},
		{21, "CURLE_QUOTE_ERROR "},
		{22, "CURLE_HTTP_RETURNED_ERROR "},
		{23, "CURLE_WRITE_ERROR "},
		{25, "CURLE_UPLOAD_FAILED "},
		{26, "CURLE_READ_ERROR "},
		{27, "CURLE_OUT_OF_MEMORY "},
		{28, "CURLE_OPERATION_TIMEDOUT "},
		{30, "CURLE_FTP_PORT_FAILED "},
		{31, "CURLE_FTP_COULDNT_USE_REST "},
		{33, "CURLE_RANGE_ERROR "},
		{34, "CURLE_HTTP_POST_ERROR "},
		{35, "CURLE_SSL_CONNECT_ERROR "},
		{36, "CURLE_BAD_DOWNLOAD_RESUME "},
		{37, "CURLE_FILE_COULDNT_READ_FILE "},
		{38, "CURLE_LDAP_CANNOT_BIND "},
		{39, "CURLE_LDAP_SEARCH_FAILED "},
		{41, "CURLE_FUNCTION_NOT_FOUND "},
		{42, "CURLE_ABORTED_BY_CALLBACK "},
		{43, "CURLE_BAD_FUNCTION_ARGUMENT "},
		{45, "CURLE_INTERFACE_FAILED "},
		{47, "CURLE_TOO_MANY_REDIRECTS "},
		{48, "CURLE_UNKNOWN_OPTION "},
		{49, "CURLE_TELNET_OPTION_SYNTAX "},
		{51, "CURLE_PEER_FAILED_VERIFICATION "},
		{52, "CURLE_GOT_NOTHING "},
		{53, "CURLE_SSL_ENGINE_NOTFOUND "},
		{54, "CURLE_SSL_ENGINE_SETFAILED "},
		{55, "CURLE_SEND_ERROR "},
		{56, "CURLE_RECV_ERROR "},
		{58, "CURLE_SSL_CERTPROBLEM "},
		{59, "CURLE_SSL_CIPHER "},
		{60, "CURLE_SSL_CACERT "},
		{61, "CURLE_BAD_CONTENT_ENCODING "},
		{62, "CURLE_LDAP_INVALID_URL "},
		{63, "CURLE_FILESIZE_EXCEEDED "},
		{64, "CURLE_USE_SSL_FAILED "},
		{65, "CURLE_SEND_FAIL_REWIND "},
		{66, "CURLE_SSL_ENGINE_INITFAILED "},
		{67, "CURLE_LOGIN_DENIED "},
		{68, "CURLE_TFTP_NOTFOUND "},
		{69, "CURLE_TFTP_PERM "},
		{70, "CURLE_REMOTE_DISK_FULL "},
		{71, "CURLE_TFTP_ILLEGAL "},
		{72, "CURLE_TFTP_UNKNOWNID "},
		{73, "CURLE_REMOTE_FILE_EXISTS "},
		{74, "CURLE_TFTP_NOSUCHUSER "},
		{75, "CURLE_CONV_FAILED "},
		{76, "CURLE_CONV_REQD "},
		{77, "CURLE_SSL_CACERT_BADFILE "},
		{78, "CURLE_REMOTE_FILE_NOT_FOUND "},
		{79, "CURLE_SSH "},
		{80, "CURLE_SSL_SHUTDOWN_FAILED "},
		{81, "CURLE_AGAIN "},
		{82, "CURLE_SSL_CRL_BADFILE "},
		{83, "CURLE_SSL_ISSUER_ERROR "},
		{84, "CURLE_FTP_PRET_FAILED "},
		{85, "CURLE_RTSP_CSEQ_ERROR "},
		{86, "CURLE_RTSP_SESSION_ERROR "},
		{87, "CURLE_FTP_BAD_FILE_LIST "},
		{88, "CURLE_CHUNK_FAILED "},
		{89, "CURLE_NO_CONNECTION_AVAILABLE "},
		{90, "CURLE_SSL_PINNEDPUBKEYNOTMATCH "},
		{91, "CURLE_SSL_INVALIDCERTSTATUS "},
		{92, "CURLE_HTTP2_STREAM "}
	};

	return errorMap[code];
}

std::string CURLpp::getLastError() const
{
	return lastError;
}

template <typename T>
void CURLpp::log(const char *name, T value){
	if(copia->verbose){
		std::cout << "\x1B[32m" << name << ":\x1B[0m " << value << "\n";
	}
}

CURLpp CURLpp::Builder::build(){
	return CURLpp(*this);
}



CurlHandlerWrapper::CurlHandlerWrapper(long timeout_in_milliseconds) : handle{curl_easy_init()} {
	if( handle ) { curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, timeout_in_milliseconds); }
}
