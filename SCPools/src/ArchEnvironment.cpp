/*
 * ArchEnvironment.cpp
 *
 *  Created on: Dec 28, 2011
 */

#include "ArchEnvironment.h"

ArchEnvironment* ArchEnvironment::instance = NULL;

ArchEnvironment* ArchEnvironment::getInstance() {
	if (instance == NULL) {
		instance = new ArchEnvironment();
	}
	return instance;
}

SCTaskPool** ArchEnvironment::getSortedConsumers(const Producer&) {}
SCTaskPool** ArchEnvironment::getSortedConsumers(const Consumer&) {}


ArchEnvironment::ArchEnvironment() {
	// TODO Auto-generated constructor stub
}

