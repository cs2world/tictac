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
/*
bool tictac3::is_empty_cell(uint32_t _board, uint16_t x, uint16_t y){
  uint16_t position = x + 3 * y;
  uint16_t value_on_position = _board & CELL_MASK[ position ];
  if( value_on_position == 0) return true;
  //"the cell is taken, please try other cells."); 
  return false;
}*/

// return value 0: no winner; 1: winner is host; 2: winner is challenger; 3: draw
uint8_t tictac3::get_winner(uint32_t _board){
  uint32_t win_pattern;
  for(int i=0; i<8; i++) {
    win_pattern = _board & CHECK_WIN_PATTERN[ i ];
    if(win_pattern ==       HOST_WIN_PATTERN[ i ]) return 1; // winner is host.
    if(win_pattern == CHALLENGER_WIN_PATTERN[ i ]) return 2; // chanllenger wins.
  }
  return 0; // no winner
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
    game.board = 0;
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
  // otherwise the challenger as player_host
  game_itr = _games.find(challenger.value);
  //auto game_itr = _games.begin();
  if (game_itr != _games.end() && game_itr->player_challenger == host ) {
    game_itr = _games.erase(game_itr);
    return;
  }
  // the record is not found either way h->>c, or c->>h
  check( false, "No game record found for you!");
}

/**
** find the player in host/primary index or challenger/2nd index
** then update the game record to the initial state 
*/
ACTION tictac3::restart(name player){
  check( has_auth( player), "You shold has the permission to play.");
  
  games_table _games(get_self(), get_self().value);
  auto primary_itr = _games.find(player.value);

  auto idx = _games.get_index<name("bychallenger")>();
  auto index_itr = idx.find(player.value);
  
  if( (primary_itr != _games.end()) && (index_itr != idx.end()) ){
    check( false, "You have more than one games....");
  }

  if( (primary_itr != _games.end()) && (index_itr == idx.end()) ){ // is a host
      _games.modify(primary_itr, get_self(), [&] (auto& this_game){
      this_game.board = 0; // clear the board
      this_game.winner = 0;
      this_game.turns = 0;
      return;
    });
  }
  
  if( (primary_itr == _games.end()) && (index_itr != idx.end()) ){ // is a challenger
    idx.modify(index_itr, get_self(), [&](auto& this_game){
      this_game.board = 0; // clear the board
      this_game.winner = 0;
      this_game.turns = 0;
      return;
    });
  }

  if( (primary_itr == _games.end()) && (index_itr == idx.end()) ){
    check( false, "You have no game to restart....");
  }

}

ACTION tictac3::move( name player, uint16_t x, uint16_t y){
  check( has_auth( player), "You shold has the permission to play.");
  // x, y has three valid values [0,1,2], any value>2 will be set=2.
  if( x > 2 ) x = 2;
  if( y > 2 ) y = 2;
  uint16_t position = x + 3 * y;
  // find the game record first in the games table
  games_table _games(get_self(), get_self().value);
  auto primary_itr = _games.find(player.value);

  auto idx = _games.get_index<name("bychallenger")>();
  auto index_itr = idx.find(player.value);
  
  if( (primary_itr != _games.end()) && (index_itr != idx.end()) ){
    check( false, "You have more than one games. one game at a time, please.");
  }

  if( (primary_itr == _games.end()) && (index_itr == idx.end()) ){
    check( false, "You have no game to for the move....");
  }

  if( (primary_itr != _games.end()) && (index_itr == idx.end()) ){ // is a host
    check(primary_itr->winner == 0, "Game over: Winner=1: Host; 2: Challenger; 3: Draw");
    uint8_t _turns = primary_itr->turns; 
    
    check( _turns % 2 == 1, "This is the Challenger's turn!" );
    
    check( (primary_itr->board & CELL_MASK[ position ] ) == 0, "This cell is taken.");
    _games.modify(primary_itr, get_self(), [&](auto& this_game) {
      this_game.board |= SET_CELL_HOST[ position ]; // cell is taken by host
      this_game.winner = get_winner(this_game.board);
      this_game.turns++;
      if(this_game.turns >= 8) this_game.winner = 3; // it is a tie game
    });
  }

 if( (primary_itr == _games.end()) && (index_itr != idx.end()) ){ // is a challenger 
    check(index_itr->winner == 0, "Game over: Winner=1: Host; 2: Challenger; 3: Draw");
    uint8_t _turns = index_itr->turns; 
    check( _turns % 2 == 0, "This is the Host's turn!" );
  
    check( ( index_itr->board & CELL_MASK[ position ] ) == 0, "This cell is taken.");
    idx.modify(index_itr, get_self(), [&](auto& this_game){  // idx.modify()????!!!
      this_game.board |= SET_CELL_CHALLENGER[ position ]; // cell is taken by challenger
      this_game.winner = get_winner(this_game.board);
      this_game.turns++;
      if(this_game.turns >= 8) this_game.winner = 3; // it is a draw
    });
  }
  
}

EOSIO_DISPATCH(tictac3, (create)(close)(move)(restart))
