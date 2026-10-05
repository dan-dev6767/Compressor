#include <CLI/CLI.hpp>
#include "compress.hpp"

int main(int argc, char* argv[])
{
	std::vector<std::string> files;
	int threads = 0;
	int level = 3;
	bool isDecompress = false;

	CLI::App app("This App can compress your files");
	argv = app.ensure_utf8(argv);
	app.allow_windows_style_options();

	// APP Options
	app.add_option("-f,--files", files, "Files names")->required()->expected(-1);
	app.add_flag("-d,--decompress", isDecompress);
	app.add_option("-t,--threads", threads, "Threads count")->check(CLI::Range(0,256));
	app.add_option("-l,--level", level, "Compression Level")->check(CLI::Range(1, 22));

	CLI11_PARSE(app, argc, argv);

	// APP Logic
	try {
		if (!isDecompress)
		{
			for (const auto& file : files)
			{
				LazyCompress(file.c_str(), level, threads);
			}
		}
		else {
			for (const auto& file : files)
			{
				LazyDeCompress(file.c_str());
			}
		}
	} catch (const std::exception& e) 
	{
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}