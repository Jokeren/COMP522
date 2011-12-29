/*
 * ArchEnvironment.h
 *
 *  Created on: Dec 28, 2011
 */

#ifndef ARCHENVIRONMENT_H_
#define ARCHENVIRONMENT_H_

#include "SCTaskPool.h"
#include "Consumer.h"
#include "Producer.h"

class ArchEnvironment {
public:
	static ArchEnvironment* getInstance();

	virtual SCTaskPool** getSortedConsumers(const Producer&);
	virtual SCTaskPool** getSortedConsumers(const Consumer&);

private:
	ArchEnvironment();
	static ArchEnvironment* instance;
};

#endif /* ARCHENVIRONMENT_H_ */
