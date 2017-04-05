#include "utils.h"
#include <QRegularExpression>

utils::utils()
{

}

bool checkRegex(const QString &text, const QString &regex, bool caseInsensitive)
{
	QRegularExpression regexObj(regex);
	if(caseInsensitive){
		regexObj.setPatternOptions(QRegularExpression::PatternOption::CaseInsensitiveOption);
	}
	return regexObj.match(text).hasMatch();
}

QString replaceRegex(const QString &text, const QString &regex, const char *after, bool caseInsensitive)
{
	QRegularExpression regexObj(regex);
	if(caseInsensitive){
		regexObj.setPatternOptions(QRegularExpression::PatternOption::CaseInsensitiveOption);
	}
	QString toAdd = text;
	toAdd.replace(regexObj, after);

	return toAdd;
}
