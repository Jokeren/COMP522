/*
 * Configuration.cpp
 *
 *  Created on: Dec 27, 2011
 */

#include "Configuration.h"
#include <stdlib.h>
#include <assert.h>

Configuration* Configuration::instance = NULL;

Configuration* Configuration::getInstance() {
	if (instance == NULL) {
		instance = new Configuration();
	}
	return instance;
}

Configuration::Configuration() {
	char* filename = "SALSA/config.txt";
	assert (filename != NULL);
	configFile = new ConfigFile(filename);
}
