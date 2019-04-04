/*
 * Configuration.h
 *
 *  Created on: Dec 27, 2011
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include "ConfigFile.h"

class Configuration {
public:
	static Configuration* getInstance();

	template<class T> bool getVal(T& var, const string& key) const;
private:
	Configuration();
	static Configuration* instance;
	ConfigFile* configFile;
};

template<class T>
bool Configuration::getVal(T& var, const string& key) const {
	return configFile->readInto(var, key);
}

#endif /* CONFIGURATION_H_ */
