#include "SDF.h"
#include "BinPacker.h"

namespace SDF {

	std::vector<sdf_glyph*> all_glyphs;
	std::vector<unsigned char> pdata;

	//	number of rendered pixels per SDF pixel
	static const int scaler = 16;
	//	(larger value means higher quality, up to a point)

	bool render_signed_distance_font(const FT_Library &ft_lib, const char* font_file, int texture_size, int charCount) {
		all_glyphs.clear();
		pdata.clear();

		FT_Face ft_face;
		FT_New_Face(ft_lib, font_file, 0, &ft_face);

		int max_unicode_char{ charCount };

		std::vector<int> render_list;
		for (int char_idx{ 0 }; char_idx <= max_unicode_char; ++char_idx) {
			render_list.push_back(char_idx);
		}

		int sz{ 4 };
		bool keep_going{ true };
		while (keep_going) {
			sz <<= 1;
			keep_going = gen_pack_list(ft_face, sz, texture_size, render_list, all_glyphs);
		}
		int sz_step{ sz >> 2 };
		while (sz_step) {
			if (keep_going) {
				sz += sz_step;
			}
			else {
				sz -= sz_step;
			}
			sz_step >>= 1;
			keep_going = gen_pack_list(ft_face, sz, texture_size, render_list, all_glyphs);
		}
		while ((!keep_going) && (sz > 1)) {
			--sz;
			keep_going = gen_pack_list(ft_face, sz, texture_size, render_list, all_glyphs);
		}

		if (!keep_going) {
			printf("The data will not fit in a texture %ix%i! Increase atlas size!\n", texture_size, texture_size);
			return false;
		}

		pdata.resize(4 * static_cast<size_t>(texture_size) * texture_size, 0);

		int packed_glyph_index{ 0 };
		for (unsigned int char_index{ 0 }; char_index < render_list.size(); ++char_index) {
			int glyph_index = FT_Get_Char_Index(ft_face, render_list[char_index]);
			if (glyph_index) {
				FT_Load_Glyph(ft_face, glyph_index, 0);
				FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_MONO);

				int w = ft_face->glyph->bitmap.width;
				int h = ft_face->glyph->bitmap.rows;
				int p = ft_face->glyph->bitmap.pitch;

				int sw = w + scaler * 4;
				int sh = h + scaler * 4;
				unsigned char *smooth_buf = new unsigned char[static_cast<size_t>(sw) * sh];
				for (int i{ 0 }; i < sw * sh; ++i) {
					smooth_buf[i] = 0;
				}

				unsigned char *buf = ft_face->glyph->bitmap.buffer;
				for (int j{ 0 }; j < h; ++j) {
					for (int i{ 0 }; i < w; ++i) {
						smooth_buf[scaler * 2 + i + (j + scaler * 2) * sw] = 255 * ((buf[j * p + (i >> 3)] >> (7 - (i & 7))) & 1);
					}
				}

				int sdfw = all_glyphs[packed_glyph_index]->width;
				int sdfx = static_cast<int>(all_glyphs[packed_glyph_index]->x);
				int sdfh = all_glyphs[packed_glyph_index]->height;
				int sdfy = static_cast<int>(all_glyphs[packed_glyph_index]->y);
				for (int j{ 0 }; j < sdfh; ++j) {
					for (int i{ 0 }; i < sdfw; ++i)
					{
						int pd_idx = (i + sdfx + (j + sdfy) * texture_size) * 4;
						size_t idx = static_cast<size_t>(pd_idx);
						pdata[idx] = get_SDF_radial(smooth_buf, sw, sh, i * scaler + (scaler >> 1), j * scaler + (scaler >> 1), 2 * scaler);
						pdata[idx + 1] = pdata[pd_idx];
						pdata[idx + 2] = pdata[pd_idx];
						pdata[idx + 3] = pdata[pd_idx];
					}
				}
				++packed_glyph_index;
				delete[] smooth_buf;
			}
		}
		FT_Done_Face(ft_face);
		return true;
	}

	bool gen_pack_list(const FT_Face& ft_face, int pixel_size, int pack_tex_size, const std::vector<int>& render_list, std::vector<sdf_glyph*>& packed_glyphs) {
		for (auto itr{ packed_glyphs.begin() }; itr != packed_glyphs.end(); ++itr) {
			delete *itr;
		}
		packed_glyphs.clear();
		FT_Set_Pixel_Sizes(ft_face, pixel_size * scaler, 0);
		std::vector<int> rectangle_info;
		std::vector<std::vector<int>> packed_glyph_info;
		for (unsigned int char_index{ 0 }; char_index < render_list.size(); ++char_index) {
			int glyph_index = FT_Get_Char_Index(ft_face, render_list[char_index]);
			if (glyph_index) {
				FT_Load_Glyph(ft_face, glyph_index, 0);
				FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_MONO);
				sdf_glyph* add_me{ new sdf_glyph() };

				int w = ft_face->glyph->bitmap.width;
				int h = ft_face->glyph->bitmap.rows;

				int sw = w + scaler * 4;
				int sh = h + scaler * 4;

				int sdfw = sw / scaler;
				int sdfh = sh / scaler;
				rectangle_info.push_back(sdfw);
				rectangle_info.push_back(sdfh);

				add_me->ID = render_list[char_index];
				add_me->width = sdfw;
				add_me->height = sdfh;

				add_me->x = -1;
				add_me->y = -1;

				add_me->xoff = static_cast<float>(ft_face->glyph->bitmap_left);
				add_me->yoff = static_cast<float>(ft_face->glyph->bitmap_top);
				add_me->xadv = static_cast<float>(ft_face->glyph->advance.x / 64.0);

				add_me->xoff = static_cast<float>(add_me->xoff / scaler - 1.5);
				add_me->yoff = static_cast<float>(add_me->yoff / scaler + 1.5);
				add_me->xadv = add_me->xadv / scaler;

				packed_glyphs.push_back(add_me);
			}
		}

		bool dont_allow_rotation{ false };
		BinPacker bp;
		bp.Pack(rectangle_info, packed_glyph_info, pack_tex_size, dont_allow_rotation);

		if (packed_glyph_info.size() == 1) {

			unsigned int lim{ static_cast<unsigned int>(packed_glyph_info[0].size()) };
			for (unsigned int i{ 0 }; i < lim; i += 4) {
				unsigned int idx = packed_glyph_info[0][i + 0];
				packed_glyphs[idx]->x = static_cast<float>(packed_glyph_info[0][i + 1]);
				packed_glyphs[idx]->y = static_cast<float>(packed_glyph_info[0][i + 2]);
			}
			return true;
		}
		return false;
	}

	unsigned char get_SDF_radial(unsigned char *fontmap, int w, int h, int x, int y, int max_radius) {
		float d2 = max_radius * max_radius + 1.f;
		unsigned char v = fontmap[x + y * w];
		for (int radius{ 1 }; (radius <= max_radius) && (radius*radius < d2); ++radius) {
			int line, lo, hi;

			line = y - radius;
			if ((line >= 0) && (line < h)) {
				lo = x - radius;
				hi = x + radius;
				if (lo < 0) {
					lo = 0;
				}
				if (hi >= w) {
					hi = w - 1;
				}
				int idx = line * w + lo;
				for (int i{ lo }; i <= hi; ++i) {
					if (fontmap[idx] != v) {
						float nx = static_cast<float>(i - x);
						float ny = static_cast<float>(line - y);
						float nd2 = nx * nx + ny * ny;
						if (nd2 < d2) {
							d2 = nd2;
						}
					}
					++idx;
				}
			}

			line = y + radius;
			if ((line >= 0) && (line < h)) {
				lo = x - radius;
				hi = x + radius;
				if (lo < 0) {
					lo = 0;
				}
				if (hi >= w) {
					hi = w - 1;
				}
				int idx = line * w + lo;
				for (int i{ lo }; i <= hi; ++i) {
					if (fontmap[idx] != v) {
						float nx = static_cast<float>(i - x);
						float ny = static_cast<float>(line - y);
						float nd2 = nx * nx + ny * ny;
						if (nd2 < d2) {
							d2 = nd2;
						}
					}
					++idx;
				}
			}

			line = x - radius;
			if ((line >= 0) && (line < w)) {
				lo = y - radius + 1;
				hi = y + radius - 1;
				if (lo < 0) {
					lo = 0;
				}
				if (hi >= h) {
					hi = h - 1;
				}
				int idx = lo * w + line;
				for (int i{ lo }; i <= hi; ++i) {
					if (fontmap[idx] != v) {
						float nx = static_cast<float>(line - x);
						float ny = static_cast<float>(i - y);
						float nd2 = nx * nx + ny * ny;
						if (nd2 < d2) {
							d2 = nd2;
						}
					}
					idx += w;
				}
			}

			line = x + radius;
			if ((line >= 0) && (line < w)) {
				lo = y - radius + 1;
				hi = y + radius - 1;
				if (lo < 0) {
					lo = 0;
				}
				if (hi >= h) {
					hi = h - 1;
				}
				int idx = lo * w + line;
				for (int i{ lo }; i <= hi; ++i) {
					if (fontmap[idx] != v) {
						float nx = static_cast<float>(line - x);
						float ny = static_cast<float>(i - y);
						float nd2 = nx * nx + ny * ny;
						if (nd2 < d2) {
							d2 = nd2;
						}
					}
					idx += w;
				}
			}
		}
		d2 = sqrtf(d2);
		if (v == 0) {
			d2 = -d2;
		}
		d2 *= 127.5f / max_radius;
		d2 += 127.5f;
		if (d2 < 0.f) {
			d2 = 0.f;
		}
		if (d2 > 255.f) {
			d2 = 255.f;
		}
		return (unsigned char)(d2 + 0.5);
	}
}