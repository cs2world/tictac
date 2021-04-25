#include <eosio/eosio.hpp>
//#include <eoslib/time.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

/*
** this one add the second index
** and change the sequence of host and chanllenger, 
*/

CONTRACT tictac3 : public contract {
  public:
    using contract::contract;

    ACTION create( const name challenger, name host);
    ACTION close( const name challenger, const name host );
    // just close the game no check
    ACTION move( name player, uint16_t x, uint16_t y); 
    ACTION restart( const name player ); // as one account only can play ONE game at one time,
    // the player can unique find one game or no game at all

    // only 0, 1, 2 are valid inputs, any number > 2 will be set to 2
    
  private:
   
   /*
   ** get a pattern:
   ** pattern = board & CHECK_WIN_PATTERN[i]
   ** if pattern == HOST_WIN_PATTERN[i], host wins
   ** if pattern == CHALLENGER_WIN_PATTERN[i] challenger wins.
   */
    const uint32_t CHECK_WIN_PATTERN[8] = {
      0b111111000000000000,  0b000000111111000000,   0b000000000000111111, //  for 3 rows
      0b110000110000110000,  0b001100001100001100,   0b000011000011000011,  //for 3 coloums
      0b110000001100000011,   0b000011001100110000 //for 2 diagnoals
    };

    const uint32_t HOST_WIN_PATTERN[8] = {
      0b010101000000000000,  0b000000010101000000,  0b000000000000010101,
      0b010000010000010000,  0b000100000100000100,  0b000001000001000001,  //for 3 coloums
      0b010000000100000001,  0b000001000100010000  //for 2 diagnoals
    };

    const uint32_t CHALLENGER_WIN_PATTERN[8] = {
      0b101010000000000000,  0b000000101010000000,  0b000000000000101010,
      0b100000100000100000,  0b001000001000001000,  0b000010000010000010,
      0b100000001000000010,  0b000010001000100000
    };

 /* define the winner pattern
    there are 8 patterns for a check_winner
    if board & CELL_MASK[i] == 0, cell i is empty, 
    mark cell i by host:
    board |= SET_CELL_HOST[i];
    mark cell i by challenger
    board |= SET_CELL_CHALLENGER[i];
    */
    const uint32_t CELL_MASK[9]={
      0b110000000000000000,  
      0b001100000000000000,  
      0b000011000000000000,
      0b000000110000000000,  
      0b000000001100000000,  
      0b000000000011000000,
      0b000000000000110000,  
      0b000000000000001100,  
      0b000000000000000011
    };
    // use for test empty position
    const uint32_t SET_CELL_HOST[9]={
      0b010000000000000000,  
      0b000100000000000000,  
      0b000001000000000000,
      0b000000010000000000,  
      0b000000000100000000,  
      0b000000000001000000,
      0b000000000000010000,  
      0b000000000000000100,  
      0b000000000000000001
    };
    const uint32_t SET_CELL_CHALLENGER[9]={
      0b100000000000000000,  
      0b001000000000000000,  
      0b000010000000000000,
      0b000000100000000000,  
      0b000000001000000000,  
      0b000000000010000000,  
      0b000000000000100000,  
      0b000000000000001000,  
      0b000000000000000010
    };

    time_point WAITING_TIME = (time_point) minutes(1); // 1 minutes

    TABLE game_record {
      name  player_host;
      name  player_challenger;
      
      uint32_t board;  // use 2 bits of data as one grid 00: empty; 01: host; 10: challenger; 11: illegal
      // total 9 positions, 18 bits, using uint32_t should be good. 

      uint8_t turns; // even#: challenger's turn; each valid move, turns++; odd#: host's turns 
      uint8_t winner; // 0: no winner; 1: the host wins; 2: the challenger wins; 3: draw
      time_point last_move_tp; // the time elapsed since epoch

      uint64_t primary_key() const { return player_host.value; }
      // add the second index
      uint64_t second_key() const { return player_challenger.value; }

      EOSLIB_SERIALIZE( game_record, (player_host)(player_challenger)(board)(turns)(winner)(last_move_tp))
    };
    typedef multi_index<name("games"), game_record, eosio::indexed_by<
      name("bychallenger"), eosio::const_mem_fun<
      game_record, uint64_t, &game_record::second_key
      >
      >
      > games_table;

    void are_they_in_game(name player1, name player2); // if in private, should work!?
    /*bool is_empty_cell(uint32_t _board, uint16_t x, uint16_t y);*/
    
    uint8_t get_winner(uint32_t a_board); 
    // return value 0: no winner; 1: winner is host; 2: winner is challenger; 3: draw
};

