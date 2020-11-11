In order to scetch the flow of some aspekts of the game i thought it might be usefull to provide some sort of use-case scenarios.

This is how I imagine the combat to take place:

First iteration:
  There are two actors, the player and an enemy.
  Both have health and damage.
  The player attacks.
  He deals damage to his enemy acording to his damage output.
  The enemy attacks.
  He also deals damage to the player acording to his damage output.
  The player attacks again.
  ...
  The enemy dies and the combat is over.
  
Second iteration:
  There are two actors, the player and an enemy.
  Both have health, skill points, speed, damage and armor.
  The player has an inventory populated with weapons, armor and potions which he finds during the game.
  The enemy happens to be of type goblin.
  A counter starts which counts the steps of the combat.
  The player chooses to attack
  This action takes a number of steps until it is performed which is determined by his speed.
  The enemy rolls a die which happens to make him attack as well.
  Again this action takes some steps acording to the enemys speed.
  A number of steps pass.
  The players action is due.
  He deals damage according to his weapon and base damage and the armor value of the enemy.
  The player chooses to use a skill.
  He uses the skill bash which is slow but deals a lot of damage, which is effective against enemies with a high armor value.
  It checks weather the player has enough skill points and then substracts a portion of his points.
  Again some steps pass untill this action is performed.
  The enemies action is due.
  He attacked so he deals damage acording to his base damage and the players armor.
  The enemie rolls a die again and it happens that he uses a skill.
  This takes some steps,
  He uses the skill spit, which decreses the players speed.
  After the players action is due he chooses another skill.
  He chooses frenzy which again costs skill points.
  The skill frenzy lets the player attack in fast paced attacks which increase in damage output everytime untill he is hit. This is effective against slow enemies.
  The enemy happens to simply attack.
  The player uses an item from his inventory, a health potion, which increases his health points.
  The enemy attacks again.
  ...
  The enemy dies and the combat is over.
  
  
