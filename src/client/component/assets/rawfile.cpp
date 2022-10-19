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
		std::unordered_map<std::string, std::unique_ptr<char[]>> rawfile_buffers;

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
				buffer = std::string(rawfile->buffer, rawfile->len);
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
			rawfile_buffers[rawfile->name] = std::make_unique<char[]>(num_bytes_read + 1);
			itr = rawfile_buffers.find(rawfile->name);
			auto rawfile_entry_buffer = itr->second.get();

			std::memcpy(rawfile_entry_buffer, buffer, num_bytes_read);
			rawfile_entry_buffer[num_bytes_read] = '\0';

			rawfile->len = num_bytes_read;
			rawfile->buffer = rawfile_entry_buffer;
			game::FS_FreeFile(buffer);
		}

		class component final : public component_interface
		{
		public:
		};
	}
}
REGISTER_COMPONENT(assets::rawfile::component)
