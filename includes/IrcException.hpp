
#ifndef IRC_EXCEPTION_HPP
#define IRC_EXCEPTION_HPP

#include <string>
#include <exception>

class IrcException : public std::exception 
{
	private:
		std::string _errorCode;
		std::string _errorMsg;
		
	public:
		IrcException(const std::string& errorCode, const std::string& errorMsg) 
			: _errorCode(errorCode), _errorMsg(errorMsg) {}
			
		virtual ~IrcException() throw() {}
		
		const std::string& getErrorCode() const { return _errorCode; }
		const std::string& getErrorMsg() const { return _errorMsg; }
		
		const char* what() const throw() { return _errorMsg.c_str(); }
};

#endif
	