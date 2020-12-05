#include "combat.hpp"

int Combat::player_unique_id_get( )
{ 
  return m_player->unique_id_get(); 
}

void Combat::init(Actor *player, Actor *enemy){
  m_player = player;
  m_enemy = enemy;
}

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

void Combat::next_step()
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
    m_redraw = true;
  }

  if( !m_player->is_alive_get() || !m_enemy->is_alive_get() ) {
    m_is_over = true;
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
  return m_is_over; 
}

bool Combat::redraw_get()
{
  return m_redraw;
}

void Combat::redraw_set(bool b)
{
  m_redraw = b;
}

Actor* Combat::player_get()
{
  return m_player;
}

Actor* Combat::enemy_get()
{
  return m_enemy;
}

void is_over_set( bool b ) 
{ 
  m_is_over = b;
}
