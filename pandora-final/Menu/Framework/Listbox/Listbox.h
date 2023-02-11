#pragma once

namespace GUI::Controls {
	bool Listbox(const std::string& id, std::vector<std::string> elements, int* option, bool bSearchBar, int iSizeInElements);
	bool Luabox( const std::string& id, std::vector<std::string> elements, int* option, bool bSearchBar, int iSizeInElements );
}