#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "webserver.hpp"

#include "CgiRequest.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

enum CgiBodyParsingState {
	READING_BODY_CHUNCKED,
	BODY_NOT_USEFUL,
	STORE_BODY

};

class CgiHandler {
	private:
		typedef std::pair<std::string, std::string> FieldPair;
		typedef std::vector<char> Vec;
		typedef std::vector<char>::iterator VecIter;
		typedef std::vector<char>::const_iterator VecConstIter;

		
		CgiBodyParsingState m_state;
		std::map<std::string, std::string> m_headers;
		// std::size_t m_bodyBytes;
		std::string m_status;
		std::string m_location;
		std::string m_content_type;
		bool m_reading_body;
		std::vector<char> m_data;
		const HttpRequest& m_request;
		std::string m_cgi_script;
		int m_pipe_fds[2];
		HttpResponse &m_response;
		Vec& m_send_buffer;
		int m_pid;
		int m_ok;
		std::size_t m_last_read;
		std::size_t m_start_time;

		void checkHeader(const std::string& header);
		void parseBody(void);
		void setBodyState();
		bool isCgiField(const std::string& field_name, const std::string& field_value);
		void addEssentialHeaders();
		void setChunckedBody();
		void parseStatus(const std::string& field_value);
		int waitForProcess();
		void handleChild();
		bool isHeaderEgnored(const std::string& field_name);
		void handleLocation();
		void appendToSendBuffer(std::vector<char>::iterator it, const std::string& str);
		bool checkTimeOut();

		void addHeader(const std::string& header);
	public:
		CgiHandler& operator=(const CgiHandler& other);
		void killProcess();
		void checkProcessState();
		CgiHandler(const HttpRequest& cgiRequest, HttpResponse& m_response);
		CgiHandler(const CgiHandler& other);
		int execute(void);

		void parse(const std::vector<char>& data);
		std::pair<std::string, std::string> parse_header(const std::string& data);
		~CgiHandler(void);
};

#endif
