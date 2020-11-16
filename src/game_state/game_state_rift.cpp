
#include "game_state.hpp"

#include "../engine_systems/logger.hpp"
#include "../entities/actor.hpp"
#include "../objects/rift.hpp"
#include <vector>
#include <random>

class Command
{
  protected:
    int m_delay{ 0 };
    Actor *m_origin;
    Actor *m_target;
  public: 
    Command(Actor *origin, Actor *target): m_origin{ origin }, m_target{ target } { }
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void delay_set(int step) = 0;
    int delay_get() { return m_delay; }
};

class Attack: public Command
{
  public:
    Attack( Actor *origin, Actor *target ): Command{ origin, target } { }
    void execute() override 
    {
      int origin_damage = m_origin->damage_get();
      m_target->health_current_set( m_target->health_current_get() - origin_damage );
    }
    void delay_set(int step) override 
    {
      int origin_speed{ 10 };
      Command::m_delay = step + 100 - origin_speed; 
    }
};

class Combat
{
  private:
    bool is_over{ false };
    int m_step{ 0 };
    bool m_await_input{ true };
    bool m_await_enemy_commad{ true };
    std::vector <Command*> m_command_queue;
    int m_input_step{ 0 };
    Actor *m_player;
    Actor *m_enemy;
  public:
    Combat(Actor *player, Actor *enemy): m_player { player }, m_enemy { enemy } { }
    bool await_enemy_command_get() { return m_await_enemy_commad; }
    void add_command(Command *command)
    {
      command->delay_set(m_step);
      m_command_queue.push_back(command);
    }
    void next_step()
    {
      ++m_step;

      std::vector<Command*> next_command;
      for( auto iter = m_command_queue.begin(); iter != m_command_queue.end(); ++iter ) {
        if( ( *iter )->delay_get( ) == m_step ) {
          next_command.push_back( ( *iter ) );  
        }
      }
      for( auto iter = next_command.begin(); iter != next_command.end(); ++iter ) {
        (*iter)->execute();
      }

      if( m_player->is_alive_get() || m_enemy->is_alive_get() ) {
        is_over = true;
      }
    }
    ~Combat()
    {
      for(auto iter = m_command_queue.begin(); iter != m_command_queue.end(); ++iter ) {
        delete (*iter);
      }
    }
    bool await_input_get() { return m_await_input; }
    bool is_over_get() { return is_over; }
};

CommandTag GameState::redraw_and_get_input(const std::string NAME_RIFT_STATE, const std::string name_room)
{ 
   m_console.print_box( 0, 0, m_console.width_get(), m_console.height_get(), '+' );
   m_console.print( NAME_RIFT_STATE, ( ( m_console.width_get() / 2 ) - ( NAME_RIFT_STATE.length() / 2 ) ), 1 );
   m_console.print( name_room, ( ( m_console.width_get() / 2 ) - ( name_room.length() / 2 ) ), 2 );
   
   m_user_interface.player_stats_brief_display( m_console, m_language, m_game_data );
   m_user_interface.action_log_display( m_action_log, m_console );
   m_user_interface.command_prompt_display( m_console, m_language );
   return CommandTag( m_user_input.player_command_get( m_language ) );
}

void GameState::game_state_rift()
{
   static const std::string NAME_RIFT_STATE = "The Rifts";
   std::string name_room( "Room INVALID" );

   Rift * rift( m_game_data.rift_get() );
   if( rift != nullptr ) {
      name_room = rift->m_rooms[ rift->m_room_current ].m_name;
   }

   
   static bool monster_seen_message_displayed( false );
   if( rift != nullptr &&
       !rift->m_rooms[ rift->m_room_current ].m_entity_list.empty() ) {
      if( !monster_seen_message_displayed ) {
         m_action_log.add_line( "You see " + rift->m_rooms[ rift->m_room_current ].m_entity_list[ 0 ]->name_get() + " before you." );
         monster_seen_message_displayed = true;
      }
   }
    
   //CommandTag command_tag = redraw_and_get_input();
   CommandTag command_tag;

   Combat *combat{ nullptr };
   Actor *enemy{ nullptr };
   Actor *player{ nullptr };

   bool rift_exists{ rift != nullptr };
   bool room_not_empty{ rift->m_rooms[ rift->m_room_current ].m_entity_list.empty() };
   if( rift_exists && room_not_empty ){
      //Check for existing enemy and populate enemy
      bool enemy_exists = m_game_data.actor_get( rift->m_rooms[ rift->m_room_current ].m_entity_list[ 0 ]->unique_id_get(), enemy );
      if( enemy_exists ){
        player = m_game_data.player_get();
        combat = new Combat{ player, enemy };
        Logger(LoggerLevel::LOG_LEVEL_INFO).log() << "New combat created with " << player->name_get() << " and " << enemy->name_get();
      }
   }
   
   bool combat_exists{ combat != nullptr };
   if( combat_exists ) {
      while( true ){
         combat->next_step();
         if( combat->is_over_get() ){
           if( !player->is_alive_get() ) {
             m_action_log.add_line( m_game_data.player_get()->name_get() + " has been struck down by " + enemy->name_get() );
             m_game_data.rift_destroy();
             m_game_state_current = GameStateEnum::GAME_STATE_PLAYER_HUB;
           }
           if( !enemy->is_alive_get() ) {
             m_action_log.add_line( m_game_data.player_get()->name_get() + " has defeated " + enemy->name_get() );
             m_game_data.entity_destroy( enemy->unique_id_get() );
             rift->m_rooms[ rift->m_room_current ].m_entity_list.erase( rift->m_rooms[ rift->m_room_current ].m_entity_list.begin() );
             monster_seen_message_displayed = false;
           }
           break;
         }
         if( combat->await_input_get() ) {
            command_tag = redraw_and_get_input(NAME_RIFT_STATE, name_room);
            handle_input(command_tag, rift, combat, player, enemy, name_room);
            m_console.clear();
         }
         if( combat->await_enemy_command_get() ) {
            constexpr int num_awailable_commands{ 1 };
            int picked_command = rand() % 1;
            switch( picked_command ) {
              case 0 : combat->add_command(new Attack{ enemy, player });
                       Logger( LoggerLevel::LOG_LEVEL_PROGRESS ) .log() << "New command added " << enemy->name_get() << " attacks " << player->name_get() ); 
                       break;
              default : Logger( LoggerLevel::LOG_LEVEL_ERROR ).log() << "During combat: Enemys random command invalid"; 
            }
         }
      }
   }

    /*
   // If any entity occupies the current room and player has acted, allow entities their turn.
   rift = m_game_data.rift_get();
   if( rift != nullptr &&
       !rift->m_rooms[ rift->m_room_current ].m_entity_list.empty() &&
       player_turn_taken ) {
      Actor *actor( nullptr );
      if( m_game_data.actor_get( rift->m_rooms[ m_action_log, rift->m_room_current ].m_entity_list[ 0 ]->unique_id_get(), actor ) ) {
         actor->attack( m_action_log, m_game_data.player_get()->unique_id_get() );

         if( !m_game_data.player_get()->is_alive_get() ) {
            m_action_log.add_line( m_game_data.player_get()->name_get() + " has been struck down be " + actor->name_get() );
            m_game_data.rift_destroy();
            m_game_state_current = GameStateEnum::GAME_STATE_PLAYER_HUB;
         }
      }
   }

   */
      
   //m_console.clear();
}

void GameState::handle_input(CommandTag command_tag, Rift *rift, Combat *combat, Actor *player, Actor *enemy, const std::string name_room)
{
   switch( command_tag ) {
      case CommandTag::COMMAND_INVALID:
         break;
      case CommandTag::COMMAND_ATTACK:
         combat->add_command(new Attack(player, enemy));
         Logger(LoggerLevel::LOG_LEVEL_PROGRESS).log() << "New command added " << player->name_get() << " attacks " << enemy->name_get();
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
