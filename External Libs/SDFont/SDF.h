#pragma once

#include <cmath>
#include <ctime>
#include <cstdio>

#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace SDF {

	struct sdf_glyph;

	extern std::vector<sdf_glyph*> all_glyphs;
	extern std::vector<unsigned char> pdata;

	struct sdf_glyph {
		int ID;
		int width, height;
		float x, y;
		float xoff, yoff;
		float xadv;
	};

	bool render_signed_distance_font(const FT_Library& ft_lib, const char* font_file, int texture_size, int charCount);
	unsigned char get_SDF_radial(unsigned char *fontmap, int w, int h, int x, int y, int max_radius);
	bool gen_pack_list(const FT_Face& ft_face, int pixel_size, int pack_tex_size, const std::vector<int>& render_list, std::vector<sdf_glyph*>& packed_glyphs);
}