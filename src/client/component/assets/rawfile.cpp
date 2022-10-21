#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/flags.hpp>
#include <utils/io.hpp>

#include "rawfile.hpp"

namespace assets
{
	namespace rawfile
	{
		std::unordered_map<std::string, std::unique_ptr<std::string>> rawfile_buffers;

		void dump_rawfile(game::RawFile* rawfile)
		{
			if (!rawfile || !rawfile->name || rawfile->name[0] == '\0')
			{
				return;
			}

			// Don't attempt to dump referenced assets
			if (rawfile->name[0] == ',')
			{
				return;
			}

			printf("Dumping rawfile %s bytes: %d\n", rawfile->name, rawfile->len);
			std::string buffer{};
			if (rawfile->buffer)
			{
				buffer.assign(rawfile->buffer, rawfile->len);
			}

			utils::io::write_file(utils::string::va("dump/%s/%s", game::g_load->filename, rawfile->name), buffer);
		}

		void override_rawfile(game::RawFile* rawfile)
		{
			if (!rawfile || !rawfile->name || rawfile->name[0] == '\0')
			{
				return;
			}

			if (rawfile->name[0] == ',')
			{
				return;
			}

			char* buffer{};
			auto num_bytes_read = game::FS_ReadFile(rawfile->name, reinterpret_cast<void**>(&buffer));

			if (num_bytes_read < 0)
			{
				return;
			}

			printf("Overriding rawfile %s\n", rawfile->name);

			auto itr = rawfile_buffers.find(rawfile->name);
			if (itr != rawfile_buffers.end())
			{
				rawfile_buffers.erase(itr);
			}
			rawfile_buffers[rawfile->name] = std::make_unique<std::string>();
			itr = rawfile_buffers.find(rawfile->name);
			auto rawfile_buffer = itr->second.get();

			rawfile_buffer->assign(buffer, num_bytes_read);

			rawfile->len = num_bytes_read;
			rawfile->buffer = rawfile_buffer->data();
			game::FS_FreeFile(buffer);
		}
	}
}