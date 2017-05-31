#ifndef INSTANTRESPONSE_H
#define INSTANTRESPONSE_H
#include "utils.h"

namespace ResponseActions{
const QString Generic	= ".*auto response.*";
const QString Add	= ".*auto response add (.*)";
const QString ReadAll	= ".*auto response read all.*";
const QString Remove	= ".*auto response remove (.*)";
}

class InstantResponse
{
public:
	InstantResponse();
	static QString computeResponse(QString text);

	static QString autoResponse(QString text);

	static QString toString();
};

#endif // INSTANTRESPONSE_H
