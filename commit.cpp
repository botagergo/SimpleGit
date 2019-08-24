#include <fstream>

#include "commit.h"
#include "globals.h"
#include "helper.h"
#include "object_file_writer.h"

std::wstring create_commit(const std::wstring &tree_id, const std::vector<std::wstring> &parents, const std::wstring &author, const std::wstring &committer, const std::wstring &message)
{
	ObjectFileWriter writer;
	writer << L"tree" << L' ' << tree_id << L"\n";
	writer << L"parent";
	for (const std::wstring& parent : parents)
		writer << L' ' << parent;
	writer << L"\n";
	writer << L"author" << L' ' << author << L"\n";
	writer << L"committer" << L' ' << committer << L"\n";
	writer << message;
	return writer.save();
}
