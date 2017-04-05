#include "instantresponse.h"
#include <QFile>
#include <QDebug>

InstantResponse::InstantResponse()
{

}

QString InstantResponse::computeResponse(QString text)
{
	if(checkRegex(text, ResponseActions::Add)){

		QString toAdd = replaceRegex(text, ResponseActions::Add, "\\1");

		if(!toAdd.contains("-->")){
			return "the message should contain the string '-->' between the pattern and the response";
		}

		toAdd = toAdd.trimmed();

		QFile file ("response");
		file.open(QIODevice::Append);

		QTextStream out(&file);
		out << toAdd << "\n";
		file.close();

		return "adding '" + toAdd + "' to the auto response";
	}
	if(checkRegex(text, ResponseActions::ReadAll)){
		QFile file ("response");
		file.open(QIODevice::ReadOnly);
		QString variable = file.readAll();
		file.close();

		QString responseString = "";
		int id = 0;
		QStringList splitted = variable.split("\n", QString::SplitBehavior::SkipEmptyParts);
		for(auto line: splitted){
			responseString.append(QString::number(++id) + ": " + line + "\\n");
		}

		return responseString;
	}
	if(checkRegex(text, ResponseActions::Remove)){

		QString toRemove = replaceRegex(text, ResponseActions::Remove, "\\1");

		if(!checkRegex(toRemove,"\\d*")){
			return toRemove + " is not a valid id";
		}


		QFile fileInput ("response");
		fileInput.open(QIODevice::ReadOnly);
		QString variable = fileInput.readAll();
		fileInput.close();

		QString responseString = "";
		int id = 0;
		QStringList splitted = variable.split("\n", QString::SplitBehavior::SkipEmptyParts);
		for(auto line: splitted){
			if(toRemove.compare(QString::number(++id)) != 0){
				responseString.append(line + "\n");
			}
		}

		QFile fileOutput ("response");
		fileOutput.open(QIODevice::WriteOnly);
		QTextStream out(&fileOutput);
		out << responseString;
		fileOutput.close();

		return "removing item number " + toRemove + " from the auto response";
	}
	return "";

}

QString InstantResponse::autoResponse(QString text)
{
	QFile fileInput ("response");
	fileInput.open(QIODevice::ReadOnly);
	QString variable = fileInput.readAll();
	fileInput.close();

	QStringList splitted = variable.split("\n", QString::SplitBehavior::SkipEmptyParts);
	for(QString line: splitted){
		if(line.contains("--&gt")){
			QString regex = line.split("--&gt;").at(0);
			QString thisMsg = line.split("--&gt;").at(1);
			if(checkRegex(text, regex)){
				return thisMsg;
			}
		}
	}
	return "";
}
