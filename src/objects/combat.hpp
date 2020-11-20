#ifndef COMBAT_HPP
#define COMBAT_HPP

#include "../entities/actor.hpp"
#include "command.hpp"
#include <vector>
#include "../game_state/game_state.hpp"
#include <string>

class Command;

class Combat
{
  private:
    bool is_over{ false };
    int m_step{ 0 };
    bool m_await_input{ true };
    bool m_await_enemy_commad{ true };
    int m_next_input{ 0 };
    int m_next_enemy_command{ 0 };
    std::vector <Command*> m_command_queue;
    int m_input_step{ 0 };
    Actor *m_player;
    Actor *m_enemy;
  public:
    int player_unique_id_get( );
    Combat(Actor *player, Actor *enemy);
    bool await_enemy_command_get();
    void await_enemy_command_set( bool b );
    void await_input_set( bool b );
    void add_command(Command *command);
    void next_step(GameState *game_state, const std::string NAME_RIFT_STATE, const std::string name_room);
    ~Combat();
    bool await_input_get();
    bool is_over_get();
};

#endif
