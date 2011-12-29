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

ArchEnvironment::ArchEnvironment() {
	// TODO Auto-generated constructor stub
}

