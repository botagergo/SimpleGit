#include <sstream>

#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "ref.h"

bool is_tag(const std::string& name);
bool is_branch(const std::string& name);
std::string	read_tag(const std::string& name);

std::string resolve_ref(const std::string& ref)
{
	try {
		if (is_tag(ref))
			return resolve_tag(ref);
		if (is_branch(ref))
			return resolve_branch(ref);
		else if (ref.size() >= 4)
			return expand_object_id_prefix(ref);
		else
			throw ResolveRefException(ref);
	}
	catch (const std::exception&) {
		throw Exception(boost::format("\"%1%\" is not a valid ref") % ref);
	}
}

std::string expand_object_id_prefix(const std::string& object_id_prefix)
{
	const std::string dir_prefix = object_id_prefix.substr(0, Globals::IdPrefixLength);
	const fs::path obj_dir = Filesystem::get_object_dir(dir_prefix);

	std::string expanded;

	for (const fs::path& object : fs::directory_iterator(obj_dir))
	{
		if (match_object_id(object_id_prefix.substr(Globals::IdPrefixLength), object.filename().string()))
		{
			if (expanded.size())
				throw Exception(boost::format("ambiguous object ref: %1%") % object_id_prefix);
			expanded = dir_prefix + object.filename().string();
		}
	}

	if (!expanded.size())
		throw InvalidObjectIdPrefix(object_id_prefix);

	return expanded;
}

std::string resolve_tag(const std::string& tag_name)
{
	return expand_object_id_prefix(read_tag(tag_name));
}

std::string resolve_branch(const std::string& branch_name)
{
	return Filesystem::read_content(Globals::BranchDir / branch_name);
}


std::string resolve_tree(const std::string& ref)
{
	if (ref.size() >= 4)
	{
		std::string object_id = expand_object_id_prefix(ref);

		std::istringstream in_stream;
		Object object(object_id);
		std::string object_kind = object.kind();

		if (object_kind == "commit")
		{
			Commit commit;
			*object.get_commit_reader() >> commit;
			return commit.tree_id;
		}
		else if (object_kind == "tree")
			return object_id;
		else
			throw Exception(boost::format("object is not a tree: %1%") % object_id);
	}

	throw Exception(boost::format("\"%1\" is not a valid ref") % ref);
}

std::string resolve_head()
{
	try {
		std::string name = Filesystem::read_content(Globals::HeadFile);
		if (is_branch(name))
			return resolve_branch(name);
		else
			return name;
	}
	catch (const FileOpenException&) {
		throw HeadDoesntExistException();
	}
}

bool match_object_id(const std::string& prefix, const std::string& object_id)
{
	return std::equal(prefix.begin(), prefix.end(), object_id.begin());
}
