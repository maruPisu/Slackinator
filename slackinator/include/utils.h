#ifndef UTILS_H
#define UTILS_H

#include <QString>

class utils
{
public:
	utils();

};

bool checkRegex(const QString &text, const QString &regex, bool caseInsensitive = true);
QString replaceRegex(const QString & text, const QString & regex, const char * after, bool caseInsensitive = true);

#endif // UTILS_H
