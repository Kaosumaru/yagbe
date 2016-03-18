#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include "vm/memory.hpp"
#include "vm/utils.hpp"
#include <bitset>
#include "tile_info.hpp"
#include "palette.hpp"


namespace yagbe
{
	class tilemap
	{
	public:
		constexpr static ipoint tilemap_size() { return { 32, 32 }; }
		constexpr static ipoint tile_size() { return{ 8, 8 }; }
		constexpr static ipoint tilemap_pixel_size() { return{ tilemap_size().x*tile_size().x, tilemap_size().y*tile_size().y }; }

		tilemap(memory &m, bool window = false) : _m(m), _window_mode(window)
		{
			if (!window)
				_parent_tilemap.reset(new tilemap(_m, true));
		}


		struct render_info
		{
			render_info() {}
			bool visible = true;

			int tile_offset;
			int in_tile_y;
			int in_tile_x;

			int reset_tile_offset_at = 99999;
			int reset_tile_offset_to = 0;


			uint8_t *tilemap_pointer;
			tile_info *tile_set_pointer;
			int tile_set_index;

			int palette_at_current_pixel() const 
			{
				if (!visible)
					return -1;
				auto info = current_tile();
				auto c = info->palette_index_at(in_tile_x, in_tile_y);
				return c;
			}

			void advance_x()
			{
				if (++in_tile_x != 8)
					return;
				in_tile_x = 0;
				tile_offset++;

				if (tile_offset == reset_tile_offset_at)
					tile_offset = reset_tile_offset_to;
			}
		protected:
			tile_info* current_tile() const
			{
				uint8_t tile_index = *(tilemap_pointer + tile_offset);
				return tile_at_index(tile_index);
			}


			tile_info *tile_at_index(uint8_t i) const
			{
				tile_info *info = tile_set_pointer;

				if (tile_set_index == 0)
					info += (int8_t)i; //this tileset is using signed coords
				else
					info += i; //this tileset is using unsigned coords

				return info;
			}
		};


		auto tile_offset_from(const ipoint& p)
		{
			//auto off = offset();
			int pix_x = p.x;// +off.x;
			int pix_y = p.y;// +off.y;

			int tile_x = (pix_x / tile_size().x);
			int tile_y = (pix_y / tile_size().y);
			return tile_x + tile_y * tilemap_size().x;
		}

		auto render_info_at_point(const ipoint& p)
		{
			render_info info;

			auto off = offset();
			int pix_x = p.x + off.x;
			int pix_y = p.y + off.y;

			int tile_x = (pix_x / tile_size().x);
			int tile_y = (pix_y / tile_size().y);

			if (is_wrapping())
			{
				tile_x %= tilemap_size().x;
				tile_y %= tilemap_size().y;

				info.reset_tile_offset_to = tile_offset_from({0, pix_y});
				info.reset_tile_offset_at = info.reset_tile_offset_to + tilemap_size().x;
			}
			else
			{
				if (pix_x < 0 || tile_x >= tilemap_size().x)
					info.visible = false;
				if (pix_y < 0 || tile_y >= tilemap_size().y)
					info.visible = false;

				if (!info.visible)
					return info;
			}

			
			info.tile_offset = tile_x + tile_y * tilemap_size().x;
			info.in_tile_x = pix_x % tile_size().x;
			info.in_tile_y = pix_y % tile_size().y;
			info.tilemap_pointer = _m.raw_pointer_at(current_tilemap_address());
			info.tile_set_index = tileset_index();
			info.tile_set_pointer = tileset_pointer();
			return info;
		}

		int palette_at_render_info(const render_info& p)
		{
			return p.palette_at_current_pixel();
		}

		int palette_at_point(const ipoint& p)
		{
			auto info = render_info_at_point(p);
			return info.palette_at_current_pixel();
		}




		bool render_scanline(yagbe::color* line, int line_index, int width)
		{
			if (!enabled())
				return false;

			render_info rinfo = render_info_at_point({ 0, line_index });
			render_info prinfo;

			if (_parent_tilemap && _parent_tilemap->enabled())
				prinfo = _parent_tilemap->render_info_at_point({ 0, line_index });

			for (int i = 0; i < width; i++)
			{
				if (_parent_tilemap && _parent_tilemap->enabled())
				{
					auto palette_index = prinfo.palette_at_current_pixel();
					if (palette_index != -1)
					{
						line[i] = color_of_index(palette_index);

						rinfo.advance_x();
						prinfo.advance_x();
						continue;
					}
				}

				auto palette_index = rinfo.palette_at_current_pixel();
				if (palette_index != -1)
					line[i] = color_of_index(palette_index);
				rinfo.advance_x();
				prinfo.advance_x();
			}

			return true;
		}

	public:

		//those are different for bg, and different for window
		bool enabled()
		{
			return !_window_mode ? (bool)_m.io_register.LCDC_background : (bool)_m.io_register.LCDC_window;
		}

		ipoint offset()
		{
			if (_window_mode)
				return{ -(_m.io_register.WX-7), -_m.io_register.WY };
			return{ _m.io_register.SCX, _m.io_register.SCY };
		}

		uint16_t current_tilemap_address()
		{
			if (_window_mode)
				return _m.io_register.LCDC_window_tile_map ? 0x9C00 : 0x9800;

			return _m.io_register.LCDC_background_tile_map ? 0x9C00 : 0x9800;
		}

		color color_of_index(uint8_t i)
		{
			return palette_entry{ _m.io_register.BGP }.color(i);
		}

		int tileset_index()
		{
			return _m.io_register.LCDC_background_tile_set ? 1 : 0;
		}

		bool is_wrapping()
		{
			return !_window_mode;
		}

	protected:
		tile_info* tileset_pointer()
		{
			if (tileset_index() == 0)
				return (tile_info*)_m.raw_pointer_at(0x9000);
			return (tile_info*)_m.raw_pointer_at(0x8000);
		}


		std::unique_ptr<tilemap> _parent_tilemap;
		memory &_m;
		bool _window_mode = false;
	};


};