#include <sstream>

#include "branch.h"
#include "commit.h"
#include "exception.h"
#include "filesystem.h"
#include "helper.h"
#include "object.h"
#include "ref.h"
#include "repository.h"
#include "tag.h"

//bool is_tag(const std::string& name);
//bool is_branch(const std::string& name);
//std::string	read_tag(const std::string& name);

Ref parse_ref(const Repository& repo, const std::string& ref) {
	auto it = ref.find('/');
	if (it == ref.end())
		throw InvalidRefException(ref);

	if (std::string(ref.begin(), it))

	std::string refs_string
}

std::string resolve_ref(const Repository& repo, const std::string& ref)
{
	try {
		if (is_tag(repo, ref))
			return resolve_tag(repo, ref);
		if (is_branch(repo, ref))
			return resolve_branch(repo, ref);
		else if (ref.size() >= 4)
			return expand_object_id_prefix(repo, ref);
		else
			throw ResolveRefException(ref);
	}
	catch (const std::exception&) {
		throw Exception(boost::format("\"%1%\" is not a valid ref") % ref);
	}
}

std::string expand_object_id_prefix(const Repository& repo, const std::string& object_id_prefix)
{
	const std::string dir_prefix = object_id_prefix.substr(0, Globals::IdPrefixLength);
	const fs::path obj_dir = get_object_dir(repo, dir_prefix);

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

std::string resolve_tag(const Repository& repo, const std::string& tag_name)
{
	return expand_object_id_prefix(repo, read_tag(repo, tag_name));
}

std::string resolve_branch(const Repository& repo, const std::string& branch_name)
{
	return Filesystem::read_content(repo.branchDir / branch_name);
}

std::string resolve_to_blob(const Repository& repo, const std::string& ref)
{
	if (ref.size() >= 4)
	{
		std::string object_id = expand_object_id_prefix(repo, ref);

		if (Object(repo, object_id).kind() == "blob")
			return object_id;
		else
			throw Exception(boost::format("object is not a blob: %1%") % object_id);
	}

	throw Exception(boost::format("\"%1\" is not a valid ref") % ref);
}

std::string resolve_to_tree(const Repository& repo, const std::string& ref)
{
	if (ref.size() >= 4)
	{
		std::string object_id = expand_object_id_prefix(repo, ref);
		Object object(repo, object_id);

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

std::string resolve_to_commit(const Repository& repo, const std::string& ref)
{
	std::string object_id = resolve_ref(repo, ref);
	std::string object_kind = Object(repo, object_id).kind();

	if (object_kind != "commit")
		throw Exception(boost::format("object is not a commit: %1%") % object_id);

	return object_id;
}

std::string resolve_to_branch(const Repository& repo, const std::string& ref)
{
	if (is_branch(repo, ref))
		return ref;
	else
		throw Exception("\"ref\" is not a branch");
}

std::string resolve_head(const Repository& repo, std::string& branch, std::string& commit_id))
{
	if (!fs::exists(repo.headFile))
		throw Exception("head doesn't exist");

	std::string content = Filesystem::read_content(repo.headFile);
	if (content.starts_with("ref: ")
	if (is_branch(repo, name))
		return resolve_branch(repo, name);
	else
		return name;
}

bool try_resolve(const Repository& repo, const std::string& ref, std::string& id)
{
	try {
		id = resolve_ref(repo, ref);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}

bool try_resolve(const Repository& repo, const std::string& ref)
{
	std::string dummy;
	return try_resolve(repo, ref, dummy);
}

bool try_resolve_to_blob(const Repository& repo, const std::string& ref, std::string& blob_id)
{
	try {
		blob_id = resolve_to_blob(repo, ref);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}

bool try_resolve_to_blob(const Repository& repo, const std::string& ref)
{
	std::string dummy;
	return try_resolve_to_blob(repo, ref, dummy);
}

bool try_resolve_to_tree(const Repository& repo, const std::string& ref, std::string& tree_id)
{
	try {
		tree_id = resolve_to_tree(repo, ref);
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}

bool try_resolve_to_tree(const Repository& repo, const std::string& ref)
{
	std::string dummy;
	return try_resolve_to_tree(repo, ref, dummy);
}

bool try_resolve_to_commit(const Repository& repo, const std::string& ref, std::string& commit_id)
{
	try {
		commit_id = resolve_to_commit(repo, ref);
		return true;
	}
	catch (const Exception&) {
		return false;
	}
}

bool try_resolve_to_commit(const Repository& repo, const std::string& ref)
{
	std::string dummy;
	return try_resolve_to_commit(repo, ref, dummy);
}

bool try_resolve_to_branch(const Repository& repo, const std::string& ref, std::string& branch_name)
{
	try {
		branch_name = resolve_to_branch(repo, ref);
		return true;
	}
	catch (const Exception&) {
		return false;
	}
}

bool try_resolve_to_branch(const Repository& repo, const std::string& ref)
{
	std::string dummy;
	return try_resolve_to_branch(repo, ref, dummy);
}

bool match_object_id(const std::string& prefix, const std::string& object_id)
{
	return std::equal(prefix.begin(), prefix.end(), object_id.begin());
}
