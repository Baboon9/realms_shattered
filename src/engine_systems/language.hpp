
#ifndef _LANGUAGE_HPP_
#define _LANGUAGE_HPP_

#include <map>
#include <string>

#include "command_tag.hpp"

class Language final {
public:
   /*
      Load list of commands in specified (language) file.

      Ex: commands_load( "data/language/en_commands.txt" )
   */
   const bool commands_load( const std::string &file_name );
   
   /*
      Load list of game text in specified (language) file.

      Ex: text_load( "data/language/en_game_text.txt" )
   */
   const bool text_load( const std::string &file_name );

   /*
      Returns true if 'command_tag' is found in commands_map, and returns the CommandTag paired with the found key.
   */
   const CommandTag command_tag_get( const std::string &player_input );

   /*
      Returns true if 'text_tag' is found in text_map, and returns the game text associated with the text_tag key.
   */
   const std::string text_tag_get( const std::string &text_tag );

private:
   std::map <std::string, CommandTag> commands_map;
   std::map <std::string, std::string> text_map;
};

#endif // _LANGUAGE_HPP_
