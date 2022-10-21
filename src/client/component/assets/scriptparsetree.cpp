#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/flags.hpp>
#include <utils/io.hpp>

#include "scriptparsetree.hpp"

namespace assets
{
	namespace scriptparsetree
	{
		std::unordered_map<std::string, std::unique_ptr<std::string>> script_buffer_buffers;

		void dump_scriptparsetree(game::ScriptParseTree* script_parse_tree)
		{
			if (!script_parse_tree || !script_parse_tree->name || script_parse_tree->name[0] == '\0')
			{
				return;
			}

			printf("Dumping script %s bytes: %d\n", script_parse_tree->name, script_parse_tree->len);
			std::string buffer{};
			if (script_parse_tree->buffer)
			{
				buffer.assign(script_parse_tree->buffer, script_parse_tree->len);
			}

			utils::io::write_file(utils::string::va("dump/%s/%s", game::g_load->filename, script_parse_tree->name), buffer);
		}

		void override_scriptparsetree(game::ScriptParseTree* script_parse_tree)
		{
			if (!script_parse_tree || !script_parse_tree->name || script_parse_tree->name[0] == '\0')
			{
				return;
			}

			char* buffer{};
			auto num_bytes_read = game::FS_ReadFile(script_parse_tree->name, reinterpret_cast<void**>(&buffer));

			if (num_bytes_read < 0)
			{
				return;
			}
			
			if (num_bytes_read > 4 && !strncmp("\x80GSC", buffer, 4))
			{
				printf("Overriding script %s\n", script_parse_tree->name);

				auto itr = script_buffer_buffers.find(script_parse_tree->name);
				if (itr != script_buffer_buffers.end())
				{
					script_buffer_buffers.erase(itr);
				}
				script_buffer_buffers[script_parse_tree->name] = std::make_unique<std::string>();
				itr = script_buffer_buffers.find(script_parse_tree->name);
				auto script_buffer = itr->second.get();

				script_buffer->assign(buffer, num_bytes_read);

				script_parse_tree->len = num_bytes_read;
				script_parse_tree->buffer = script_buffer->data();
			}
			else
			{
				printf("%s is not a valid compiled GSC script\n", script_parse_tree->name);
			}

			game::FS_FreeFile(buffer);
		}
	}
}