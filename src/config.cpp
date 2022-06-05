#include <cstdarg>
#include <fstream>
#include <map>
#include <string>
#include <iostream>

#include "locale.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "config.h"
#include "error.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"

#include "platform.h"

namespace po = boost::program_options;

Config Config::create(std::initializer_list<fs::path> config_files) {
	const po::options_description& cfg_od = get_config_description();

	Config config;

	for (const fs::path& config_file : config_files) {
		try {
			auto pair = std::make_pair(config_file, std::optional<po::variables_map>());
			if (fs::exists(config_file)) {
				pair.second = read(config_file, cfg_od);
			}
			config.data.insert(pair);
		} catch (...) {}
	}

	return config;
}

void Config::set(const fs::path& config_file, const po::variables_map& vm_config) {
	auto& vm = get_vm(config_file);
	if (vm) {
		merge_variables_map(vm.value(), vm_config);
	} else {
		vm = vm_config;
	}
}

void Config::write() {
	for (auto kv : data) {
		if (!kv.second) {
			continue;
		}
		write(kv.first, kv.second.value());
	}
}

void Config::merge_variables_map(po::variables_map& dest, const po::variables_map& src)
{
	for (auto it = src.begin(); it != src.end(); it++)
	{
		//dest.erase(it->first);
		dest.insert_or_assign(it->first, it->second);
	}
}

std::optional<po::variables_map>& Config::get_vm(const fs::path& config_file) {
	auto it = data.find(config_file);
	if (it != data.end()) {
		return it->second;
	} else {
		throw ConfigFileNotExistException(config_file);
	}
}

po::variables_map Config::read(const fs::path& cfg_file, const po::options_description& cfg_od) {
	po::variables_map vm;
	po::store(po::parse_config_file(cfg_file.c_str(), cfg_od), vm);
	return vm;
}

void Config::write(const fs::path& config_file, const po::variables_map& params)	{
	using boost::property_tree::ptree;

	const po::options_description& cfg_od = get_config_description();
	po::variables_map vm = read(config_file, cfg_od);
	merge_variables_map(vm, params);

	std::map<std::string, ptree> ini_data;

	for (auto param : vm) {
		std::size_t first_dot = param.first.find_first_of('.');
		if (first_dot == std::string::npos)
			throw Exception(boost::format("Parameter name needs to contain '.': %1%") % param.first);

		std::string param_category = param.first.substr(0, first_dot);
		std::string param_name = param.first.substr(first_dot + 1);

		if (param_category.empty())
			throw Exception(boost::format("Parameter category cannot be empty: %1%") % param_category);
		if (param_name.empty())
			throw Exception(boost::format("Parameter name cannot be empty: %1%") % param_name);

		auto it = ini_data.find(param_category);
		if (it == ini_data.end())
			it = ini_data.insert(std::make_pair(param_category, ptree())).first;
		
		if (param.second.value().type() == typeid(std::string))
			it->second.put(param_name, param.second.as<std::string>());
		else if (param.second.value().type() == typeid(fs::path))
			it->second.put(param_name, param.second.as<fs::path>());
		else if (param.second.value().type() == typeid(bool))
			it->second.put(param_name, param.second.as<bool>());
		else
			FATAL << "unsupported type" << std::endl;
	}

	ptree root;
	for (auto it : ini_data) {
		root.push_front(ptree::value_type(it.first, it.second));
	}

	std::ofstream out;
	Filesystem::open(config_file, out);
	write_ini(out, root);
}

const po::options_description& Config::get_config_description() {
	auto get = [] {
		po::options_description cfg_od;

		cfg_od.add_options()
			("user.name", po::value<std::string>(), "The name of the git user")
			("user.email", po::value<std::string>(), "The email of the git user")
			("init.templateDir", po::value<fs::path>(), "The location of the template directory for repository initialization")
			("core.bare", po::value<bool>(), "The location of the template directory for repository initialization")
			;
		return cfg_od;
	};

	const static po::options_description cfg_od = get();
	return cfg_od;
}

/*void init()
{
	Globals::Config.clear();

	if (fs::exists(Globals::SystemConfigFile))
		Config::read(Globals::Config, Globals::SystemConfigFile);
	if (fs::exists(Globals::UserConfigFile))
		Config::read(Globals::Config, Globals::UserConfigFile);
	if (fs::exists(Globals::RepositoryConfigFile))
		Config::read(Globals::Config, Globals::RepositoryConfigFile);

	if (Globals::Config.find("user.name") == Globals::Config.end())
		Globals::Config["user.name"] = get_username();

	if (Globals::Config.find("debug") != Globals::Config.end())
		Globals::Debug = true;
	else
		Globals::Debug = false;
}
*/
