#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP
#include <iostream>
#include <vector>


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

		CgiHandler& operator=(const CgiHandler& other);
		void checkHeader(const std::string& header);
		void parseBody(void);
		void setBodyState();
		bool isCgiField(const std::string& field_name, const std::string& field_value);
		void addEssentialHeaders();
		void setChunckedBody();
		void parseStatus(const std::string& field_value);
	public:
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
