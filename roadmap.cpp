#include "roadmap.h"
#include <QFile>
#include <QDebug>

Roadmap::Roadmap()
{

}

QString Roadmap::computeResponse(QString text)
{
	if(checkRegex(text, RoadmapActions::Add)){

		QString toAdd = replaceRegex(text, RoadmapActions::Add, "\\1");

		QFile file ("roadmap");
		file.open(QIODevice::Append);

		QTextStream out(&file);
		out << toAdd << "\n";
		file.close();

		return "adding '" + toAdd + "' to the roadmap";
	}
	if(checkRegex(text, RoadmapActions::ReadAll)){
		QFile file ("roadmap");
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
	if(checkRegex(text, RoadmapActions::Remove)){

		QString toRemove = replaceRegex(text, RoadmapActions::Remove, "\\1");

		if(!checkRegex(toRemove,"\\d*")){
			return toRemove + " is not a valid id";
		}


		QFile fileInput ("roadmap");
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

		QFile fileOutput ("roadmap");
		fileOutput.open(QIODevice::WriteOnly);
		QTextStream out(&fileOutput);
		out << responseString;
		fileOutput.close();

		return "removing item number " + toRemove + " from the roadmap";
	}
	return "";
}

QString Roadmap::toString()
{
	return "roadmap [add|remove|read all]";

}
