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

std::string resolve_to_blob(const std::string& ref)
{
	if (ref.size() >= 4)
	{
		std::string object_id = expand_object_id_prefix(ref);

		if (Object(object_id).kind() == "blob")
			return object_id;
		else
			throw Exception(boost::format("object is not a blob: %1%") % object_id);
	}

	throw Exception(boost::format("\"%1\" is not a valid ref") % ref);
}

std::string resolve_to_tree(const std::string& ref)
{
	if (ref.size() >= 4)
	{
		std::string object_id = expand_object_id_prefix(ref);
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

std::string resolve_to_commit(const std::string& ref)
{
	std::string object_id = resolve_ref(ref);
	std::string object_kind = Object(object_id).kind();

	if (object_kind != "commit")
		throw Exception(boost::format("object is not a commit: %1%") % object_id);

	return object_id;
}

std::string resolve_to_branch(const std::string& ref)
{
	if (is_branch(ref))
		return ref;
	else
		throw Exception("\"ref\" is not a branch");
}

std::string resolve_head()
{
	if (!fs::exists(Globals::HeadFile))
		throw Exception("head doesn't exist");

	std::string name = Filesystem::read_content(Globals::HeadFile);
	if (is_branch(name))
		return resolve_branch(name);
	else
		return name;
}

bool try_resolve(const std::string& ref, std::string& id)
{
	try {
		id = resolve_ref(ref);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}

bool try_resolve(const std::string& ref)
{
	std::string dummy;
	return try_resolve(ref, dummy);
}

bool try_resolve_to_blob(const std::string& ref, std::string& blob_id)
{
	try {
		blob_id = resolve_to_blob(ref);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}

bool try_resolve_to_blob(const std::string& ref)
{
	std::string dummy;
	return try_resolve_to_blob(ref, dummy);
}

bool try_resolve_to_tree(const std::string& ref, std::string& tree_id)
{
	try {
		tree_id = resolve_to_tree(ref);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}

bool try_resolve_to_tree(const std::string& ref)
{
	std::string dummy;
	return try_resolve_to_tree(ref, dummy);
}

bool try_resolve_to_commit(const std::string& ref, std::string& commit_id)
{
	try {
		commit_id = resolve_to_commit(ref);
		return true;
	}
	catch (const Exception&) {
		return false;
	}
}

bool try_resolve_to_commit(const std::string& ref)
{
	std::string dummy;
	return try_resolve_to_commit(ref, dummy);
}

bool try_resolve_to_branch(const std::string& ref, std::string& branch_name)
{
	try {
		branch_name = resolve_to_branch(ref);
		return true;
	}
	catch (const Exception&) {
		return false;
	}
}

bool try_resolve_to_branch(const std::string& ref)
{
	std::string dummy;
	return try_resolve_to_branch(ref, dummy);
}

bool match_object_id(const std::string& prefix, const std::string& object_id)
{
	return std::equal(prefix.begin(), prefix.end(), object_id.begin());
}
