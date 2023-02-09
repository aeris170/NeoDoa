# Convert Font Awesome icon font parameters to C and C++ compatible formats.
#
#------------------------------------------------------------------------------
#
# 1 - Source material
#
#   1.1 - Font Awesome
#           https://github.com/FortAwesome/Font-Awesome/blob/6.x/metadata/icons.yml
#           https://github.com/FortAwesome/Font-Awesome/blob/6.x/webfonts/fa-brands-400.ttf
#           https://github.com/FortAwesome/Font-Awesome/blob/6.x/webfonts/fa-regular-400.ttf
#           https://github.com/FortAwesome/Font-Awesome/blob/6.x/webfonts/fa-solid-900.ttf
#
#------------------------------------------------------------------------------
#
# 2 - Data sample
#
#   Font Awesome 4 example:
#
#           - input:                music:
#                                     changes:
#                                       - '1'
#                                       - 5.0.0
#                                     label: Music
#                                     search:
#                                       terms:
#                                         - note
#                                         - sound
#                                     styles:
#                                       - solid
#                                     unicode: f001
#
#           - output C and C++:     #define ICON_FA_MUSIC "\xef\x80\x81"	// U+f001
#
#           - output C#:            public const string Music = "\uf001";
#
#   All fonts have computed min, max_16 and max unicode fonts.
#   The min excludes the ASCII characters code points.
#   The max_16 is for use with libraries that only support 16 bit code points.
#
#           - output C and C++:     #define ICON_MIN_FA 0xf000
#                                   #define ICON_MAX_16_FA 0xf2e0
#                                   #define ICON_MAX_FA 0xf2e0
#             Exception for Font Awesome brands: we use ICON_MIN_FAB, ICON_MAX_16_FAB and ICON_MAX_FAB
#             to differentiate between brand and non-brand icons so they can be used together
#             (the defines must be unique in C and C++).
#
#           - output C#:            public const int IconMin = 0xf000;
#                                   public const int IconMax16 = 0xf2e0;
#                                   public const int IconMax = 0xf2e0;
#
#------------------------------------------------------------------------------
#
# 3 - Script dependencies
#
#   3.1 - Fonts source material online
#   3.2 - Python 3 - https://www.python.org/downloads/
#   3.3 - Requests - https://pypi.org/project/requests/
#   3.4 - PyYAML - https://pypi.org/project/PyYAML/
#
#------------------------------------------------------------------------------
#
# 4 - References
#
#   Stolen from: https://github.com/juliettef/IconFontCppHeaders/
#
#------------------------------------------------------------------------------


import requests
import yaml
import os
import sys

if sys.version_info[0] < 3:
    raise Exception( "Python 3 or a more recent version is required." )

# Fonts

class Font:
    font_name = '[ ERROR - missing font name ]'
    font_abbr = '[ ERROR - missing font abbreviation ]'
    font_minmax_abbr = ''   # optional - use if min and max defines must be differentiated. See Font Awesome Brand for example.
    font_data = '[ ERROR - missing font data file or url ]'
    ttfs = '[ ERROR - missing ttf ]'

    @classmethod
    def get_icons( cls, input_data ):
        # intermediate representation of the fonts data, identify the min and max
        print( '[ ERROR - missing implementation of class method get_icons for {!s} ]'.format( cls.font_name ))
        icons_data = {}
        icons_data.update({ 'font_min' : '[ ERROR - missing font min ]',
                            'font_max' : '[ ERROR - missing font max ]',
                            'icons' : '[ ERROR - missing list of pairs [ font icon name, code ]]' })
        return icons_data

    @classmethod
    def get_intermediate_representation( cls ):
        font_ir = {}
        if 'http' in cls.font_data:  # if url, download data
            response = requests.get( cls.font_data, timeout = 2 )
            if response.status_code == 200:
                input_raw = response.text
                print( 'Downloaded - ' + cls.font_name )
            else:
                raise Exception( 'Download failed - ' + cls.font_name )
        else:   # read data from file if present
            if os.path.isfile( cls.font_data ):
                with open( cls.font_data, 'r' ) as f:
                    input_raw = f.read()
                    f.close()
                    print( 'File read - ' + cls.font_name )
            else:
                raise Exception( 'File ' + cls.font_name + ' missing - ' + cls.font_data )
        if input_raw:
            icons_data = cls.get_icons( input_raw )
            font_ir.update( icons_data )
            font_ir.update({ 'font_data' : cls.font_data,
                             'font_name' : cls.font_name,
                             'font_abbr' : cls.font_abbr,
                             'font_minmax_abbr' : cls.font_minmax_abbr,
                             'ttfs' : cls.ttfs, })
            print( 'Generated intermediate data - ' + cls.font_name )
        return font_ir

class FontFA6( Font ):              # Font Awesome version 6 - Regular and Solid styles
    font_name = 'Font Awesome 6'
    font_abbr = 'FA'
    font_data = 'https://github.com/FortAwesome/Font-Awesome/raw/6.x/metadata/icons.yml'
    ttfs = [[ 'FAR', 'fa-regular-400.ttf', 'https://github.com/FortAwesome/Font-Awesome/blob/6.x/webfonts/fa-regular-400.ttf' ],
            [ 'FAS', 'fa-solid-900.ttf', 'https://github.com/FortAwesome/Font-Awesome/blob/6.x/webfonts/fa-solid-900.ttf' ]]
    font_fa_style = [ 'regular', 'solid' ]

    @classmethod
    def get_icons( cls, input_data ):
        icons_data = { }
        data = yaml.safe_load( input_data )
        if data:
            font_min = '0x10ffff'
            font_min_int = int( font_min, 16 )
            font_max_16 = '0x0'   # 16 bit max 
            font_max_16_int = int( font_max_16, 16 )
            font_max = '0x0'
            font_max_int = int( font_max, 16 )
            icons = []
            for key in data:
                item = data[ key ]
                for style in item[ 'styles' ]:
                    if style in cls.font_fa_style:
                        item_unicode = item[ 'unicode' ].zfill( 4 )
                        if [ key, item_unicode ] not in icons:
                            item_int = int( item_unicode, 16 )
                            if item_int < font_min_int and item_int > 0x0127 :  # exclude ASCII characters code points
                                font_min = item_unicode
                                font_min_int = item_int
                            if item_int > font_max_16_int and item_int <= 0xffff:   # exclude code points > 16 bits
                                font_max_16 = item_unicode
                                font_max_16_int = item_int
                            if item_int > font_max_int:
                                font_max = item_unicode
                                font_max_int = item_int
                            icons.append([ key, item_unicode ])
            icons_data.update({ 'font_min':font_min, 
                                'font_max_16' : font_max_16,
                                'font_max':font_max, 
                                'icons':icons })
        return icons_data

class FontFA6Brands( FontFA6 ):     # Font Awesome version 6 - Brand style
    font_name = 'Font Awesome 6 Brands'
    font_minmax_abbr = 'FAB'
    ttfs = [[ 'FAB', 'fa-brands-400.ttf', 'https://github.com/FortAwesome/Font-Awesome/blob/5.x/webfonts/fa-brands-400.ttf' ]]
    font_fa_style = [ 'brands' ]

class FontFA6Pro( FontFA6 ):        # Font Awesome version 6 Pro - Regular and Solid styles
    font_name = 'Font Awesome 6 Pro'
    font_data = 'icons.yml'
    ttfs = [[ 'FAR', 'fa-regular-400.ttf', 'fa-regular-400.ttf' ],
            [ 'FAS', 'fa-solid-900.ttf', 'fa-solid-900.ttf' ]]
    font_fa_style = [ 'regular', 'solid' ]

class FontFA6ProBrands( FontFA6Brands ):  # Font Awesome version 6 Pro - Brand style
    font_name = 'Font Awesome 6 Pro Brands'
    font_data = 'icons.yml'
    ttfs = [[ 'FAB', 'fa-brands-400.ttf', 'fa-brands-400.ttf' ]]


class Language:
    language_name = '[ ERROR - missing language name ]'
    file_name = '[ ERROR - missing file name ]'
    intermediate = {}

    def __init__( self, intermediate ):
        self.intermediate = intermediate

    @classmethod
    def prelude( cls ):
        print( '[ ERROR - missing implementation of class method prelude for {!s} ]'.format( cls.language_name ))
        result = '[ ERROR - missing prelude ]'
        return result

    @classmethod
    def lines_minmax( cls ):
        print( '[ ERROR - missing implementation of class method lines_minmax for {!s} ]'.format( cls.language_name ))
        result = '[ ERROR - missing min and max ]'
        return result

    @classmethod
    def line_icon( cls, icon ):
        print( '[ ERROR - missing implementation of class method line_icon for {!s} ]'.format( cls.language_name ))
        result = '[ ERROR - missing icon line ]'
        return result

    @classmethod
    def epilogue( cls ):
        return ''

    @classmethod
    def convert( cls ):
        result = cls.prelude() + cls.lines_minmax()
        for icon in cls.intermediate.get( 'icons' ):
            line_icon = cls.line_icon( icon )
            result += line_icon
        result += cls.epilogue()
        print( 'Converted - {!s} for {!s}'.format( cls.intermediate.get( 'font_name' ), cls.language_name ))
        return result

    @classmethod
    def save_to_file( cls ):
        filename = cls.file_name.format( name = str( cls.intermediate.get( 'font_name' )).replace( ' ', '' ))
        converted = cls.convert()
        with open( filename, 'w' ) as f:
            f.write( converted )
            f.close()
        print( 'Saved - {!s}'.format( filename ))


class LanguageC( Language ):
    language_name = 'C and C++'
    file_name = 'Icons{name}.h'

    @classmethod
    def prelude( cls ):
        tmpl_prelude = '// Generated for NeoDoa\n' + \
            '// from {font_data}\n' + \
            '// for use with {ttf_files}\n' + \
            '#pragma once\n\n'
        ttf_files = []
        for ttf in cls.intermediate.get( 'ttfs' ):
            ttf_files.append( ttf[ 2 ])
        result = tmpl_prelude.format( lang = cls.language_name,
                                      font_data = cls.intermediate.get( 'font_data' ),
                                      ttf_files = ', '.join( ttf_files ))
        tmpl_prelude_define_file_name = '#define FONT_ICON_FILE_NAME_{font_abbr} "{file_name_ttf}"\n'
        for ttf in cls.intermediate.get( 'ttfs' ):
            result += tmpl_prelude_define_file_name.format( font_abbr = ttf[ 0 ], file_name_ttf = ttf[ 1 ])
        return result + '\n'

    @classmethod
    def lines_minmax( cls ):
        tmpl_line_minmax = '#define ICON_{minmax}_{abbr} 0x{val}\n'
        abbreviation = cls.intermediate.get( 'font_minmax_abbr' ) if cls.intermediate.get( 'font_minmax_abbr' ) else cls.intermediate.get('font_abbr')
        result = tmpl_line_minmax.format( minmax = 'MIN',
                                          abbr = abbreviation,
                                          val = cls.intermediate.get( 'font_min' )) + \
                 tmpl_line_minmax.format( minmax = 'MAX_16',
                                          abbr = abbreviation,
                                          val = cls.intermediate.get( 'font_max_16' )) + \
                 tmpl_line_minmax.format( minmax = 'MAX',
                                          abbr = abbreviation,
                                          val = cls.intermediate.get( 'font_max' ))
        return result

    @classmethod
    def line_icon( cls, icon ):
        tmpl_line_icon = '#define ICON_{abbr}_{icon} "{code}"\t// U+{unicode}\n'
        icon_name = str.upper( icon[ 0 ]).replace( '-', '_' )
        icon_code = repr( chr( int( icon[ 1 ], 16 )).encode( 'utf-8' ))[ 2:-1 ]
        result = tmpl_line_icon.format( abbr = cls.intermediate.get( 'font_abbr' ),
                                        icon = icon_name,
                                        code = icon_code,
                                        unicode =icon[ 1 ] )
        return result

    @classmethod
    def convert_ttf_to_header( cls ):
        for ttf in cls.intermediate.get( 'ttfs' ):
            # retrieve and read ttf file
            if 'http' in ttf[ 2 ]:
                # download and read (if file is on GitHub, add '?raw=true')
                response = requests.get( ttf[ 2 ] + '?raw=true' if 'github.com' in ttf[ 2 ] else ttf[ 2 ], timeout = 2 )
                if response.status_code == 200:
                    ttf_data = response.content
                    print( 'ttf file downloaded - ' + ttf[ 1 ] )
                else:
                    raise Exception( 'ttf file missing - ' + ttf[ 2 ])
            else:
                # open from disk and read
                if os.path.isfile( ttf[ 2 ] ):
                    with open( ttf[ 2 ], 'rb' ) as f:
                        ttf_data = f.read()
                        f.close()
                        print( 'ttf file read - ' + ttf[ 1 ])
                else:
                    raise Exception( 'ttf file missing - ' + ttf[ 2 ])
            # convert to header and save to disk
            if ttf_data:
                # convert ttf to header
                tmpl_prelude_ttf = '// Generated for NeoDoa\n' + \
                                   '// from {ttf_file}\n' + \
                                   '// Requires #include <stdint.h>\n' + \
                                   '#pragma once\n\n' + \
                                   'static const uint8_t s_{name}_ttf[{size}] = \n{{'
                result = tmpl_prelude_ttf.format( lang = cls.language_name,
                                                  ttf_file = ttf[ 2 ],
                                                  name = str( ttf[ 1 ][ :-len('.ttf') ].replace( '-', '_' ).replace( ' ', '' )),
                                                  size = str( len( ttf_data )))
                n = 0
                for byte in ttf_data:
                    if (n % 16) == 0:
                        result += '\n\t'
                    result += "0x" + str( hex( int( byte / 16 ))[ 2: ]) + str( hex( byte % 16 )[ 2: ]) + ", "
                    n += 1
                result += '\n};\n\n'
                # save to disk
                ttf_header_file_name = cls.file_name.format( name = str( cls.intermediate.get( 'font_name' )).replace( ' ', '' )) + '_' + ttf[ 1 ] + '.h'
                with open( ttf_header_file_name, 'w' ) as f:
                    f.write( result )
                    f.close()
                print( 'ttf File Saved - {!s}'.format( ttf_header_file_name ))
            else:
                raise Exception( 'Failed ttf to header conversion' + ttf[ 1 ] )

# Main
fonts = [ FontFA6, FontFA6Brands, FontFA6Pro, FontFA6ProBrands ]
languages = [ LanguageC ]
ttf2headerC = False # convert ttf files to C and C++ headers

intermediates = []
for font in fonts:
    try:
        font_intermediate = font.get_intermediate_representation()
        if font_intermediate:
            intermediates.append( font_intermediate )
    except Exception as e:
        print( '[ ERROR: {!s} ]'.format( e ))
if intermediates:
    for interm in intermediates:
        Language.intermediate = interm
        for lang in languages:
            if lang:
                lang.save_to_file()
                if ttf2headerC and lang == LanguageC:
                    try:
                        lang.convert_ttf_to_header()
                    except Exception as e:
                        print( '[ ERROR: {!s} ]'.format( e ))
