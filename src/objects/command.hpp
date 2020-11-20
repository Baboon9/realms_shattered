#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "../entities/actor.hpp"
#include "../engine_systems/action_log.hpp"
#include "combat.hpp"

class Combat;

class Command
{
  protected:
    int m_delay{ 0 };
    Actor *m_origin;
    Actor *m_target;
    ActionLog &m_action_log;
    Combat *m_combat;
  public: 
    Command( Actor *origin, Actor *target, ActionLog &action_log, Combat *combat );
    virtual ~Command() { }
    virtual void execute( ) = 0;
    virtual void delay_set( int step ) = 0;
    int delay_get( );
};

class Attack: public Command
{
  public:
    Attack( Actor *origin, Actor *target, ActionLog &action_log, Combat *combat );
    void execute() override; 
    void delay_set(int step) override; 
};

#endif
