#include <eosio/eosio.hpp>

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
    void are_they_in_game(name player1, name player2);

  private:
    TABLE game_record {
      name  player_host;
      name  player_challenger;

      uint64_t primary_key() const { return player_host.value; }
      // add the second index
      uint64_t second_key() const { return player_challenger.value; }

      EOSLIB_SERIALIZE( game_record, (player_host)(player_challenger) )
    };
    typedef multi_index<name("games"), game_record, eosio::indexed_by<
      name("bychallenger"), eosio::const_mem_fun<
      game_record, uint64_t, &game_record::second_key
      >
      >
      > games_table;
};

