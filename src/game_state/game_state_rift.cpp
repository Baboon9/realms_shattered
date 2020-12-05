
#include "game_state.hpp"

#include "../engine_systems/logger.hpp"
#include "../entities/actor.hpp"
#include "../objects/rift.hpp"
#include <random>
#include "../objects/command.hpp"
#include "../objects/combat.hpp"

CommandTag GameState::redraw_and_get_input(const std::string NAME_RIFT_STATE, const std::string name_room)
{ 
   redraw(NAME_RIFT_STATE, name_room);
   return CommandTag( m_user_input.player_command_get( m_language ) );
}

void GameState::redraw( const std::string NAME_RIFT_STATE, const std::string name_room )
{
   m_console.clear();
   m_console.print_box( 0, 0, m_console.width_get(), m_console.height_get(), '+' );
   m_console.print( NAME_RIFT_STATE, ( ( m_console.width_get() / 2 ) - ( NAME_RIFT_STATE.length() / 2 ) ), 1 );
   m_console.print( name_room, ( ( m_console.width_get() / 2 ) - ( name_room.length() / 2 ) ), 2 );
   
   m_user_interface.player_stats_brief_display( m_console, m_language, m_game_data );
   m_user_interface.action_log_display( m_action_log, m_console );
   m_user_interface.command_prompt_display( m_console, m_language );
}

void GameState::game_state_rift()
{
   static const std::string NAME_RIFT_STATE = "The Rifts";
   std::string name_room( "Room INVALID" );

   Rift * rift( m_game_data.rift_get() );
   if( rift != nullptr ) {
      name_room = rift->m_rooms[ rift->m_room_current ].m_name;
   }
  
   m_console.print_box( 0, 0, m_console.width_get(), m_console.height_get(), '+' );
   m_console.print( NAME_RIFT_STATE, ( ( m_console.width_get() / 2 ) - ( NAME_RIFT_STATE.length() / 2 ) ), 1 );
   m_console.print( name_room, ( ( m_console.width_get() / 2 ) - ( name_room.length() / 2 ) ), 2 );
   
   static bool monster_seen_message_displayed( false );
   if( rift != nullptr &&
       !rift->m_rooms[ rift->m_room_current ].m_entity_list.empty() ) {
      if( !monster_seen_message_displayed ) {
         m_action_log.add_line( "You see " + rift->m_rooms[ rift->m_room_current ].m_entity_list[ 0 ]->name_get() + " before you." );
         monster_seen_message_displayed = true;
      }
   }
   
   m_user_interface.player_stats_brief_display( m_console, m_language, m_game_data );
   m_user_interface.action_log_display( m_action_log, m_console );
   m_user_interface.command_prompt_display( m_console, m_language );

   Actor *enemy{ nullptr };
   Actor *player{ nullptr };

   bool rift_exists{ rift != nullptr };
   bool room_empty{ rift->m_rooms[ rift->m_room_current ].m_entity_list.empty() };
   if( rift_exists && !room_empty ){
      //Check for existing enemy and populate enemy
      bool enemy_exists = m_game_data.actor_get( rift->m_rooms[ rift->m_room_current ].m_entity_list[ 0 ]->unique_id_get(), enemy );
      if( enemy_exists ){
        player = m_game_data.player_get();
        m_combat.init(player, enemy);
        Logger(LoggerLevel::LOG_LEVEL_INFO).log() << "New combat created with " << player->name_get() << " and " << enemy->name_get();
        m_game_state_current = GameStateEnum::GAME_STATE_COMBAT;
      }
   }

   CommandTag command_tag;
   bool command_is_set = false;
   if( room_empty ){
      command_tag = CommandTag{ m_user_input.player_command_get( m_language ) };
      command_is_set = true;
   }

   bool combat_over = m_combat.is_over_get();
   if( combat_over ) { 
      command_tag = CommandTag{ m_user_input.player_command_get( m_language ) };
      command_is_set = true;
      if( m_combat.player_get()->is_alive_get()){
	 monster_seen_message_displayed = false;
         m_game_data.entity_destroy( enemy->unique_id_get() );
         rift->m_rooms[ rift->m_room_current ].m_entity_list.erase( rift->m_rooms[ rift->m_room_current ].m_entity_list.begin() );
      }
   }
  
   if( command_is_set ){
   
      switch( command_tag ) {
        case CommandTag::COMMAND_INVALID:
           break;
        case CommandTag::COMMAND_MOVE:
           if( rift != nullptr ) {
              if( !rift->m_rooms[ rift->m_room_current ].m_entity_list.empty() ) {
                 m_action_log.add_line( "You cannot move on until the enemy has been defeated!" );
                 break;
              }

              ++rift->m_room_current;

              if( rift->m_room_current >= rift->m_room_count ) {
                 Logger( LoggerLevel::LOG_LEVEL_INFO ).log() << "Player has cleared the rift";
                 m_action_log.add_line( m_game_data.player_get()->name_get() + " has cleared the rift!" );
                 m_game_data.rift_destroy();
                 m_game_state_current = GameStateEnum::GAME_STATE_PLAYER_HUB;
                 redraw("the Rifts", name_room);
              } else {
                 Logger( LoggerLevel::LOG_LEVEL_INFO ).log() << "Player moved to room: " << rift->m_room_current + 1;
                 m_action_log.add_line( m_game_data.player_get()->name_get() + " has entered " + name_room );
              }
            }
          break;
        case CommandTag::COMMAND_QUIT:
           m_game_data.game_terminate();
           break;
      }
   }
   /* 
   bool combat_exists{ combat != nullptr };
   if( combat_exists ) {
      while( true ){
         m_console.sleep(10);
         combat->next_step(this, NAME_RIFT_STATE, name_room);
         if( combat->is_over_get() ){
           if( !player->is_alive_get() ) {
             m_action_log.add_line( m_game_data.player_get()->name_get() + " has been struck down by " + enemy->name_get() );
             m_game_data.rift_destroy();
             m_game_state_current = GameStateEnum::GAME_STATE_PLAYER_HUB;
           }
           if( !enemy->is_alive_get() ) {
             m_action_log.add_line( enemy->name_get() + " has been struck down by " + player->name_get() );
             m_game_data.entity_destroy( enemy->unique_id_get() );
             rift->m_rooms[ rift->m_room_current ].m_entity_list.erase( rift->m_rooms[ rift->m_room_current ].m_entity_list.begin() );
             monster_seen_message_displayed = false;
           }
           break;
         }
         if( combat->await_input_get() ) {
            command_tag = redraw_and_get_input(NAME_RIFT_STATE, name_room);
            handle_input(command_tag, rift, combat, player, enemy, name_room);
         }
         if( combat->await_enemy_command_get() ) {
            combat->await_enemy_command_set(false);
            constexpr int num_awailable_commands{ 1 };
            int picked_command = rand() % 1;
            switch( picked_command ) {
              case 0 : combat->add_command( new Attack{ enemy, player, &m_action_log, combat } );
                       Logger( LoggerLevel::LOG_LEVEL_PROGRESS ) .log() << "New command added " << enemy->name_get() << " attacks " << player->name_get(); 
                       break;
              default : Logger( LoggerLevel::LOG_LEVEL_ERROR ).log() << "During combat: Enemys random command invalid"; 
            }
         }
      }
   }
   */
   m_console.clear();
}

void GameState::handle_input(CommandTag command_tag, Rift *rift, Combat *combat, Actor *player, Actor *enemy, const std::string name_room)
{
   switch( command_tag ) {
      case CommandTag::COMMAND_INVALID:
         break;
      case CommandTag::COMMAND_ATTACK:
         if( enemy == nullptr ) {
            m_action_log.add_line( "There is nothing there to attack");
            break;
         }
         combat->add_command(new Attack{ player, enemy, &m_action_log, combat } );
         combat->await_input_set(false);
         Logger(LoggerLevel::LOG_LEVEL_PROGRESS).log() << "New command added " << player->name_get() << " attacks " << enemy->name_get();
         break;
      case CommandTag::COMMAND_BASH:
         if( enemy == nullptr ) {
            m_action_log.add_line( "There is nothing there to attack");
            break;
         }
         combat->add_command(new Bash{ player, enemy, &m_action_log, combat } );
         combat->await_input_set(false);
         Logger(LoggerLevel::LOG_LEVEL_PROGRESS).log() << "New command added " << player->name_get() << " bashs " << enemy->name_get();
         break;
      case CommandTag::COMMAND_MOVE:
         if( rift != nullptr ) {
            if( !rift->m_rooms[ rift->m_room_current ].m_entity_list.empty() ) {
               m_action_log.add_line( "You cannot move on until the enemy has been defeated!" );
               break;
            }

            ++rift->m_room_current;

            if( rift->m_room_current >= rift->m_room_count ) {
               Logger( LoggerLevel::LOG_LEVEL_INFO ).log() << "Player has cleared the rift";
               m_action_log.add_line( m_game_data.player_get()->name_get() + " has cleared the rift!" );
               m_game_data.rift_destroy();
               m_game_state_current = GameStateEnum::GAME_STATE_PLAYER_HUB;
               redraw("the Rifts", name_room);
            } else {
               Logger( LoggerLevel::LOG_LEVEL_INFO ).log() << "Player moved to room: " << rift->m_room_current + 1;
               m_action_log.add_line( m_game_data.player_get()->name_get() + " has entered " + name_room );
            }
         }
         break;
      case CommandTag::COMMAND_QUIT:
         m_game_data.game_terminate();
         break;
   }
}
