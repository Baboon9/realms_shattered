
#include "platform_linux.hpp"
#include "../engine_systems/logger.hpp"
#include <string>

#ifdef __linux__
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <bits/stdc++.h>
#include <unistd.h>

const std::string working_directory_get_linux()
{
   std::string working_directory;

   char buffer[ PATH_MAX ];
   getcwd( buffer, PATH_MAX );
   working_directory = buffer;

   return working_directory;
}

void console_buffer_dimensions_get_linux( int &console_buffer_width, int &console_buffer_height )
{
    // Obtain console size using ioctl.
   struct winsize screen_size;
   ioctl(STDOUT_FILENO, TIOCGWINSZ, &screen_size);

   console_buffer_width = screen_size.ws_col;
   console_buffer_height = screen_size.ws_row;
}

void console_clear_linux()
{
   system( "clear" );
}

// TODO (WraitheDX): Constrain cursor position to dimensions of the console window.
void console_cursor_position_set_linux( const int cursor_x, const int cursor_y )
{
    printf( "%c[%d;%df",0x1B,cursor_y,cursor_x );
}

void console_pause_linux()
{
   system( "read -rsp $'Press any key to continue . . .\n'" );
}

const bool get_file_list_linux( std::vector <std::string> &file_list, const std::string &file_path )
{
   DIR * dir = opendir( file_path.c_str() );
   struct dirent *entry;
   if( dir == NULL ) {
      Logger( LoggerLevel::LOG_LEVEL_ERROR ).log() << "get_file_list_linux() failed to find file in: " << file_path;
      return false;
   } else {
      bool file_found( false );
      while(dir) {
         if ( ( entry = readdir( dir ) ) != NULL ) {
            if( strncmp( entry->d_name, ".", 2 ) == 0 || strncmp( entry->d_name, "..", 2 ) == 0) {
               continue;
            }
            Logger( LoggerLevel::LOG_LEVEL_INFO ).log() << "get_file_list_linux found a file named: " << entry->d_name;
            file_list.push_back( entry->d_name );
            file_found = true;
         } else {
            closedir( dir );
            return file_found;

         }
      }
   }
   return false;
}

void folder_create_linux( const std::string folder_path )
{
   if( mkdir( folder_path.c_str(), 0777) == -1) {
     Logger( LoggerLevel::LOG_LEVEL_ERROR ).log() << "folder_create_linux() failed to create a folder.";
   }
}


const bool folder_exists_check_linux( const std::string folder_path )
{
   DIR * dir = opendir( folder_path.c_str() );
   if(dir) {
     return true;
   } else {
     return false;
   }
}

void console_sleep_linux( int milliseconds )
{
  unsigned int microseconds = 1000 * milliseconds;
  usleep(microseconds);
}
#endif // __linux__
