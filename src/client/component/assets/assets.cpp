#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/flags.hpp>
#include <utils/io.hpp>

#include "rawfile.hpp"
#include "scriptparsetree.hpp"

namespace assets
{
	utils::hook::detour db_add_x_asset_hook;

	game::XAssetHeader db_add_x_asset_stub(game::XAssetType type, game::XAssetHeader header)
	{
		switch (type)
		{
		case game::ASSET_TYPE_RAWFILE:
			if (utils::flags::has_flag("dump"))
			{
				//rawfile::dump_rawfile(header.rawfile);
			}

			rawfile::override_rawfile(header.rawfile);
			break;
		case game::ASSET_TYPE_SCRIPTPARSETREE:
			if (utils::flags::has_flag("dump"))
			{
				scriptparsetree::dump_scriptparsetree(header.scriptParseTree);
			}

			scriptparsetree::override_scriptparsetree(header.scriptParseTree);
		default:
			break;
		}

		return db_add_x_asset_hook.invoke<game::XAssetHeader>(type, header);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			db_add_x_asset_hook.create(0x14141F4C0_g, db_add_x_asset_stub);
		}
	};
}

REGISTER_COMPONENT(assets::component)