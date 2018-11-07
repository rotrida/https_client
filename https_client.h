#pragma once

#include <WiFiClientSecure.h>
#include <unordered_map>
#include <string>

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<> struct hash<String>
    {
        typedef String argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept
        {
            return std::hash<std::string>{}(s.c_str());
        }
    };
}

class https_client
{
	private:
	
		int _code;
		String _address;
		int _port;
		std::unordered_map<String, String> _headers;
		String _response;
		String _last_error;
		
		bool read_result(WiFiClientSecure & client);
		void read_headers(WiFiClientSecure & client);
		
	public:
	
		https_client(const String & address, int port);
		int get_code() const;
		const String & get_response() const;
		const String & get_last_error() const;
		bool execute();
		const std::unordered_map<String, String> & get_headers() const;
};
