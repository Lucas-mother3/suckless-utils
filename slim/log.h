#ifndef _LOG_H_
#define _LOG_H_

#ifdef USE_CONSOLEKIT
#include "Ck.h" 
#endif
#ifdef USE_PAM
#include "PAM.h"
#endif
#include <fstream>

using namespace std;

class LogUnit
{
	ofstream logFile;
	ostream * logOut;
public:
	bool openLog(const char * filename);
	void closeLog();

	LogUnit();

	~LogUnit() { closeLog(); }

	template<typename Type> LogUnit & operator<<(const Type & text)
	{
		*logOut << text; logOut->flush();
		return *this;
	}

	LogUnit & operator<<(ostream & (*fp)(ostream&))
	{
		*logOut << fp; logOut->flush();
		return *this;
	}

	LogUnit & operator<<(ios_base & (*fp)(ios_base&))
	{
		*logOut << fp; logOut->flush();
		return *this;
	}
};

extern LogUnit logStream;

#endif /* _LOG_H_ */
