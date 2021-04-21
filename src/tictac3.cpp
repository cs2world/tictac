#include <tictac3.hpp>

void tictac3::are_they_in_game(name player1, name player2){
  games_table _games(get_self(), get_self().value);

  auto itr = _games.find(player1.value);
  check ( itr == _games.end(), "player 1 already is a host!");
  itr = _games.find(player2.value);
  check ( itr == _games.end(), "player 2 already is a host!");

  // using 2nd index
  auto idx = _games.get_index<name("bychallenger")>();
  auto itr2 = idx.find(player1.value);
  check ( itr2 == idx.end(), "Player1 is already a challenger!");
  itr2 = idx.find(player2.value);
  check ( itr2 == idx.end(), "Player2 is alreday a challenger!") ;

 // when both players are not in both hosts and challengers, return.
  return;
}


ACTION tictac3::create( const name challenger, name host ) {
  require_auth(host);
  check( host != challenger, "you should not play against yourself!");
  check( is_account(challenger), "challenger is not a valid account!");

  are_they_in_game(host, challenger);
  // Init the _game table
  games_table _games(get_self(), get_self().value);
  _games.emplace(host, [&](auto& game) {
    game.player_host = host;
    game.player_challenger = challenger;
    game.turns = 0;
    game.winner = 0;
  });
}


ACTION tictac3::close( const name challenger, const name host ) {
  check( has_auth( host) || has_auth(challenger), "You don't have the right to close game" );
  // confirm they are both on one game record
  games_table _games(get_self(), get_self().value);

  // Delete all records in _games table
  auto game_itr = _games.find(host.value);
  //auto game_itr = _games.begin();
  if (game_itr != _games.end() && game_itr->player_challenger == challenger ) {
    game_itr = _games.erase(game_itr);
    return;
  }
  // other challenger as player_host
  game_itr = _games.find(challenger.value);
  //auto game_itr = _games.begin();
  if (game_itr != _games.end() && game_itr->player_challenger == host ) {
    game_itr = _games.erase(game_itr);
    return;
  }
  // the record is not found either way h->>c, or c->>h
  check( 1 == 0, "No game record found for them!");
}

EOSIO_DISPATCH(tictac3, (create)(close))
