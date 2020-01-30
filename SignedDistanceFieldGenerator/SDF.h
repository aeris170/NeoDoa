#pragma once

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#ifdef DLL
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT __declspec(dllimport)
#endif



namespace SDF {

	struct DLL_EXPORT sdf_glyph {
		int ID;
		int width, height;
		float x, y;
		float xoff, yoff;
		float xadv;
	};

	DLL_EXPORT extern std::vector<sdf_glyph*> all_glyphs;
	DLL_EXPORT extern std::vector<unsigned char> pdata;

	DLL_EXPORT bool render_signed_distance_font(const FT_Library& ft_lib, const char* font_file, int texture_size, int charCount);
	bool gen_pack_list(const FT_Face& ft_face, int pixel_size, int pack_tex_size, const std::vector<int>& render_list, std::vector<sdf_glyph*>& packed_glyphs);
	unsigned char get_SDF_radial(unsigned char* fontmap, int w, int h, int x, int y, int max_radius);

}