#include "./game_state.hpp"
#include "../objects/rift.hpp"
#include "../engine_systems/logger.hpp"
#include "../objects/command.hpp"

void GameState::game_state_combat(){
  
   static const std::string NAME_RIFT_STATE = "The Rifts";
   std::string name_room( "Room INVALID" );

   Rift * rift( m_game_data.rift_get() );
   if( rift != nullptr ) {
      name_room = rift->m_rooms[ rift->m_room_current ].m_name;
   }

    if(m_combat.redraw_get()) {
    m_console.clear();

    m_console.print_box( 0, 0, m_console.width_get(), m_console.height_get(), '+' );
    m_console.print( NAME_RIFT_STATE, ( ( m_console.width_get() / 2 ) - ( NAME_RIFT_STATE.length() / 2 ) ), 1 );
    m_console.print( name_room, ( ( m_console.width_get() / 2 ) - ( name_room.length() / 2 ) ), 2 );
   
    m_user_interface.player_stats_brief_display( m_console, m_language, m_game_data );
    m_user_interface.action_log_display( m_action_log, m_console );
    m_user_interface.command_prompt_display( m_console, m_language );
  }
  
  bool has_input{ false };
  CommandTag command_tag;
  if(m_combat.await_input_get()) {
    command_tag = { m_user_input.player_command_get( m_language ) };
  }

  Actor *player = m_combat.player_get();
  Actor *enemy = m_combat.enemy_get();
  if(!m_combat.is_over_get()) {
    m_console.sleep(10);
    m_combat.next_step();
  } else {
    if(player->is_alive_get()){
      m_action_log.add_line( player->name_get() + " has been struck down by " + enemy->name_get() );
      m_game_data.rift_destroy();
      m_game_state_current = GameStateEnum::GAME_STATE_PLAYER_HUB;
    }
    if(!m_combat.enemy_get()->is_alive_get()){
      m_action_log.add_line( enemy->name_get() + " has been struck down by " + player->name_get() );
      m_game_state_current = GameStateEnum::GAME_STATE_RIFT;
    }
  }

  if(has_input){
   switch( command_tag ) {
      case CommandTag::COMMAND_INVALID:
         break;
      case CommandTag::COMMAND_ATTACK:
         if( enemy == nullptr ) {
            m_action_log.add_line( "There is nothing there to attack");
            break;
         }
         m_combat.add_command(new Attack{ player, enemy, &m_action_log, &m_combat } );
         m_combat.await_input_set(false);
         Logger(LoggerLevel::LOG_LEVEL_PROGRESS).log() << "New command added " << player->name_get() << " attacks " << enemy->name_get();
         break;
      case CommandTag::COMMAND_BASH:
         if( enemy == nullptr ) {
            m_action_log.add_line( "There is nothing there to attack");
            break;
         }
         m_combat.add_command(new Bash{ player, enemy, &m_action_log, &m_combat } );
         m_combat.await_input_set(false);
         Logger(LoggerLevel::LOG_LEVEL_PROGRESS).log() << "New command added " << player->name_get() << " bashs " << enemy->name_get();
         break;
      }
   }
}
