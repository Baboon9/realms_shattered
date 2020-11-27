#include "command.hpp"


Command::Command( Actor *origin, Actor *target, ActionLog &action_log, Combat *combat ): m_combat{ combat }, m_origin{ origin }, m_target{ target }, m_action_log{ action_log } 
{ }

int Command::delay_get( ) 
{
  return m_delay; 
}


Attack::Attack( Actor *origin, Actor *target, ActionLog &action_log, Combat *combat ): Command{ origin, target, action_log, combat } { }

void Attack::execute() 
{
  int origin_damage = m_origin->damage_get();
  m_target->health_current_set( m_target->health_current_get() - origin_damage );
  m_action_log.add_line(m_origin->name_get() + " attacks " + m_target->name_get() + " for " + std::to_string( origin_damage ) );
      
  if( m_origin->unique_id_get() == Command::m_combat->player_unique_id_get() ) {
    Command::m_combat->await_input_set(true);
  } else {
    Command::m_combat->await_enemy_command_set(true);
  }
}

void Attack::delay_set(int step)
{
  int origin_speed{ 10 };
  Command::m_delay = step + 100 - origin_speed; 
}

Bash::Bash( Actor *origin, Actor *target, ActionLog &action_log, Combat *combat ): Command{ origin, target, action_log, combat } { }

void Bash::execute()
{
  int origin_damage = m_origin->damage_get();
  m_target->health_current_set( m_target->health_current_get() - origin_damage * 3 );
  m_action_log.add_line( m_origin->name_get() + " bashes " + m_target->name_get() + " for " + std::to_string( origin_damage * 3 ) );
  
  if( m_origin->unique_id_get() == Command::m_combat->player_unique_id_get() ) {
    Command::m_combat->await_input_set(true);
  } else {
    Command::m_combat->await_enemy_command_set(true);
  }
}

void Bash::delay_set(int step)
{
  int origin_speed{ 10 };
  Command::m_delay = step + ( 100 - origin_speed ) * 3; 
}
