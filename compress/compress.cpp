#include "compress.hpp"
#include <zstd.h>

#include <fstream>
#include <vector>
#include <filesystem>

void LazyCompress(const char* filePath, int level, int threads)
{
	// Turn filePath str into a path
	std::filesystem::path file = std::filesystem::u8path(filePath);

	// Create Source File and Destination File
	std::ifstream source(file, std::ios::binary);
	if (!source.is_open()) throw std::runtime_error("Cannot open source file!");

	std::ofstream destination(file.string()+".zst", std::ios::binary);
	if (!destination.is_open()) throw std::runtime_error("Cannot create destination file!");

	// Init buffers
	std::vector<char> inBuffer(ZSTD_CStreamInSize());
	ZSTD_inBuffer input = { inBuffer.data(),0,0 };

	std::vector<char> outBuffer(ZSTD_CStreamOutSize());
	ZSTD_outBuffer output = { outBuffer.data(),ZSTD_CStreamOutSize(),0 };

	// Init ZSTD
	std::unique_ptr<ZSTD_CCtx, decltype(&ZSTD_freeCCtx)> cctx(ZSTD_createCCtx(), ZSTD_freeCCtx);
	if (!cctx) throw std::runtime_error("Cannot create ZSTD Compress context");

	size_t threadCount = ZSTD_CCtx_setParameter(cctx.get(), ZSTD_c_nbWorkers, threads);
	size_t levelCount = ZSTD_CCtx_setParameter(cctx.get(), ZSTD_c_compressionLevel, level);

	while (source.read(inBuffer.data(), inBuffer.size()) || source.gcount() > 0)
	{
		input.src = inBuffer.data();
		input.size = source.gcount();
		input.pos = 0;

		while (input.pos < input.size)
		{
			output.pos = 0;
			// Compresss File 
			size_t code = ZSTD_compressStream2(cctx.get(), &output, &input, ZSTD_e_continue);
			if (ZSTD_ErrorCode(code)) throw std::runtime_error(ZSTD_getErrorName(code));

			// Save File
			destination.write(outBuffer.data(), output.pos);
		}
	}

	input.size = 0;
	input.pos = 0;

	// Save File End
	size_t remaining = 1;
	while (remaining > 0)
	{
		output.pos = 0;
		remaining = ZSTD_compressStream2(cctx.get(), &output, &input, ZSTD_e_end);
		if (ZSTD_ErrorCode(remaining)) throw std::runtime_error(ZSTD_getErrorName(remaining));
	}
}

void LazyDeCompress(const char* filePath)
{
	// Turn filePath str into a path
	std::filesystem::path file = std::filesystem::u8path(filePath);

	// Create Source File and Destination File
	std::ifstream source(file, std::ios::binary);
	if (!source.is_open()) throw std::runtime_error("Cannot open source file!");
	
	file.replace_extension();

	std::ofstream destination(file.string() + ".zstd", std::ios::binary);
	if (!destination.is_open()) throw std::runtime_error("Cannot create destination file!");

	// Init buffers
	std::vector<char> inBuffer(ZSTD_DStreamInSize());
	std::vector<char> outBuffer(ZSTD_DStreamOutSize());

	// Init ZSTD
	std::unique_ptr<ZSTD_DCtx, decltype(&ZSTD_freeDCtx)> dctx(ZSTD_createDCtx(), ZSTD_freeDCtx);
	if (!dctx) throw std::runtime_error("Cannot Initializate Decompression ZSTD context");

	size_t lastRet = 0;
	while (source.read(inBuffer.data(), inBuffer.size()) || source.gcount() > 0)
	{
		ZSTD_inBuffer input = { inBuffer.data(), static_cast<size_t>(source.gcount()), 0 };

		while (input.pos < input.size)
		{
			ZSTD_outBuffer output = { outBuffer.data(),ZSTD_DStreamOutSize(),0 };

			// Decompress file
			lastRet = ZSTD_decompressStream(dctx.get(), &output, &input);
			if (ZSTD_isError(lastRet)) throw std::runtime_error(ZSTD_getErrorName(lastRet));

			// Save Decompress data
			destination.write(outBuffer.data(), output.pos);
		}
	}
	if (lastRet != 0) throw std::runtime_error("Decomression error: file truncated or incomplete!");
}