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
    bool m_is_over{ false };
    int m_step{ 0 };
    bool m_await_input{ true };
    bool m_await_enemy_commad{ true };
    int m_next_input{ 0 };
    int m_next_enemy_command{ 0 };
    std::vector <Command*> m_command_queue;
    int m_input_step{ 0 };
    Actor *m_player;
    Actor *m_enemy;
    bool m_redraw{ false };
  public:
    void is_over_set( bool b );
    Actor* player_get( );
    Actor* enemy_get( );
    bool redraw_get( );
    void redraw_set( bool b );
    int player_unique_id_get( );
    Combat() = default;
    void init(Actor *player, Actor *enemy);
    bool await_enemy_command_get();
    void await_enemy_command_set( bool b );
    void await_input_set( bool b );
    void add_command(Command *command);
    void next_step();
    ~Combat();
    bool await_input_get();
    bool is_over_get();
};

#endif
