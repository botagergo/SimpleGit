#pragma once

#include "defs.h"
#include "exception.h"

namespace po = boost::program_options;

class Config
{
public:
	class ConfigParameterNotFoundException;
	class ConfigFileNotExistException;

	enum CreateFlags { CREATE_IF_NOT_EXIST };

	~Config() { write(); }

	static Config create(std::initializer_list<fs::path> configs);

	template <typename T> T get(const std::string& name) const {
		T value;
		if (!try_get(name, value))
			throw ConfigParameterNotFoundException(name);
		return value;
	}
	template <typename T> bool try_get(const std::string& name, T& value) const {
		for (auto kv : data) {
			if (!kv.second)
				continue;
			auto it = kv.second.value().find(name);
			if (it != kv.second.value().end()) {
				value = it->second.as<T>();
				return true;
			}
		}
		return false;
	}

	template <typename T> void set(const fs::path& config, const std::string& name, const T& value) {
		auto& vm = get_vm(config);
		if (!vm) {
			vm = po::variables_map();
		}
		vm.value().insert_or_assign(name, po::variable_value(boost::any(value), false));
	}

	void set(const fs::path& config_file, const po::variables_map& vm_config);

	void write();

	static const po::options_description& get_config_description();

private:
	Config() = default;

	std::optional<po::variables_map>& get_vm(const fs::path& config_file);

	static po::variables_map		read(const fs::path& config_file, const po::options_description& cfg_od);
	static void						write(const fs::path& config_file, const po::variables_map& params);
	static void						merge_variables_map(po::variables_map& dest, const po::variables_map& src);

	std::map<fs::path, std::optional<po::variables_map>> data;
};

class Config::ConfigParameterNotFoundException : public Exception {
public:
	ConfigParameterNotFoundException(const std::string& name) : Exception(boost::format("config parameter not found: %1%") % name) {}
};

class Config::ConfigFileNotExistException : public Exception {
public:
	ConfigFileNotExistException(const fs::path& config_file) : Exception(boost::format("config file does not exist: %1%") % config_file) {}
};
