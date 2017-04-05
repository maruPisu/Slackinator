#ifndef ROADMAP_H
#define ROADMAP_H
#include "utils.h"

namespace RoadmapActions{
const QString Generic	= ".*roadmap.*";
const QString Add	= ".*roadmap add (.*)";
const QString ReadAll	= ".*roadmap read all.*";
const QString Remove	= ".*roadmap remove (.*)";
}


class Roadmap
{
public:
	Roadmap();

	/**
	 * @brief computeResponse
	 *
	 * given a message, parse it, does the jobs and returns the corresponding message for the chat
	 * @param text
	 * @return
	 */
	static QString computeResponse(QString text);
};

#endif // ROADMAP_H
