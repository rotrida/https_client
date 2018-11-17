#include "https_client.h"
#include <vector>
#include <utility>
#include "utils.h"

std::pair<String, String> separate_host_url(const String & address)
{
	const auto pos = address.indexOf('//');
	
	if(pos == -1 || pos+1 >= address.length())
		return std::make_pair(address, "");
		
	return std::make_pair(address.substring(0, pos), address.substring(pos));
}

https_client::https_client(const String & address, int port):
	_address(address), _port(port)
{
}

int https_client::get_code() const
{
	return _code;
}

const String & https_client::get_response() const
{
	return _response;
}

const String & https_client::get_last_error() const
{
	return _last_error;
}

bool https_client::read_result(WiFiClientSecure & client)
{
	if(!client.connected())
	{
		_last_error = "Not connected";
		return false;
	}
		
	String line = client.readStringUntil('\n');
	
	const auto result = utils::split(line, ' ');
	
	if(result.size() < 3 )
	{
		_last_error = "Wrong result format: " + line;
		return false;
	}
		
	_code = atoi(result[1].c_str());
	return true;
}

void https_client::read_headers(WiFiClientSecure & client)
{
	while (client.connected()) 
    {
		String line = client.readStringUntil('\n');
		
		if (line == "\r")
			return;
		
		auto header = utils::split(line, ' ');
		
		if(header.size() < 2)
			continue;

		auto & key = header[0];
		const auto length = key.length();
		if(length > 1)
			key = key.substring(0, length-2);
		
		_headers[key] = header[1];
    }
}

bool https_client::execute()
{
	WiFiClientSecure client;

 	const auto address = separate_host_url(_address);
		
	const auto & host = address.first;
	const auto & url = address.second;
    
    if (!client.connect(host, _port)) 
    {
		_last_error = "Error connecting to " + host + ":" + String(_port);
		return false;
	}
	
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
               
	
	read_result(client);
	read_headers(client);
  
	if(!client.connected())
	{
		_last_error = "Error reading body: Not connected.";
		return false;
	}
		
    client.readStringUntil('\n');
    _response = client.readStringUntil('\n');
    
    return true;
}

const std::unordered_map<String, String> & https_client::get_headers() const
{
	return _headers;
}
