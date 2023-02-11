#pragma once

namespace GUI::Controls {
	bool Slider( const std::string& name, float* var_name, float min, float max, const std::string& display = "%.1f", float increment = 1.f, bool precision = false );
	bool Slider( const std::string& name, int* var_name, int min, int max, const std::string& display = "%d", int increment = 1, bool precision = false );
	//int Slider( const std::string &name, int *var_name, int min, int max, const std::string &display = "%.0f" );
}
