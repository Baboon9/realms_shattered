#include "combat.hpp"

int Combat::player_unique_id_get( )
{ 
  return m_player->unique_id_get(); 
}

Combat::Combat(Actor *player, Actor *enemy): m_player { player }, m_enemy { enemy } { }

bool Combat::await_enemy_command_get() 
{ 
  return m_await_enemy_commad; 
}

void Combat::await_enemy_command_set( bool b )
{ 
  m_await_enemy_commad = b; 
}

void Combat::await_input_set( bool b )
{ 
  m_await_input = b; 
}

void Combat::add_command(Command *command)
{
  command->delay_set(m_step);
  m_command_queue.push_back(command);
}

void Combat::next_step( GameState *game_state, const std::string NAME_RIFT_STATE, const std::string name_room )
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
    game_state->redraw(NAME_RIFT_STATE, name_room);
  }

  if( !m_player->is_alive_get() || !m_enemy->is_alive_get() ) {
    is_over = true;
  }
}

Combat::~Combat()
{
  for(auto iter = m_command_queue.begin(); iter != m_command_queue.end(); ++iter ) {
    delete (*iter);
  }
}

bool Combat::await_input_get() 
{ 
  return m_await_input; 
}

bool Combat::is_over_get() 
{ 
  return is_over; 
}
